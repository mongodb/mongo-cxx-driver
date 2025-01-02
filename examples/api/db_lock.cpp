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

} // namespace

db_lock::~db_lock() {
    this->get().drop(wc_majority());
}

db_lock::db_lock(mongocxx::client& client, std::string name) : _client_ptr(&client), _name(name) {
    // https://www.mongodb.com/docs/manual/reference/limits/#mongodb-limit-Length-of-Database-Names
    static constexpr std::size_t db_name_size_max = 63u;

    if (_name.size() > db_name_size_max) {
        // Strip prefix, which is more likely to be common across components.
        // e.g. `api_mongocxx_examples_very_long_component_name` -> `g_component_name` (length: 16).
        _name = std::move(_name).substr(_name.size() - db_name_size_max, db_name_size_max);
    }

    ((void)std::lock_guard<std::mutex>{db_locks_mut}, _lock = std::unique_lock<std::mutex>(db_locks[name]));

    this->get().drop(wc_majority());
}

mongocxx::database db_lock::get() const& {
    EXPECT(_client_ptr);

    return _client_ptr->database(_name);
}
