// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/config/prelude.hpp>

#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/document/value.hpp>

#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/private/database.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/read_preference.hpp>
#include <mongocxx/private/write_concern.hpp>

#include <mongoc.h>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection::impl {

   public:
    impl(mongoc_collection_t* collection,
         bsoncxx::stdx::string_view database_name,
         const class client::impl* client,
         bsoncxx::stdx::string_view name) :
        collection_t(collection),
        database_name(std::move(database_name)),
        client_impl(client),
        name(name)
    {}

    ~impl() { libmongoc::collection_destroy(collection_t); }

    bsoncxx::document::value gle() {
        const bson_t* gle = mongoc_collection_get_last_error(collection_t);
        libbson::scoped_bson_t error_copy;
        bson_copy_to(gle, error_copy.bson());
        return std::move(error_copy.steal());
    }

    mongoc_collection_t* collection_t;
    std::string database_name;
    const class client::impl* client_impl;
    std::string name;

}; // class impl

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
