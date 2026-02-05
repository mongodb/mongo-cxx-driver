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

#include <mongocxx/search_index_view.hpp>

//

#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/search_indexes.hh>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/search_index_model.hpp>
#include <mongocxx/search_index_view.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/aggregate.hh>
#include <mongocxx/read_preference.hh>
#include <mongocxx/scoped_bson.hh>
#include <mongocxx/search_index_model.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

template <typename Indexes>
Indexes& check_moved_from(Indexes& indexes) {
    if (v1::search_indexes::internal::is_moved_from(indexes)) {
        throw mongocxx::v_noabi::logic_error{error_code::k_invalid_search_index_view};
    }
    return indexes;
}

mongoc_read_prefs_t const* get_read_prefs(v_noabi::options::aggregate const& opts) {
    auto const& opt = opts.read_preference();
    return opt ? v_noabi::read_preference::internal::as_mongoc(*opt) : nullptr;
}

v1::cursor
list_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts, mongoc_read_prefs_t const* read_prefs) {
    using bsoncxx::v_noabi::from_v1;

    v1::pipeline pipeline;

    if (name) {
        pipeline.append_stage(
            scoped_bson{BCON_NEW("$listSearchIndexes", "{", "name", BCON_UTF8(std::string{name}.c_str()), "}")}.view());
    } else {
        pipeline.append_stage(scoped_bson{BCON_NEW("$listSearchIndexes", "{", "}")}.view());
    }

    return v1::cursor::internal::make(
        libmongoc::collection_aggregate(
            coll, MONGOC_QUERY_NONE, scoped_bson_view{pipeline.view_array()}.bson(), opts, read_prefs));
}

bsoncxx::v_noabi::document::value create_impl(mongoc_collection_t* coll, bson_t const* command, bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command, opts, reply.out_ptr(), &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
    }

    return bsoncxx::v_noabi::from_v1(std::move(reply).value());
}

void append_to(bsoncxx::v_noabi::document::view definition, char const* name, char const* type, scoped_bson& doc) {
    if (name) {
        doc += scoped_bson{BCON_NEW("name", BCON_UTF8(name))};
    }

    doc += scoped_bson{BCON_NEW("definition", BCON_DOCUMENT(to_scoped_bson_view(definition).bson()))};

    if (type) {
        doc += scoped_bson{BCON_NEW("type", BCON_UTF8(type))};
    }
}

void append_to(v_noabi::search_index_model const& index, scoped_bson& doc) {
    auto const& name_opt = v_noabi::search_index_model::internal::name(index);
    auto const& type_opt = v_noabi::search_index_model::internal::type(index);

    return append_to(
        index.definition(),
        name_opt ? name_opt->terminated().data() : nullptr,
        type_opt ? type_opt->terminated().data() : nullptr,
        doc);
}

std::string create_one_impl(mongoc_collection_t* coll, bson_t const* index, bson_t const* opts) {
    auto const reply = create_impl(
        coll,
        scoped_bson{BCON_NEW(
                        "createSearchIndexes",
                        BCON_UTF8(libmongoc::collection_get_name(coll)),
                        "indexes",
                        "[",
                        BCON_DOCUMENT(index),
                        "]")}
            .bson(),
        opts);

    return std::string{reply["indexesCreated"].get_array().value[0].get_document().value["name"].get_string().value};
}

std::vector<std::string> create_many_impl(
    mongoc_collection_t* coll,
    std::vector<v_noabi::search_index_model> const& indexes,
    bson_t const* opts) {
    struct deleter_type {
        void operator()(bson_array_builder_t* ptr) const noexcept {
            bson_array_builder_destroy(ptr);
        }
    };

    using builder_ptr_type = std::unique_ptr<bson_array_builder_t, deleter_type>;

    auto const builder_owner = builder_ptr_type{bson_array_builder_new(), deleter_type{}};
    auto const builder = builder_owner.get();

    for (auto const& index : indexes) {
        scoped_bson doc;
        append_to(index, doc);
        if (!bson_array_builder_append_document(builder, doc.bson())) {
            throw std::logic_error{"mongocxx::v_noabi::create_many_impl: bson_array_builder_append_document failed"};
        }
    }

    scoped_bson arr;
    if (!bson_array_builder_build(builder, arr.out_ptr())) {
        throw std::logic_error{"mongocxx::v_noabi::create_many_impl: bson_array_builder_build failed"};
    }

    auto const reply = create_impl(
        coll,
        scoped_bson{BCON_NEW(
                        "createSearchIndexes",
                        BCON_UTF8(libmongoc::collection_get_name(coll)),
                        "indexes",
                        BCON_ARRAY(arr.bson()))}
            .bson(),
        opts);

    std::vector<std::string> ret;

    // {"indexesCreated": [{ "name": <name> }, ...]} -> [<name>, ...]
    for (auto const& index : reply["indexesCreated"].get_array().value) {
        ret.push_back(std::string{index.get_document().value["name"].get_string().value});
    }

    return ret;
}

void drop_one_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts) {
    scoped_bson const command{
        BCON_NEW("dropSearchIndex", BCON_UTF8(libmongoc::collection_get_name(coll)), "name", BCON_UTF8(name))};

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command, opts, reply.out_ptr(), &error)) {
        // https://www.mongodb.com/docs/manual/reference/error-codes/
        static constexpr std::uint32_t serverErrorNamespaceNotFound = 26;

        // Ignore NamespaceNotFound.
        if (error.domain != MONGOC_ERROR_QUERY || error.code != serverErrorNamespaceNotFound) {
            v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
        }
    }
}

