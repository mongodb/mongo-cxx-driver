/*    Copyright 2014 MongoDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "mongo/platform/basic.h"

#include "mongo/integration/integration_test.h"

#include <memory>

#include "mongo/client/dbclient.h"

namespace {

using namespace std;
using namespace mongo;
using namespace mongo::integration;

const string TEST_NS = "test-basic.basic";
const string TEST_DB = "test-basic";
const string TEST_COLL = "basic";

class RSBasicTest : public ReplicaSetTest {
public:
    RSBasicTest() {
        std::string errmsg;
        ConnectionString cs = ConnectionString::parse(rs().mongodbUri(), errmsg);
        conn.reset(static_cast<DBClientReplicaSet*>(cs.connect(errmsg)));
        conn->dropCollection(TEST_NS);
    }

    std::auto_ptr<DBClientReplicaSet> conn;
};

bool serverGTE(DBClientBase* c, int major, int minor) {
    BSONObj result;
    c->runCommand("admin", BSON("buildinfo" << true), result);

    std::vector<BSONElement> version = result.getField("versionArray").Array();
    int serverMajor = version[0].Int();
    int serverMinor = version[1].Int();

    // std::pair uses lexicographic ordering
    return std::make_pair(serverMajor, serverMinor) >= std::make_pair(major, minor);
}

TEST_F(RSBasicTest, InsertRecoversFromPrimaryFailure) {
    WriteConcern wcAll = WriteConcern().nodes(2).timeout(60000);
    conn->insert(TEST_NS, BSON("x" << 1), 0, &wcAll);

    orchestration::Server original_primary = rs().primary();
    original_primary.stop();

    while (true) {
        try {
            conn->insert(TEST_NS, BSON("x" << 2), 0, &WriteConcern::acknowledged);
            break;
        } catch (const DBException&) {
            mongo::sleepsecs(1);
        }
    }

    ASSERT_EQUALS(conn->count(TEST_NS, Query("{x: 1}")), 1U);
    ASSERT_EQUALS(conn->count(TEST_NS, Query("{x: 2}")), 1U);

    original_primary.start();

    while (true) {
        try {
            conn->insert(TEST_NS, BSON("x" << 2), 0, &wcAll);
            break;

        } catch (const DBException&) {
            // try again
            mongo::sleepsecs(1);
        }
    }
}

TEST_F(RSBasicTest, SecondaryQueryIsNotInteruptedByPrimaryFailure) {
    WriteConcern wcAll = WriteConcern().nodes(2);
    conn->insert(TEST_NS, BSON("x" << 1), 0, &wcAll);

    orchestration::Server primary = rs().primary();
    primary.stop();
    conn->findOne(TEST_NS, Query().readPref(ReadPreference_SecondaryOnly, BSONArray()));
    primary.start();
}

TEST_F(RSBasicTest, FindAndModifyFailure) {
    if (serverGTE(conn.get(), 3, 1)) {
        WriteConcern wcAll = WriteConcern().nodes(5).journal(true);
        conn->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        // findAndModify is expected to throw since we want 5 nodes, and the replica set set only
        // has 3 nodes
        ASSERT_THROWS(conn->findAndModify(TEST_NS,
                                          BSON("i" << 1),
                                          BSON("$inc" << BSON("i" << 1)),
                                          false,
                                          false,
                                          BSONObj(),
                                          BSONObj(),
                                          &wcAll),
                      OperationException);
    }
}

}  // namespace
