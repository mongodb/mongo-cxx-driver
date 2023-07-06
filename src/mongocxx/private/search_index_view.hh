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
#include <bsoncxx/types/bson_value/view.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/options/search_index_view.hpp>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class search_index_view::impl {
   public:
    impl(mongoc_collection_t* collection, mongoc_client_t* client)
        : _coll{collection}, _client{client} {}

    impl(const impl& i) = default;

    impl(impl&& i) = default;

    ~impl() = default;

    impl& operator=(const impl& i) = default;

    // std::string get_index_name_from_keys(bsoncxx::document::view_or_value keys) {
    //     libbson::scoped_bson_t keys_bson{keys};

    //     auto name_from_keys = libmongoc::collection_keys_to_index_string(keys_bson.bson());
    //     std::string result{name_from_keys};
    //     bson_free(name_from_keys);

    //     return result;
    // }

    // cursor list(const client_session* session) {
    //     if (session) {
    //         bsoncxx::builder::basic::document options_builder;
    //         options_builder.append(
    //             bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    //         libbson::scoped_bson_t bson_options(options_builder.extract());
    //         return libmongoc::collection_find_indexes_with_opts(_coll, bson_options.bson());
    //     }

    //     return libmongoc::collection_find_indexes_with_opts(_coll, nullptr);
    // }

    bsoncxx::stdx::optional<std::string> create_one(const search_index_model& model,
                                                    const options::search_index_view& options) {
        bsoncxx::document::value result =
            create_many(std::vector<search_index_model>{model}, options);
        bsoncxx::document::view result_view = result.view();

        return bsoncxx::stdx::make_optional(result_view["indexesCreated"]
                                                .get_array()
                                                .value.begin()
                                                ->get_document()
                                                .value["name"]
                                                .get_string()
                                                .value.to_string());
    }

    bsoncxx::document::value create_many(const std::vector<search_index_model>& search_indexes,
                                         const options::search_index_view& options) {
        using namespace bsoncxx;

        builder::basic::array search_index_arr;

        for (auto&& model : search_indexes) {
            builder::basic::document search_index_doc;
            const stdx::optional<std::string> name = model.get_name();
            const bsoncxx::document::view& definition = model.get_definition();

            if (name) {
                search_index_doc.append(kvp("name", name.value()));
            }
            search_index_doc.append(kvp("definition", definition));
            search_index_arr.append(search_index_doc.view());
        }

        document::view_or_value command =
            make_document(kvp("createSearchIndexes", libmongoc::collection_get_name(_coll)),
                          kvp("indexes", search_index_arr.view()));

        libbson::scoped_bson_t reply;
        bson_error_t error;

        builder::basic::document opts_doc;

        libbson::scoped_bson_t command_bson{command};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};

        auto result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }

        return reply.steal();
    }

    void drop_one(const std::string name, const options::search_index_view& options) {
        bsoncxx::builder::basic::document opts_doc;

        bsoncxx::document::value command = make_document(
            kvp("dropSearchIndex", libmongoc::collection_get_name(_coll)), kvp("name", name));

        libbson::scoped_bson_t reply;
        libbson::scoped_bson_t command_bson{command.view()};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};
        bson_error_t error;

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    void update_one(const std::string name,
                    const bsoncxx::document::view_or_value& definition,
                    const options::search_index_view& options) {
        bsoncxx::builder::basic::document opts_doc;
        bsoncxx::document::value command =
            make_document(kvp("updateSearchIndex", libmongoc::collection_get_name(_coll)),
                          kvp("name", name),
                          kvp("definition", definition.view()));

        libbson::scoped_bson_t reply;
        libbson::scoped_bson_t command_bson{command.view()};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};
        bson_error_t error;

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    mongoc_collection_t* _coll;
    mongoc_client_t* _client;

    class scoped_server_description {
       public:
        explicit scoped_server_description(mongoc_server_description_t* sd) : sd(sd) {}
        ~scoped_server_description() {
            mongoc_server_description_destroy(sd);
        }
        mongoc_server_description_t* sd;
    };
};
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
