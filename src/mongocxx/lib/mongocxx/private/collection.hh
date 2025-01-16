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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>

#include <bsoncxx/private/helpers.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/database.hh>
#include <mongocxx/private/read_preference.hh>
#include <mongocxx/private/write_concern.hh>

namespace mongocxx {
namespace v_noabi {

class collection::impl {
   public:
    impl(
        mongoc_collection_t* collection,
        bsoncxx::v_noabi::stdx::string_view database_name,
        mongocxx::v_noabi::client::impl const* client)
        : collection_t(collection), database_name(std::move(database_name)), client_impl(client) {}

    impl(impl const& i)
        : collection_t{libmongoc::collection_copy(i.collection_t)},
          database_name{i.database_name},
          client_impl{i.client_impl} {}

    impl& operator=(impl const& i) {
        if (this != &i) {
            libmongoc::collection_destroy(collection_t);
            collection_t = libmongoc::collection_copy(i.collection_t);

            database_name = i.database_name;
            client_impl = i.client_impl;
        }

        return *this;
    }

    ~impl() {
        libmongoc::collection_destroy(collection_t);
    }

    mongoc_collection_t* collection_t;
    std::string database_name;
    mongocxx::v_noabi::client::impl const* client_impl;
};

} // namespace v_noabi
} // namespace mongocxx
