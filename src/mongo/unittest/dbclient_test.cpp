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

#include <boost/scoped_ptr.hpp>
#include <list>
#include <string>
#include <vector>

#include "mongo/unittest/integration_test.h"
#include "mongo/util/fail_point_service.h"
#include "mongo/util/stringutils.h"

#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"

using std::auto_ptr;
using std::list;
using std::string;
using std::vector;

using boost::function;

using namespace bson;
using namespace mongo::unittest;
using namespace mongo;

namespace {
    const string TEST_NS = "test.dbclient";
    const string TEST_DB = "test";
    const string TEST_COLL = "dbclient";

    class ConnHook : public DBConnectionHook {
    public:
        ConnHook() : _count(0) { };
        void onCreate(DBClientBase*) { _count++; }
        void onDestroy(DBClientBase*) { }
        void onHandedOut(DBClientBase*) { }
        int getCount() { return _count; }
    private:
        int _count;
    };

    class DBClientTest : public ::testing::Test {
    public:
        DBClientTest() {
            c.connect(string("localhost:") + integrationTestParams.port);
            c.dropCollection(TEST_NS);
        }
        DBClientConnection c;
    };

    /* Query Class */
    TEST(QueryTest, Explain) {
        Query q;
        q.explain();
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.isExplain());
    }

    TEST(QueryTest, Snapshot) {
        Query q;
        q.snapshot();
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$snapshot"));
        ASSERT_TRUE(q.obj.getBoolField("$snapshot"));
    }

    TEST(QueryTest, Sort) {
        Query q;
        q.sort(BSON("a" << 1));
        ASSERT_TRUE(q.isComplex());
        BSONObj sort = q.getSort();
        ASSERT_TRUE(sort.hasField("a"));
        ASSERT_EQUALS(sort.getIntField("a"), 1);
    }

    TEST(QueryTest, Hint) {
        Query q;
        q.hint(BSON("a" << 1));
        BSONObj hint = q.getHint();
        ASSERT_TRUE(hint.hasField("a"));
        ASSERT_EQUALS(hint.getIntField("a"), 1);
    }

    TEST(QueryTest, MinKey) {
        Query q;
        BSONObj minobj;
        q.minKey(minobj);
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$min"));
        ASSERT_EQUALS(q.obj["$min"].Obj(), minobj);
    }

    TEST(QueryTest, MaxKey) {
        Query q;
        BSONObj maxobj;
        q.maxKey(maxobj);
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$max"));
        ASSERT_EQUALS(q.obj["$max"].Obj(), maxobj);
    }

    TEST(QueryTest, ReadPreferencePrimary) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "primary");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferencePrimaryPreferred) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "primaryPreferred");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceSecondary) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondary");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceSecondaryPreferred) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondaryPreferred");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceNearest) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_Nearest, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "nearest");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceTagSets) {
        Query q("{}");
        BSONObj tag_set1 = BSON("datacenter" << "nyc");
        BSONObj tag_set2 = BSON("awesome" << "yeah");
        BSONObjBuilder bob;
        BSONArrayBuilder bab;
        bab.append(tag_set1);
        bab.append(tag_set2);
        q.readPref(mongo::ReadPreference_SecondaryOnly, bab.arr());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));

        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondary");
        ASSERT_TRUE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
        vector<BSONElement> tag_sets = read_pref_obj[Query::ReadPrefTagsField.name()].Array();
        ASSERT_EQUALS(tag_sets[0].Obj(), tag_set1);
        ASSERT_EQUALS(tag_sets[1].Obj(), tag_set2);
    }

    /* Connection String */
    TEST(ConnectionString, SameLogicalEndpoint) {
        string err1;
        string err2;
        ConnectionString cs1;
        ConnectionString cs2;

        // INVALID -- default non parsed state
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));
        cs2 = ConnectionString::parse("host1,host2,host3", err1);
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

        // MASTER
        cs1 = ConnectionString::parse("localhost:1234", err1);
        cs2 = ConnectionString::parse("localhost:1234", err2);
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

        // PAIR -- compares the host + port even in swapped order
        cs1 = cs1.parse("localhost:1234,localhost:5678", err1);
        cs2 = cs2.parse("localhost:1234,localhost:5678", err2);
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));
        cs2 = cs2.parse("localhost:5678,localhost:1234", err2);
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

        // SET -- compares the set name only
        cs1 = cs1.parse("testset/localhost:1234,localhost:5678", err1);
        cs2 = cs2.parse("testset/localhost:5678,localhost:1234", err2);
        ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

        // Different types
        cs1 = cs1.parse("testset/localhost:1234,localhost:5678", err1);
        cs2 = cs2.parse("localhost:5678,localhost:1234", err2);
        ASSERT_FALSE(cs1.sameLogicalEndpoint(cs2));
    }

    TEST(ConnectionString, TypeToString) {
        ASSERT_EQUALS(
            ConnectionString::typeToString(ConnectionString::INVALID),
            "invalid"
        );
        ASSERT_EQUALS(
            ConnectionString::typeToString(ConnectionString::MASTER),
            "master"
        );
        ASSERT_EQUALS(
            ConnectionString::typeToString(ConnectionString::PAIR),
            "pair"
        );
        ASSERT_EQUALS(
            ConnectionString::typeToString(ConnectionString::SET),
            "set"
        );
        ASSERT_EQUALS(
            ConnectionString::typeToString(ConnectionString::CUSTOM),
            "custom"
        );
    }

    /* DBClient Tests */
    TEST_F(DBClientTest, ManualGetMore) {
        // Ported from dbtests/querytests.cpp
        for(int i = 0; i < 3; ++i) {
            c.insert(TEST_NS, BSON("num" << i));
        }
        auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, Query("{}"), 0, 0, 0, 0, 2);

        uint64_t cursor_id = cursor->getCursorId();
        cursor->decouple();
        cursor.reset();
        cursor = c.getMore(TEST_NS, cursor_id);
        ASSERT_TRUE(cursor->more());
        ASSERT_EQUALS(cursor->next().getIntField("num"), 2);
    }

    /* DBClient free functions */
    TEST_F(DBClientTest, ServerAlive) {
        ASSERT_TRUE(serverAlive("localhost:" + integrationTestParams.port));
        ASSERT_FALSE(serverAlive("mongo.example:27017"));
    }

    TEST_F(DBClientTest, ErrField) {
        ASSERT_FALSE(hasErrField(BSONObj()));
        ASSERT_TRUE(hasErrField(BSON("$err" << true)));
    }

    /* Connection level functions */
    TEST_F(DBClientTest, InsertVector) {
        vector<BSONObj> v;
        v.push_back(BSON("num" << 1));
        v.push_back(BSON("num" << 2));
        c.insert(TEST_NS, v);
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
    }

    TEST_F(DBClientTest, InsertVectorContinueOnError) {
        vector<BSONObj> v;
        v.push_back(BSON("_id" << 1));
        v.push_back(BSON("_id" << 1));
        v.push_back(BSON("_id" << 2));
        ASSERT_THROWS(
            c.insert(TEST_NS, v, InsertOption_ContinueOnError),
            OperationException
        );
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
    }

    TEST_F(DBClientTest, GetIndexes) {
        auto_ptr<DBClientCursor> cursor = c.getIndexes(TEST_NS);
        ASSERT_FALSE(cursor->more());

        c.insert(TEST_NS, BSON("test" << true));
        cursor = c.getIndexes(TEST_NS);
        ASSERT_EQUALS(cursor->itcount(), 1);

        c.ensureIndex(TEST_NS, BSON("test" << 1));
        cursor = c.getIndexes(TEST_NS);
        vector<BSONObj> v;
        while(cursor->more())
            v.push_back(cursor->next());
        ASSERT_EQUALS(v.size(), 2U);
        ASSERT_EQUALS(v[0]["name"].String(), "_id_");
        ASSERT_EQUALS(v[1]["name"].String(), "test_1");
    }

    TEST_F(DBClientTest, DropIndexes) {
        c.ensureIndex(TEST_NS, BSON("test" << 1));
        unsigned index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 2U);
        c.dropIndexes(TEST_NS);
        index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 1U);
    }

    TEST_F(DBClientTest, DropIndex) {
        c.ensureIndex(TEST_NS, BSON("test" << 1));
        c.ensureIndex(TEST_NS, BSON("test2" << -1));
        unsigned index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 3U);

        // Interface that takes an index key obj
        c.dropIndex(TEST_NS, BSON("test" << 1));
        index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 2U);

        // Interface that takes an index name
        c.dropIndex(TEST_NS, "test2_-1");
        index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 1U);

        // Drop of unknown index should throw an error
        ASSERT_THROWS(c.dropIndex(TEST_NS, "test3_1"), DBException);
    }

    TEST_F(DBClientTest, ReIndex) {
        c.ensureIndex(TEST_NS, BSON("test" << 1));
        c.ensureIndex(TEST_NS, BSON("test2" << -1));
        unsigned index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 3U);
        c.reIndex(TEST_NS);
        index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 3U);
    }

    TEST_F(DBClientTest, Aggregate) {
        BSONObj doc = BSON("hello" << "world");

        BSONObj pipeline = BSON("0" << BSON("$match" << doc));

        c.insert(TEST_NS, doc);
        c.insert(TEST_NS, doc);

        std::auto_ptr<DBClientCursor> cursor = c.aggregate(TEST_NS, pipeline);
        ASSERT_TRUE(cursor.get());

        for (int i = 0; i < 2; i++) {
            ASSERT_TRUE(cursor->more());

            BSONObj result = cursor->next();
            ASSERT_TRUE(result.valid());
            ASSERT_EQUALS(result["hello"].String(), "world");
        }

        ASSERT_FALSE(cursor->more());
    }

    TEST_F(DBClientTest, CreateCollection) {
        ASSERT_FALSE(c.exists(TEST_NS));
        ASSERT_TRUE(c.createCollection(TEST_NS));
        ASSERT_FALSE(c.createCollection(TEST_NS));
        ASSERT_TRUE(c.exists(TEST_NS));
    }

    TEST_F(DBClientTest, CopyDatabase) {
        c.dropDatabase("copy");
        c.insert(TEST_NS, BSON("test" << true));
        ASSERT_TRUE(c.copyDatabase(TEST_DB, "copy"));
        string copy_ns = string("copy.") + TEST_COLL;
        c.exists(copy_ns);
        BSONObj doc = c.findOne(copy_ns, Query("{}"));
        ASSERT_TRUE(doc["test"].boolean());
    }

    TEST_F(DBClientTest, DBProfilingLevel) {
        DBClientWithCommands::ProfilingLevel level;
        ASSERT_TRUE(c.setDbProfilingLevel(TEST_DB, c.ProfileAll));
        ASSERT_TRUE(c.getDbProfilingLevel(TEST_DB, level, 0));
        ASSERT_EQUALS(level, c.ProfileAll);

        ASSERT_TRUE(c.setDbProfilingLevel(TEST_DB, c.ProfileSlow));
        ASSERT_TRUE(c.getDbProfilingLevel(TEST_DB, level, 0));
        ASSERT_EQUALS(level, c.ProfileSlow);

        ASSERT_TRUE(c.setDbProfilingLevel(TEST_DB, c.ProfileOff));
        ASSERT_TRUE(c.getDbProfilingLevel(TEST_DB, level, 0));
        ASSERT_EQUALS(level, c.ProfileOff);
    }

    TEST_F(DBClientTest, QueryJSON) {
        Query q(string("{name: 'Tyler'}"));
        BSONObj filter = q.getFilter();
        ASSERT_TRUE(filter.hasField("name"));
        ASSERT_EQUALS(filter["name"].String(), "Tyler");
    }

    // Used to test exhaust via query below
    void nop(const BSONObj &) { /* nop */ }

    // This also excercises availableOptions (which is protected)
    TEST_F(DBClientTest, Exhaust) {
        for(int i=0; i<1000; ++i)
            c.insert(TEST_NS, BSON("num" << i));

        boost::function<void(const BSONObj &)> f = nop;
        c.query(f, TEST_NS, Query("{}"));
    }

    TEST_F(DBClientTest, GetPrevError) {
        c.insert(TEST_NS, BSON("_id" << 1));
        ASSERT_THROWS(
            c.insert(TEST_NS, BSON("_id" << 1)),
            OperationException
        );
        c.insert(TEST_NS, BSON("_id" << 2));
        ASSERT_TRUE(c.getLastError().empty());
        ASSERT_FALSE(c.getPrevError().isEmpty());
    }

    TEST_F(DBClientTest, MaxScan) {
        for(int i = 0; i < 100; ++i) {
            c.insert(TEST_NS, fromjson("{}"));
        }
        vector<BSONObj> results;
        c.findN(results, TEST_NS, Query("{}"), 100);
        ASSERT_EQUALS(results.size(), 100U);
        results.clear();
        c.findN(results, TEST_NS, Query("{$query: {}, $maxScan: 50}"), 100);
        ASSERT_EQUALS(results.size(), 50U);
    }

    TEST_F(DBClientTest, ReturnKey) {
        c.insert(TEST_NS, BSON("a" << true << "b" << true));

        BSONObj result;

        result = c.findOne(TEST_NS, Query("{$query: {a: true}}"));
        ASSERT_TRUE(result.hasField("a"));
        ASSERT_TRUE(result.hasField("b"));

        result = c.findOne(TEST_NS, Query("{$query: {a: true}, $returnKey: true}"));
        ASSERT_FALSE(result.hasField("a"));
        ASSERT_FALSE(result.hasField("b"));

        c.ensureIndex(TEST_NS, BSON("a" << 1));
        result = c.findOne(TEST_NS, Query("{$query: {a: true}, $returnKey: true}"));
        ASSERT_TRUE(result.hasField("a"));
        ASSERT_FALSE(result.hasField("b"));
    }

    TEST_F(DBClientTest, ShowDiskLoc) {
        c.insert(TEST_NS, BSON("a" << true));

        BSONObj result;

        result = c.findOne(TEST_NS, Query("{$query: {}}"));
        ASSERT_FALSE(result.hasField("$diskLoc"));

        result = c.findOne(TEST_NS, Query("{$query: {}, $showDiskLoc: true}"));
        ASSERT_TRUE(result.hasField("$diskLoc"));
    }

    TEST_F(DBClientTest, MaxTimeMS) {
        // Requires --setParameter=enableTestCommands=1
        c.insert(TEST_NS, BSON("a" << true));
        BSONObj result;

        c.runCommand("admin", BSON("buildinfo" << true), result);

        if (versionCmp(result["version"].toString(), "2.5.3") >= 0) {
            c.runCommand("admin", BSON(
                "configureFailPoint" << "maxTimeAlwaysTimeOut" <<
                "mode" << BSON("times" << 2)
            ), result);

            // First test with a query
            ASSERT_NO_THROW(
                c.findOne(TEST_NS, Query("{$query: {}}"));
            );
            ASSERT_THROWS(
                c.findOne(TEST_NS, Query("{$query: {}, $maxTimeMS: 1}"));
            , DBException);

            // Then test with a command
            ASSERT_TRUE(
                c.runCommand(TEST_DB, BSON("count" << TEST_COLL), result)
            );
            ASSERT_FALSE(
                c.runCommand(TEST_DB,
                    BSON("count" << TEST_COLL << "maxTimeMS" << 1), result)
            );
        } else {
            // we are not connected to MongoDB >= 2.5.3, skip
            SUCCEED();
        }
    }

    TEST_F(DBClientTest, Comment) {
        c.insert(TEST_NS, BSON("a" << true));
        string profile_coll = TEST_DB + ".system.profile";
        c.dropCollection(profile_coll);
        c.setDbProfilingLevel(TEST_DB, c.ProfileAll);
        c.findOne(TEST_NS, Query("{$query: {a: 'z'}, $comment: 'wow'})"));
        c.setDbProfilingLevel(TEST_DB, c.ProfileOff);
        BSONObj result = c.findOne(profile_coll, BSON(
            "ns" << TEST_NS <<
            "op" << "query" <<
            "query.$comment" << "wow"
        ));
        ASSERT_FALSE(result.isEmpty());
    }

    TEST_F(DBClientTest, FlushBadConnections) {
        string host_str = string("localhost:") + integrationTestParams.port;

        pool.removeHost(host_str);

        ConnHook* dh = new ConnHook();
        pool.addHook(dh);

        {
            ScopedDbConnection conn1(host_str);
            ScopedDbConnection conn2(host_str);
            conn1.done();
            conn2.done();
        }

        // Cause one of the connections to fail
        getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
            setMode(FailPoint::nTimes, 1);

        // call isMaster on all the connections, remove the bad ones
        pool.flush();

        {
            ScopedDbConnection conn1(host_str);
            ScopedDbConnection conn2(host_str);
            conn1.done();
            conn2.done();
        }

        ASSERT_EQUALS(dh->getCount(), 3);
    }

    TEST_F(DBClientTest, IsConnectionGood) {
        string host_str = string("localhost:") + integrationTestParams.port;
        ScopedDbConnection conn(host_str);
        ASSERT_TRUE(conn.ok());
        ASSERT_EQUALS(conn.getHost(), host_str);
        DBClientBase* pconn = conn.get();
        ASSERT_TRUE(pool.isConnectionGood(host_str, pconn));
        conn.done();
    }

    TEST_F(DBClientTest, AppendInfo) {
        pool.clear();

        BSONObjBuilder b;
        pool.appendInfo(b);
        BSONObj info = b.done();
        BSONObj hosts = info["hosts"].Obj();
        ASSERT_EQUALS(hosts["localhost:27999::0"]["available"].Int(), 0);

        {
            string host_str = string("localhost:") + integrationTestParams.port;
            ScopedDbConnection conn(host_str);
            conn.done();
        }

        BSONObjBuilder b2;
        pool.appendInfo(b2);
        BSONObj info2 = b2.done();
        hosts = info2["hosts"].Obj();
        ASSERT_EQUALS(hosts["localhost:27999::0"]["available"].Int(), 1);
    }

    TEST_F(DBClientTest, ClearStaleConnections) {
        string host_str = string("localhost:") + integrationTestParams.port;

        ConnHook* dh = new ConnHook();
        pool.clear();
        pool.addHook(dh);

        {
            ScopedDbConnection conn1(host_str);
            ScopedDbConnection conn2(host_str);
            conn1.done();
            conn2.done();
        }

        // Cause a conn to be stale
        getGlobalFailPointRegistry()->getFailPoint("notStillConnected")->
            setMode(FailPoint::nTimes, 1);

        // runs getStaleConnections and deletes them
        pool.taskDoWork();

        ASSERT_EQUALS(pool.taskName(), "DBConnectionPool-cleaner");

        {
            ScopedDbConnection conn1(host_str);
            ScopedDbConnection conn2(host_str);
            conn1.done();
            conn2.done();
        }

        ASSERT_EQUALS(dh->getCount(), 3);
    }

    TEST_F(DBClientTest, CursorAttachNewConnection) {
        c.insert(TEST_NS, BSON("num" << 1));
        c.insert(TEST_NS, BSON("num" << 2));

        DBClientCursor cursor(&c, TEST_NS, Query("{}").obj, 0, 0, 0, 0, 0);
        ASSERT_TRUE(cursor.init());
        ASSERT_TRUE(cursor.more());
        cursor.next();

        string host_str = string("localhost:") + integrationTestParams.port;
        log() << "here";
        cursor.attach(new ScopedDbConnection(host_str));
        ASSERT_TRUE(cursor.more());
        cursor.next();
    }

    TEST_F(DBClientTest, LazyCursor) {
        c.insert(TEST_NS, BSON("test" << true));

        DBClientCursor cursor(&c, TEST_NS, Query("{}").obj, 0, 0, 0, 0, 0);
        bool is_retry = false;
        cursor.initLazy(is_retry);
        ASSERT_TRUE(cursor.initLazyFinish(is_retry));

        vector<BSONObj> docs;
        while(cursor.more())
            docs.push_back(cursor.next());

        ASSERT_EQUALS(docs.size(), 1U);
        ASSERT_TRUE(docs.front()["test"].value());
    }

    void nop_hook(BSONObjBuilder* bob) { (void)bob; }
    void nop_hook_post(BSONObj, string) { }

    TEST_F(DBClientTest, LazyCursorCommand) {
        c.setRunCommandHook(nop_hook);
        c.setPostRunCommandHook(nop_hook_post);
        DBClientCursor cursor(&c, TEST_DB + ".$cmd", Query("{dbStats: 1}").obj, 1, 0, 0, 0, 0);
        bool is_retry = false;
        cursor.initLazy(is_retry);
        ASSERT_TRUE(cursor.initLazyFinish(is_retry));

        ASSERT_TRUE(cursor.more());
        ASSERT_TRUE(cursor.next().hasField("db"));
        ASSERT_FALSE(cursor.more());
    }

    TEST_F(DBClientTest, InitCommand) {
        DBClientCursor cursor(&c, TEST_DB + ".$cmd", Query("{dbStats: 1}").obj, 1, 0, 0, 0, 0);
        ASSERT_TRUE(cursor.initCommand());

        ASSERT_TRUE(cursor.more());
        ASSERT_TRUE(cursor.next().hasField("db"));
        ASSERT_FALSE(cursor.more());
    }

    TEST_F(DBClientTest, GetMoreLimit) {
        c.insert(TEST_NS, BSON("num" << 1));
        c.insert(TEST_NS, BSON("num" << 2));
        c.insert(TEST_NS, BSON("num" << 3));
        c.insert(TEST_NS, BSON("num" << 4));

        // set nToReturn to 3 but batch size to 1
        // This verifies:
        //   * we can manage with multiple batches
        //   * we can correctly upgrade batchSize 1 to 2 to avoid automatic
        //     cursor closing when nReturn = 1 (wire protocol edge case)
        auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, Query("{}"), 3, 0, 0, 0, 1);
        vector<BSONObj> docs;
        while(cursor->more())
            docs.push_back(cursor->next());

        ASSERT_EQUALS(docs.size(), 3U);
    }

    TEST_F(DBClientTest, NoGetMoreLimit) {
        c.insert(TEST_NS, BSON("num" << 1));
        c.insert(TEST_NS, BSON("num" << 2));
        c.insert(TEST_NS, BSON("num" << 3));
        c.insert(TEST_NS, BSON("num" << 4));

        // set nToReturn to 2 but batch size to 4
        // check to see if a limit of 2 takes despite the larger batch
        auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, Query("{}"), 2, 0, 0, 0, 4);
        vector<BSONObj> docs;
        while(cursor->more())
            docs.push_back(cursor->next());

        ASSERT_EQUALS(docs.size(), 2U);
    }

    TEST_F(DBClientTest, PeekError) {
        BSONObj result;
        c.runCommand("admin", BSON("buildinfo" << true), result);

        // TODO: figure out if we can come up with query that produces $err on 2.4.x
        if (versionCmp(result["version"].toString(), "2.5.3") >= 0) {
            auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, Query("{'$fake': true}"));
            ASSERT_TRUE(cursor->peekError());
        } else {
            SUCCEED();
        }
    }

    TEST_F(DBClientTest, DefaultWriteConcernInsert) {
        c.insert(TEST_NS, BSON("_id" << 1));
        ASSERT_THROWS(
            c.insert(TEST_NS, BSON("_id" << 1)),
            OperationException
        );
        ASSERT_EQUALS(c.count(TEST_NS, BSON("_id" << 1)), 1U);
    }

    TEST_F(DBClientTest, DefaultWriteConcernUpdate) {
        c.insert(TEST_NS, BSON("a" << true));
        ASSERT_THROWS(
            c.update(TEST_NS, BSON("a" << true), BSON("$badOp" << "blah")),
            OperationException
        );
        ASSERT_EQUALS(c.count(TEST_NS, BSON("a" << true)), 1U);
    }

    TEST_F(DBClientTest, DefaultWriteConcernRemove) {
        ASSERT_THROWS(
            c.remove("BAD.$NS", BSON("a" << true)),
            OperationException
        );
    }

    TEST_F(DBClientTest, UnacknowledgedInsert) {
        c.insert(TEST_NS, BSON("_id" << 1));
        ASSERT_NO_THROW(
            c.insert(
                TEST_NS,
                BSON("_id" << 1),
                0,
                &WriteConcern::unacknowledged
            )
        );
        ASSERT_EQUALS(c.count(TEST_NS, BSON("_id" << 1)), 1U);
    }

    TEST_F(DBClientTest, UnacknowledgedUpdate) {
        c.insert(TEST_NS, BSON("a" << true));
        ASSERT_NO_THROW(
            c.update(
                TEST_NS,
                BSON("a" << true),
                BSON("$badOp" << "blah"),
                false,
                false,
                &WriteConcern::unacknowledged
            )
        );
        ASSERT_EQUALS(c.count(TEST_NS, BSON("a" << true)), 1U);
    }

    TEST_F(DBClientTest, UnacknowledgedRemove) {
        ASSERT_NO_THROW(
            c.remove(
                "BAD.$NS",
                BSON("a" << true),
                false,
                &WriteConcern::unacknowledged
            )
        );
    }

    TEST_F(DBClientTest, AcknowledgeMultipleNodesNonReplicated) {
        WriteConcern wc = WriteConcern().nodes(2).timeout(3000);
        ASSERT_THROWS(
            c.insert(TEST_NS, BSON("_id" << 1), 0, &wc),
            OperationException
        );
    }

} // namespace
