// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mongocxx/index_view.hpp>

//

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/indexes.hh>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/index_model.hpp>
#include <mongocxx/options/index_view.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

std::string keys_to_index_string(bsoncxx::v_noabi::document::view keys) {
    struct deleter_type {
        void operator()(char* ptr) const noexcept {
            bson_free(ptr);
        }
    };

    using ptr_type = std::unique_ptr<char, deleter_type>;

    return ptr_type{libmongoc::collection_keys_to_index_string(to_scoped_bson_view(keys))}.get();
}

void append_to(index_model const& index, scoped_bson& doc) {
    auto const& model_opts = index.options();
    auto const& keys = index.keys();

    if (!model_opts["name"]) {
        doc += scoped_bson{BCON_NEW("name", BCON_UTF8(keys_to_index_string(keys).c_str()))};
    }

    doc += scoped_bson{BCON_NEW("key", BCON_DOCUMENT(to_scoped_bson_view(keys).bson()))};
    doc += to_scoped_bson_view(model_opts);
}

template <typename CreateIndexOptions>
void append_create_to(CreateIndexOptions const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }
}

template <typename DropIndexOptions>
void append_drop_to(DropIndexOptions const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }
}

bsoncxx::v1::document::value create_impl(
    mongoc_collection_t* coll,
    bson_t const* indexes,
    bson_t const* opts,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> commit_quorum) {
    scoped_bson command{
        BCON_NEW("createIndexes", BCON_UTF8(libmongoc::collection_get_name(coll)), "indexes", BCON_ARRAY(indexes))};

    if (commit_quorum) {
        command += to_scoped_bson_view(*commit_quorum);
    }

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command.bson(), opts, reply.out_ptr(), &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
    }

    return std::move(reply).value();
}

bsoncxx::v1::stdx::optional<std::string> create_one_impl(
    mongoc_collection_t* coll,
    v_noabi::index_model const& index,
    bson_t const* opts,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> commit_quorum) {
    auto const reply_owner = [&] {
        scoped_bson doc;
        append_to(index, doc);
        return create_impl(coll, scoped_bson{BCON_NEW("0", BCON_DOCUMENT(doc.bson()))}.bson(), opts, commit_quorum);
    }();

    auto const reply = [&] {
        // SERVER-78611: sharded clusters may place fields in a raw response document instead of in
        // the top-level document.
        if (auto const raw = reply_owner.view()["raw"]) {
            auto const shard_response = raw.get_document().view();

            // There should only be a single field in the raw response with the shard connection
            // string as the key. e.g.:
            //   {
            //     'raw': {
            //       'shard01/localhost:27018,27019,27020': {
            //         ... # Raw response fields.
            //       }
            //     }
            //   }
            // Using a for loop for convenience.
            for (auto const e : shard_response) {
                return e.get_document().view();
            }
        }

        return reply_owner.view();
    }();

    if (auto const note = reply["note"]) {
        if (note.get_string().value == "all indexes already exist") {
            return {};
        }
    }

    if (auto const name = index.options()["name"]) {
        return std::string{name.get_string().value};
    }

    return keys_to_index_string(index.keys());
}

bsoncxx::v_noabi::document::value create_many_impl(
    mongoc_collection_t* coll,
    std::vector<index_model> const& indexes,
    bson_t const* opts,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> commit_quorum) {
    struct builder_deleter_type {
        void operator()(bson_array_builder_t* ptr) const noexcept {
            bson_array_builder_destroy(ptr);
        }
    };

    using builder_ptr_type = std::unique_ptr<bson_array_builder_t, builder_deleter_type>;

    auto const builder_owner = builder_ptr_type{bson_array_builder_new(), builder_deleter_type{}};
    auto const builder = builder_owner.get();

    for (auto const& model : indexes) {
        scoped_bson doc;

        append_to(model, doc);

        bson_array_builder_append_document(builder, doc.bson());
    }

    scoped_bson arr;
    if (!bson_array_builder_build(builder, arr.out_ptr())) {
        throw std::logic_error{"mongocxx::v_noabi::create_many_impl: bson_array_builder_build failed"};
    }
    return bsoncxx::v_noabi::from_v1(create_impl(coll, arr.bson(), opts, commit_quorum));
}

} // namespace

