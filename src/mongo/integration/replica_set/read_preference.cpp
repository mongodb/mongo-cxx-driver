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

#include <cstdlib>

#include <memory>

#include "mongo/client/dbclient.h"

namespace {

    using namespace std;
    using namespace mongo;
    using namespace mongo::integration;

    const string TEST_NS = "test-read_pref.read_pref";
    const string TEST_DB = "test-read_pref";
    const string TEST_COLL = "read_pref";

    class ReadPreferenceTest : public ReplicaSetTest {
    public:
        static void SetUpTestCase() {
            try {
                ReplicaSetTest::SetUpTestCase();
                std::string errmsg;

                ConnectionString cs = ConnectionString::parse(rs().mongodbUri(), errmsg);
                replset_conn.reset(static_cast<DBClientReplicaSet*>(cs.connect(errmsg)));
                replset_conn->dropCollection(TEST_NS);

                primary_conn.reset(new DBClientConnection());
                primary_conn->connect(rs().primary().uri());

                secondary_conn.reset(new DBClientConnection());
                secondary_conn->connect(rs().secondaries().front().uri());
                return;

            } catch (const std::exception& ex) {
                std::cout << "Got fatal error during test setup: " << ex.what() << std::endl;
            } catch (...) {
                std::cout << "Got unknown error during test setup" << std::endl;
            }
            std::abort();
        }

        static auto_ptr<DBClientReplicaSet> replset_conn;
        static auto_ptr<DBClientConnection> primary_conn;
        static auto_ptr<DBClientConnection> secondary_conn;
    };

    auto_ptr<DBClientReplicaSet> ReadPreferenceTest::replset_conn;
    auto_ptr<DBClientConnection> ReadPreferenceTest::primary_conn;
    auto_ptr<DBClientConnection> ReadPreferenceTest::secondary_conn;

    int op_count(const auto_ptr<DBClientConnection>& connection, const std::string& op_type) {
        BSONObj cmd = BSON("serverStatus" << 1);
        BSONObj info;
        connection->runCommand("admin", cmd, info);
        return info["opcounters"][op_type].Int();
    }

    void assert_route(
        const auto_ptr<DBClientReplicaSet>& test_conn,
        const auto_ptr<DBClientConnection>& expected_target,
        void (*op)(const auto_ptr<DBClientReplicaSet>&, ReadPreference),
        ReadPreference rp,
        const std::string& op_type)
    {
        // Prime the test connection
        test_conn->findOne(TEST_NS, Query().readPref(rp, BSONArray()));

        // Record operations of this type before running operation
        int ops_before = op_count(expected_target, op_type);

        // Run the Operation
        op(test_conn, rp);

        // Record operations of this type after running operation
        int ops_after = op_count(expected_target, op_type);

        // The serverStatus command itself adds a command to the count
        ASSERT_EQUALS(ops_after - ops_before, op_type == "command" ? 2 : 1);
    }

    void query(const auto_ptr<DBClientReplicaSet>& test_conn, ReadPreference rp) {
        Query q = Query().readPref(rp, BSONArray());
        test_conn->findOne(TEST_NS, q);
    }

    void count(const auto_ptr<DBClientReplicaSet>& test_conn, ReadPreference rp) {
        Query q = Query().readPref(rp, BSONArray());
        test_conn->count(TEST_NS, q, QueryOption_SlaveOk);
    }

    void distinct(const auto_ptr<DBClientReplicaSet>& test_conn, ReadPreference rp) {
        Query q = Query().readPref(rp, BSONArray());
        test_conn->distinct(TEST_NS, "a", q);
    }

    void collStats(const auto_ptr<DBClientReplicaSet>& test_conn, ReadPreference rp) {
        BSONObjBuilder cmd;
        cmd.append("query", BSON("collStats" << TEST_COLL));

        switch (rp) {
            case ReadPreference_PrimaryOnly:
                cmd.append("$readPreference", BSON("mode" << "primary"));
                break;
            case ReadPreference_PrimaryPreferred:
                cmd.append("$readPreference", BSON("mode" << "primaryPreferred"));
                break;
            case ReadPreference_SecondaryOnly:
                cmd.append("$readPreference", BSON("mode" << "secondary"));
                break;
            case ReadPreference_SecondaryPreferred:
                cmd.append("$readPreference", BSON("mode" << "secondaryPreferred"));
                break;
            case ReadPreference_Nearest:
                // for completeness
                break;
        }

        BSONObj info;
        test_conn->runCommand(TEST_DB, cmd.obj(), info);
    }

