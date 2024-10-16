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

#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>

#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>

#include <examples/api/concern.hh>
#include <examples/api/db_lock.hh>
#include <examples/macros.hh>

namespace {

std::unordered_map<std::string, std::mutex> db_locks;
std::mutex db_locks_mut;

}  // namespace

db_lock::~db_lock() {
    this->get().drop(wc_majority());
}

db_lock::db_lock(mongocxx::client& client, std::string name) : _client_ptr(&client), _name(name) {
    ((void)std::lock_guard<std::mutex>{db_locks_mut},
     _lock = std::unique_lock<std::mutex>(db_locks[name]));

    this->get().drop(wc_majority());
}

mongocxx::database db_lock::get() const& {
    EXPECT(_client_ptr);

    return _client_ptr->database(_name);
}
