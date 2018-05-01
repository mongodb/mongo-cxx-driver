// Copyright 2017 MongoDB Inc.
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

#pragma once

#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

class index_view::impl {
   public:
    impl(mongoc_collection_t* collection) : _coll{collection} {}

    impl(const impl& i) = default;

    impl(impl&& i) = default;

    ~impl() = default;

    impl& operator=(const impl& i) = default;

    std::string get_index_name_from_keys(bsoncxx::document::view_or_value keys) {
        libbson::scoped_bson_t keys_bson{keys};

        auto name_from_keys = libmongoc::collection_keys_to_index_string(keys_bson.bson());
        std::string result{name_from_keys};
        bson_free(name_from_keys);

        return result;
    }

    cursor list(const client_session* session) {
        if (session) {
            bsoncxx::builder::basic::document options_builder;
            options_builder.append(
                bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
            libbson::scoped_bson_t bson_options(options_builder.extract());
            return libmongoc::collection_find_indexes_with_opts(_coll, bson_options.bson());
        }

        return libmongoc::collection_find_indexes_with_opts(_coll, nullptr);
    }

    bsoncxx::stdx::optional<std::string> create_one(const client_session* session,
                                                    const index_model& model,
                                                    const options::index_view& options) {
        bsoncxx::document::value result =
            create_many(session, std::vector<index_model>{model}, options);
        bsoncxx::document::view result_view = result.view();

        if (result_view["note"] &&
            bsoncxx::string::to_string(result_view["note"].get_utf8().value) ==
                "all indexes already exist") {
            return bsoncxx::stdx::nullopt;
        }

        if (auto name = model.options()["name"]) {
            return bsoncxx::stdx::make_optional(
                bsoncxx::string::to_string(name.get_value().get_utf8().value));
        }

        return bsoncxx::stdx::make_optional(get_index_name_from_keys(model.keys()));
    }

    bsoncxx::document::value create_many(const client_session* session,
                                         const std::vector<index_model>& indexes,
                                         const options::index_view& options) {
        using namespace bsoncxx;
        using builder::basic::concatenate;

        builder::basic::array index_arr;

        for (auto&& model : indexes) {
            builder::basic::document index_doc;
            const bsoncxx::document::view& opts_view = model.options();
            const bsoncxx::document::view& keys = model.keys();

            if (!opts_view["name"]) {
                index_doc.append(kvp("name", get_index_name_from_keys(keys)));
            }

            index_doc.append(kvp("key", keys), concatenate(opts_view));
            index_arr.append(index_doc.view());
        }

        document::view_or_value command =
            make_document(kvp("createIndexes", libmongoc::collection_get_name(_coll)),
                          kvp("indexes", index_arr.view()));

        libbson::scoped_bson_t reply;
        bson_error_t error;

        builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        libbson::scoped_bson_t command_bson{command};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};

        auto result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(error);
        }

        return reply.steal();
    }

    void drop_one(const client_session* session,
                  bsoncxx::stdx::string_view name,
                  const options::index_view& options) {
        if (name == bsoncxx::stdx::string_view{"*"}) {
            throw logic_error(error_code::k_invalid_parameter);
        }

        bsoncxx::builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        bsoncxx::document::value command = make_document(
            kvp("dropIndexes", libmongoc::collection_get_name(_coll)), kvp("index", name));

        libbson::scoped_bson_t reply;
        libbson::scoped_bson_t command_bson{command.view()};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};
        bson_error_t error;

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(error);
        }
    }

    MONGOCXX_INLINE void drop_all(const client_session* session,
                                  const options::index_view& options) {
        bsoncxx::document::value command = make_document(
            kvp("dropIndexes", libmongoc::collection_get_name(_coll)), kvp("index", "*"));

        libbson::scoped_bson_t reply;
        bson_error_t error;

        libbson::scoped_bson_t command_bson{command.view()};

        bsoncxx::builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        libbson::scoped_bson_t opts_bson{opts_doc.view()};

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(error);
        }
    }

    mongoc_collection_t* _coll;
};
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
