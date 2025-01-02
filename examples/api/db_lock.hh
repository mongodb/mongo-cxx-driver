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

#include <mutex>
#include <string>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/database-fwd.hpp>

// Ensure exclusive access to the associated database.
// For convenience, drops the database on both lock and unlock.
class db_lock {
   private:
    mongocxx::client* _client_ptr;
    std::string _name;
    std::unique_lock<std::mutex> _lock;

   public:
    ~db_lock();

    db_lock(db_lock&&) = delete;
    db_lock& operator=(db_lock&&) = delete;
    db_lock(db_lock const&) = delete;
    db_lock& operator=(db_lock const&) = delete;

    db_lock(mongocxx::client& client, std::string name);

    mongocxx::database get() const&;
};