    TEST_F(ReadPreferenceTest, RoutingQuery) {
        assert_route(replset_conn, primary_conn, query, ReadPreference_PrimaryOnly, "query");
        assert_route(replset_conn, primary_conn, query, ReadPreference_PrimaryPreferred, "query");
        assert_route(replset_conn, secondary_conn, query, ReadPreference_SecondaryOnly, "query");
        assert_route(replset_conn, secondary_conn, query, ReadPreference_SecondaryPreferred, "query");
    }

    TEST_F(ReadPreferenceTest, RoutingCount) {
        assert_route(replset_conn, primary_conn, count, ReadPreference_PrimaryOnly, "command");
        assert_route(replset_conn, primary_conn, count, ReadPreference_PrimaryPreferred, "command");
        assert_route(replset_conn, secondary_conn, count, ReadPreference_SecondaryOnly, "command");
        assert_route(replset_conn, secondary_conn, count, ReadPreference_SecondaryPreferred, "command");
    }

    TEST_F(ReadPreferenceTest, RoutingDistinct) {
        assert_route(replset_conn, primary_conn, distinct, ReadPreference_PrimaryOnly, "command");
        assert_route(replset_conn, primary_conn, distinct, ReadPreference_PrimaryPreferred, "command");
        assert_route(replset_conn, secondary_conn, distinct, ReadPreference_SecondaryOnly, "command");
        assert_route(replset_conn, secondary_conn, distinct, ReadPreference_SecondaryPreferred, "command");
    }

    TEST_F(ReadPreferenceTest, RoutingCollStats) {
        assert_route(replset_conn, primary_conn, collStats, ReadPreference_PrimaryOnly, "command");
        assert_route(replset_conn, primary_conn, collStats, ReadPreference_PrimaryPreferred, "command");
        assert_route(replset_conn, secondary_conn, collStats, ReadPreference_SecondaryOnly, "command");
        assert_route(replset_conn, secondary_conn, collStats, ReadPreference_SecondaryPreferred, "command");
    }

    TEST_F(ReadPreferenceTest, RoutingPrimaryDown) {
        mongo::orchestration::Server primary = rs().primary();
        primary.stop();

        while (true) {
            try {
                replset_conn->findOne(TEST_NS, Query().readPref(ReadPreference_SecondaryOnly, BSONArray()));
                break;
            } catch (const DBException& ex) {
                std::cout << ex.what() <<std::endl;
                mongo::sleepsecs(1);
            }
        }

        assert_route(replset_conn, secondary_conn, query, ReadPreference_PrimaryPreferred, "query");
        assert_route(replset_conn, secondary_conn, query, ReadPreference_SecondaryOnly, "query");
        assert_route(replset_conn, secondary_conn, query, ReadPreference_SecondaryPreferred, "query");

        primary.start();

        while (true) {
            try {
                WriteConcern wcAll = WriteConcern().nodes(2);
                replset_conn->insert(TEST_NS, BSON("x" << 2), 0, &wcAll);
                break;
            } catch (const DBException&) {
                mongo::sleepsecs(1);
            }
        }
    }

    TEST_F(ReadPreferenceTest, QueryPrimaryOnly) {
        Query q = Query().readPref(ReadPreference_PrimaryOnly, BSONArray());
        auto_ptr<DBClientCursor> cursor_ptr = replset_conn->query(TEST_NS, q);
        ASSERT_TRUE(cursor_ptr->originalHost() == replset_conn->masterConn().getServerAddress());
    }

    TEST_F(ReadPreferenceTest, QueryPrimaryPreferred) {
        Query q = Query().readPref(ReadPreference_PrimaryPreferred, BSONArray());
        auto_ptr<DBClientCursor> cursor_ptr = replset_conn->query(TEST_NS, q);
        ASSERT_TRUE(cursor_ptr->originalHost() == replset_conn->masterConn().getServerAddress());
    }

    TEST_F(ReadPreferenceTest, QuerySecondaryOnly) {
        Query q = Query().readPref(ReadPreference_SecondaryOnly, BSONArray());
        auto_ptr<DBClientCursor> cursor_ptr = replset_conn->query(TEST_NS, q);
        ASSERT_TRUE(cursor_ptr->originalHost() != replset_conn->masterConn().getServerAddress());
    }

    TEST_F(ReadPreferenceTest, QuerySecondaryPreferred) {
        Query q = Query().readPref(ReadPreference_SecondaryPreferred, BSONArray());
        auto_ptr<DBClientCursor> cursor_ptr = replset_conn->query(TEST_NS, q);
        ASSERT_TRUE(cursor_ptr->originalHost() != replset_conn->masterConn().getServerAddress());
    }

} // namespace
