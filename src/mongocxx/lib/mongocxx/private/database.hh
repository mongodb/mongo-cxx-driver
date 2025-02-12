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

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>

#include <mongocxx/private/client.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/write_concern.hh>

namespace mongocxx {
namespace v_noabi {

class database::impl {
   public:
    impl(mongoc_database_t* db, mongocxx::v_noabi::client::impl const* client, std::string name)
        : database_t(db), client_impl(client), name(std::move(name)) {}

    impl(impl const& i)
        : database_t{libmongoc::database_copy(i.database_t)}, client_impl{i.client_impl}, name{i.name} {}

    impl& operator=(impl const& i) {
        if (this != &i) {
            libmongoc::database_destroy(database_t);
            database_t = libmongoc::database_copy(i.database_t);
            client_impl = i.client_impl;
            name = i.name;
        }

        return *this;
    }

    ~impl() {
        libmongoc::database_destroy(database_t);
    }

    mongoc_database_t* database_t;
    mongocxx::v_noabi::client::impl const* client_impl;
    std::string name;
};

} // namespace v_noabi
} // namespace mongocxx