v_noabi::cursor index_view::list() {
    return _indexes.list();
}

v_noabi::cursor index_view::list(v_noabi ::client_session const& session) {
    return _indexes.list(v_noabi::client_session::internal::as_v1(session));
}

bsoncxx::v_noabi::stdx::optional<std::string> index_view::create_one(
    v_noabi::index_model const& model,
    v_noabi::options::index_view const& options) {
    scoped_bson doc;

    append_create_to(options, doc);

    return create_one_impl(v1::indexes::internal::get_collection(_indexes), model, doc.bson(), options.commit_quorum());
}

bsoncxx::v_noabi::stdx::optional<std::string> index_view::create_one(
    v_noabi::client_session const& session,
    v_noabi::index_model const& model,
    v_noabi::options::index_view const& options) {
    scoped_bson doc;

    append_create_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return create_one_impl(v1::indexes::internal::get_collection(_indexes), model, doc.bson(), options.commit_quorum());
}

bsoncxx::v_noabi::document::value index_view::create_many(
    std::vector<index_model> const& indexes,
    v_noabi::options::index_view const& options) {
    scoped_bson doc;

    append_create_to(options, doc);

    return create_many_impl(
        v1::indexes::internal::get_collection(_indexes), indexes, doc.bson(), options.commit_quorum());
}

bsoncxx::v_noabi::document::value index_view::create_many(
    v_noabi::client_session const& session,
    std::vector<index_model> const& indexes,
    v_noabi::options::index_view const& options) {
    scoped_bson doc;

    append_create_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return create_many_impl(
        v1::indexes::internal::get_collection(_indexes), indexes, doc.bson(), options.commit_quorum());
}

void index_view::drop_one(bsoncxx::v_noabi::stdx::string_view name, v_noabi::options::index_view const& options) try {
    scoped_bson doc;

    append_drop_to(options, doc);

    v1::indexes::internal::drop_one_impl(v1::indexes::internal::get_collection(_indexes), name, doc.bson());
} catch (v1::exception const& ex) {
    if (ex.code() == v1::indexes::errc::invalid_name) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

void index_view::drop_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::string_view name,
    v_noabi::options::index_view const& options) try {
    scoped_bson doc;

    append_drop_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    v1::indexes::internal::drop_one_impl(v1::indexes::internal::get_collection(_indexes), name, doc.bson());
} catch (v1::exception const& ex) {
    if (ex.code() == v1::indexes::errc::invalid_name) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

void index_view::drop_one(index_model const& model, v_noabi::options::index_view const& options) {
    auto const name = model.options()["name"];

    if (name && name.type() == bsoncxx::v_noabi::type::k_string) {
        this->drop_one(name.get_string().value, options);
    } else {
        this->drop_one(keys_to_index_string(model.keys()), options);
    }
}

void index_view::drop_one(
    v_noabi::client_session const& session,
    v_noabi::index_model const& model,
    v_noabi::options::index_view const& options) {
    auto const name = model.options()["name"];

    if (name && name.type() == bsoncxx::v_noabi::type::k_string) {
        this->drop_one(session, name.get_string().value, options);
    } else {
        this->drop_one(session, keys_to_index_string(model.keys()), options);
    }
}

void index_view::drop_all(v_noabi::options::index_view const& options) try {
    scoped_bson doc;

    append_drop_to(options, doc);

    v1::indexes::internal::drop_all_impl(v1::indexes::internal::get_collection(_indexes), doc.bson());
} catch (v1::exception const& ex) {
    if (ex.code() == v1::indexes::errc::invalid_name) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

void index_view::drop_all(v_noabi::client_session const& session, v_noabi::options::index_view const& options) try {
    scoped_bson doc;

    append_drop_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    v1::indexes::internal::drop_all_impl(v1::indexes::internal::get_collection(_indexes), doc.bson());
} catch (v1::exception const& ex) {
    if (ex.code() == v1::indexes::errc::invalid_name) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
    } else {
        v_noabi::throw_exception<v_noabi::operation_exception>(ex);
    }
}

} // namespace v_noabi
} // namespace mongocxx
