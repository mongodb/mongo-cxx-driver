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

#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/write_concern.hpp>

#include <examples/api/concern.hh>

// Preferred default for most operations.
mongocxx::collection set_rw_concern_majority(mongocxx::collection coll) {
    coll.read_concern(rc_majority());
    coll.write_concern(wc_majority());
    return coll;
}

// Preferred default for most operations.
mongocxx::database set_rw_concern_majority(mongocxx::database db) {
    db.read_concern(rc_majority());
    db.write_concern(wc_majority());
    return db;
}

mongocxx::read_concern rc_majority() {
    mongocxx::read_concern rc;
    rc.acknowledge_level(mongocxx::read_concern::level::k_majority);
    return rc;
}

mongocxx::write_concern wc_majority() {
    mongocxx::write_concern wc;
    wc.acknowledge_level(mongocxx::write_concern::level::k_majority);
    return wc;
}
