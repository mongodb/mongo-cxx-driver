/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>

#include "mongoc.h"

#include "bson/document.hpp"

#include "driver/base/cursor.hpp"
#include "driver/result/distinct.hpp"
#include "driver/result/explain.hpp"
#include "driver/result/write.hpp"

namespace mongo {
namespace driver {

class client;
class database;
class write_concern;
class read_preference;

namespace model {
class aggregate;
class find;
class find_one_and_replace;
class find_one_and_remove;
class find_one_and_update;
class remove;
class insert;
class replace;
class distinct;
class count;
class explain;

namespace details {
class xupdate;
}

using update = details::xupdate;
}  // namespace model

namespace result {
class write;
class explain;
class distinct;
}  // namespace result

namespace fluent {
class findable;
class aggregatable;
}  // namespace fluent

class collection {

    friend class database;

   public:
    collection(collection&& client);
    ~collection();

    collection& operator=(collection&& client);

    cursor find(const model::find& model) const;
    cursor aggregate(const model::aggregate& model) const;

    result::write insertOne(const model::insert& model);
    result::write insertMany(const model::insert& model);
    result::write replaceOne(const model::replace& model);
    result::write updateOne(const model::update& model);
    result::write updateMany(const model::update& model);
    result::write removeOne(const model::remove& model);
    result::write removeMany(const model::remove& model);

    bson::document::value find_one_and_replace(const model::find_one_and_replace& model);
    bson::document::value find_one_and_update(const model::find_one_and_update& model);
    bson::document::value find_one_and_remove(const model::find_one_and_remove& model);

    bson::document::value explain(const model::explain& model) const;
    result::distinct distinct(const model::distinct& model) const;

    int64_t count(const model::count& model) const;

    void drop();

   private:
    collection(client* client, database* database, std::string name);

    collection(const collection& client) = delete;
    collection& operator=(const collection& client) = delete;

    client* _client;
    database* _database;

    std::string _name;
    mongoc_collection_t* _collection;
};

}  // namespace driver
}  // namespace mongo
