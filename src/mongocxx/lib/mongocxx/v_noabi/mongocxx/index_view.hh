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

#pragma once

#include <mongocxx/v1/cursor.hh>

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
#include <mongocxx/index_view.hpp> // IWYU pragma: export
#include <mongocxx/options/index_view.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/scoped_bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

using bsoncxx::v_noabi::builder::basic::kvp;
using bsoncxx::v_noabi::builder::basic::make_document;

class index_view::impl {
   public:
    impl(mongoc_collection_t* collection, mongoc_client_t* client) : _coll{collection}, _client{client} {}

    std::string get_index_name_from_keys(bsoncxx::v_noabi::document::view_or_value keys) {
        auto name_from_keys = libmongoc::collection_keys_to_index_string(to_scoped_bson_view(keys));
        std::string result{name_from_keys};
        bson_free(name_from_keys);

        return result;
    }

    cursor list(client_session const* session) {
        if (session) {
            bsoncxx::v_noabi::builder::basic::document options_builder;
            options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
            return v1::cursor::internal::make(
                libmongoc::collection_find_indexes_with_opts(_coll, to_scoped_bson_view(options_builder)));
        }

        return v1::cursor::internal::make(libmongoc::collection_find_indexes_with_opts(_coll, nullptr));
    }

    bsoncxx::v_noabi::stdx::optional<std::string>
    create_one(client_session const* session, index_model const& model, options::index_view const& options) {
        auto const result = create_many(session, std::vector<index_model>{model}, options);
        auto result_view = result.view();

        // SERVER-78611: sharded clusters may place fields in a raw response document instead of in
        // the top-level document.
        if (auto const raw = result_view["raw"]) {
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
            for (auto const& shard_response : raw.get_document().view()) {
                result_view = shard_response.get_document().view();
            }
        }

        auto const note = result_view["note"];

        if (note && bsoncxx::v_noabi::string::to_string(note.get_string().value) == "all indexes already exist") {
            return bsoncxx::v_noabi::stdx::nullopt;
        }

        if (auto name = model.options()["name"]) {
            return bsoncxx::v_noabi::stdx::make_optional(
                bsoncxx::v_noabi::string::to_string(name.get_value().get_string().value));
        }

        return bsoncxx::v_noabi::stdx::make_optional(get_index_name_from_keys(model.keys()));
    }

    bsoncxx::v_noabi::document::value create_many(
        client_session const* session,
        std::vector<index_model> const& indexes,
        options::index_view const& options) {
        using namespace bsoncxx;
        using builder::basic::concatenate;

        builder::basic::array index_arr;

        for (auto&& model : indexes) {
            builder::basic::document index_doc;
            bsoncxx::v_noabi::document::view const& opts_view = model.options();
            bsoncxx::v_noabi::document::view const& keys = model.keys();

            if (!opts_view["name"]) {
                index_doc.append(kvp("name", get_index_name_from_keys(keys)));
            }

            index_doc.append(kvp("key", keys), concatenate(opts_view));
            index_arr.append(index_doc.view());
        }

        document::view_or_value command = make_document(
            kvp("createIndexes", libmongoc::collection_get_name(_coll)), kvp("indexes", index_arr.view()));

        bson_error_t error;

        builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        if (options.commit_quorum()) {
            auto server_description = scoped_server_description(
                libmongoc::client_select_server(_client, true /* for_writes */, nullptr /* read_prefs */, &error));
            if (!server_description.sd)
                throw_exception<write_exception>(error);

            bson_t const* const hello = libmongoc::server_description_hello_response(server_description.sd);

            bson_iter_t iter;
            if (!bson_iter_init_find(&iter, hello, "maxWireVersion") || bson_iter_int32(&iter) < 9) {
                throw write_exception{
                    error_code::k_invalid_parameter,
                    "option 'commitQuorum' not available on the current server version"};
            }

            command = make_document(concatenate(command), concatenate(options.commit_quorum()->view()));
        }

        scoped_bson reply;
        auto result = libmongoc::collection_write_command_with_opts(
            _coll, to_scoped_bson_view(command), to_scoped_bson_view(opts_doc), reply.out_ptr(), &error);

        if (!result) {
            throw_exception<operation_exception>(from_v1(std::move(reply)), error);
        }

        return from_v1(std::move(reply));
    }

    void drop_one(
        client_session const* session,
        bsoncxx::v_noabi::stdx::string_view name,
        options::index_view const& options) {
        if (name == bsoncxx::v_noabi::stdx::string_view{"*"}) {
            throw logic_error(error_code::k_invalid_parameter);
        }

        bsoncxx::v_noabi::builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        bsoncxx::v_noabi::document::value command =
            make_document(kvp("dropIndexes", libmongoc::collection_get_name(_coll)), kvp("index", name));

        bson_error_t error;

        scoped_bson reply;
        bool result = libmongoc::collection_write_command_with_opts(
            _coll, to_scoped_bson_view(command), to_scoped_bson_view(opts_doc), reply.out_ptr(), &error);

        if (!result) {
            throw_exception<operation_exception>(from_v1(std::move(reply)), error);
        }
    }

    inline void drop_all(client_session const* session, options::index_view const& options) {
        bsoncxx::v_noabi::document::value command =
            make_document(kvp("dropIndexes", libmongoc::collection_get_name(_coll)), kvp("index", "*"));

        bson_error_t error;

        bsoncxx::v_noabi::builder::basic::document opts_doc;

        if (options.max_time()) {
            opts_doc.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{options.max_time()->count()}));
        }

        if (options.write_concern()) {
            opts_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
        }

        if (session) {
            opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        scoped_bson reply;
        bool result = libmongoc::collection_write_command_with_opts(
            _coll, to_scoped_bson_view(command), to_scoped_bson_view(opts_doc), reply.out_ptr(), &error);

        if (!result) {
            throw_exception<operation_exception>(from_v1(std::move(reply)), error);
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

        scoped_server_description(scoped_server_description&& other) : sd(other.sd) {
            other.sd = nullptr;
        }

        scoped_server_description& operator=(scoped_server_description&& other) {
            sd = other.sd;
            other.sd = nullptr;
            return *this;
        }

        scoped_server_description(scoped_server_description const&) = delete;
        scoped_server_description& operator=(scoped_server_description const&) = delete;

        mongoc_server_description_t* sd;
    };
};

} // namespace v_noabi
} // namespace mongocxx