void update_one_impl(
    mongoc_collection_t* coll,
    char const* name,
    bsoncxx::v_noabi::document::view definition,
    bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(
            coll,
            scoped_bson{BCON_NEW(
                            "updateSearchIndex",
                            BCON_UTF8(libmongoc::collection_get_name(coll)),
                            "name",
                            BCON_UTF8(name),
                            "definition",
                            BCON_DOCUMENT(to_scoped_bson_view(definition).bson()))}
                .bson(),
            opts,
            reply.out_ptr(),
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
    }
}

} // namespace

search_index_view::search_index_view(search_index_view const& other) : _indexes{check_moved_from(other._indexes)} {}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by v1::search_indexes.
search_index_view& search_index_view::operator=(search_index_view const& other) {
    _indexes = check_moved_from(other._indexes);
    return *this;
}

v_noabi::cursor search_index_view::list(v_noabi::options::aggregate const& options) {
    scoped_bson opts_doc;

    v_noabi::options::aggregate::internal::append_to(options, opts_doc);

    return list_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        nullptr,
        opts_doc.bson(),
        get_read_prefs(options));
}

v_noabi::cursor search_index_view::list(
    v_noabi::client_session const& session,
    v_noabi::options::aggregate const& options) {
    scoped_bson opts_doc;

    v_noabi::options::aggregate::internal::append_to(options, opts_doc);
    v_noabi::client_session::internal::append_to(session, opts_doc);

    return list_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        nullptr,
        opts_doc.bson(),
        get_read_prefs(options));
}

v_noabi::cursor search_index_view::list(
    bsoncxx::v_noabi::string::view_or_value name,
    v_noabi::options::aggregate const& options) {
    scoped_bson opts_doc;

    v_noabi::options::aggregate::internal::append_to(options, opts_doc);

    return list_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        name.terminated().data(),
        opts_doc.bson(),
        get_read_prefs(options));
}

v_noabi::cursor search_index_view::list(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    v_noabi::options::aggregate const& options) {
    scoped_bson doc;

    v_noabi::options::aggregate::internal::append_to(options, doc);
    v_noabi::client_session::internal::append_to(session, doc);

    return list_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        name.terminated().data(),
        doc.bson(),
        get_read_prefs(options));
}

std::string search_index_view::create_one(bsoncxx::v_noabi::document::view_or_value definition) {
    scoped_bson doc;

    append_to(definition.view(), nullptr, nullptr, doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), doc.bson(), nullptr);
}

std::string search_index_view::create_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value definition) {
    scoped_bson doc;
    append_to(definition.view(), nullptr, nullptr, doc);

    scoped_bson opt_doc;

    v_noabi::client_session::internal::append_to(session, doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), doc.bson(), opt_doc.bson());
}

std::string search_index_view::create_one(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    scoped_bson doc;

    append_to(definition.view(), name.terminated().data(), nullptr, doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), doc.bson(), nullptr);
}

std::string search_index_view::create_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    scoped_bson doc;

    append_to(definition.view(), name.terminated().data(), nullptr, doc);

    scoped_bson opts_doc;

    v_noabi::client_session::internal::append_to(session, opts_doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), doc.bson(), opts_doc.bson());
}

std::string search_index_view::create_one(v_noabi::search_index_model const& model) {
    scoped_bson doc;

    append_to(model, doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), doc.bson(), nullptr);
}

std::string search_index_view::create_one(
    v_noabi::client_session const& session,
    v_noabi::search_index_model const& model) {
    scoped_bson opts_doc;

    append_to(model, opts_doc);

    v_noabi::client_session::internal::append_to(session, opts_doc);

    return create_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), opts_doc.bson(), nullptr);
}

std::vector<std::string> search_index_view::create_many(std::vector<v_noabi::search_index_model> const& models) {
    return create_many_impl(v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), models, nullptr);
}

std::vector<std::string> search_index_view::create_many(
    v_noabi::client_session const& session,
    std::vector<v_noabi::search_index_model> const& models) {
    scoped_bson opts_doc;

    v_noabi::client_session::internal::append_to(session, opts_doc);

    return create_many_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), models, opts_doc.bson());
}

void search_index_view::drop_one(bsoncxx::v_noabi::string::view_or_value name) {
    return drop_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)), name.terminated().data(), nullptr);
}

void search_index_view::drop_one(v_noabi::client_session const& session, bsoncxx::v_noabi::string::view_or_value name) {
    scoped_bson opts_doc;

    v_noabi::client_session::internal::append_to(session, opts_doc);

    return drop_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        name.terminated().data(),
        opts_doc.bson());
}

void search_index_view::update_one(
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    return update_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        name.terminated().data(),
        definition.view(),
        nullptr);
}

void search_index_view::update_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::string::view_or_value name,
    bsoncxx::v_noabi::document::view_or_value definition) {
    scoped_bson opts_doc;

    v_noabi::client_session::internal::append_to(session, opts_doc);

    return update_one_impl(
        v1::search_indexes::internal::get_collection(check_moved_from(_indexes)),
        name.terminated().data(),
        definition.view(),
        opts_doc.bson());
}

} // namespace v_noabi
} // namespace mongocxx
