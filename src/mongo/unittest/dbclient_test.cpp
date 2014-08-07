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

#include <boost/scoped_ptr.hpp>
#include <algorithm>
#include <functional>
#include <list>
#include <string>
#include <vector>

#include "mongo/stdx/functional.h"
#include "mongo/unittest/integration_test.h"
#include "mongo/util/fail_point_service.h"
#include "mongo/util/stringutils.h"

#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"

#include "boost/thread.hpp"

using std::auto_ptr;
using std::list;
using std::string;
using std::vector;

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

    bool serverGTE(DBClientBase* c, int major, int minor) {
        BSONObj result;
        c->runCommand("admin", BSON("buildinfo" << true), result);

        std::vector<BSONElement> version = result.getField("versionArray").Array();
        int serverMajor = version[0].Int();
        int serverMinor = version[1].Int();

        return (serverMajor >= major && serverMinor >= minor);
    }

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
    TEST_F(DBClientTest, Save) {
        // Save a doc with autogenerated id
        c.save(TEST_NS, BSON("hello" << "world"));
        BSONObj result = c.findOne(TEST_NS, Query());
        BSONElement id = result.getField("_id");
        ASSERT_EQUALS(id.type(), jstOID);

        // Save a doc with explicit id
        c.save(TEST_NS, BSON("_id" << "explicit_id" << "hello" << "bar"));
        BSONObj doc = c.findOne(TEST_NS, QUERY("_id" << "explicit_id"));
        ASSERT_EQUALS(doc.getStringField("_id"), std::string("explicit_id"));
        ASSERT_EQUALS(doc.getStringField("hello"), std::string("bar"));

        // Save docs with _id field already present (shouldn't create new docs)
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
        c.save(TEST_NS, BSON("_id" << id.OID() << "hello" << "world"));
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
        c.save(TEST_NS, BSON("_id" << "explicit_id" << "hello" << "baz"));
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
        ASSERT_EQUALS(
            c.findOne(TEST_NS, QUERY("_id" << "explicit_id")).getStringField("hello"),
            std::string("baz")
        );
    }

    TEST_F(DBClientTest, FindAndModify) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 1),
            BSON("$inc" << BSON("i" << 1)),
            false
        );

        ASSERT_EQUALS(result.getIntField("_id"), 1);
        ASSERT_EQUALS(result.getIntField("i"), 1);
        ASSERT_EQUALS(c.count(TEST_NS), 1U);
    }

    TEST_F(DBClientTest, FindAndModifyNoMatch) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 2),
            BSON("$inc" << BSON("i" << 1)),
            false
        );

        ASSERT_TRUE(result.isEmpty());
        ASSERT_EQUALS(c.count(TEST_NS), 1U);
    }

    TEST_F(DBClientTest, FindAndModifyNoMatchUpsert) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 2),
            BSON("$inc" << BSON("i" << 1)),
            true
        );

        ASSERT_TRUE(result.isEmpty());
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
    }

    TEST_F(DBClientTest, FindAndModifyReturnNew) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 1),
            BSON("$inc" << BSON("i" << 1)),
            false,
            true
        );

        ASSERT_EQUALS(result.getIntField("_id"), 1);
        ASSERT_EQUALS(result.getIntField("i"), 2);
        ASSERT_EQUALS(c.count(TEST_NS), 1U);
    }

    TEST_F(DBClientTest, FindAndModifyNoMatchUpsertReturnNew) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 2),
            BSON("$inc" << BSON("i" << 1)),
            true,
            true
        );

        ASSERT_TRUE(result.hasField("_id"));
        ASSERT_EQUALS(result.getIntField("i"), 3);
        ASSERT_EQUALS(c.count(TEST_NS), 2U);
    }

    TEST_F(DBClientTest, FindAndModifySort) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
        c.insert(TEST_NS, BSON("_id" << 2 << "i" << 2));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSONObj(),
            BSON("$inc" << BSON("i" << 1)),
            false,
            false,
            BSON("i" << -1)
        );

        ASSERT_EQUALS(result.getIntField("_id"), 2);
        ASSERT_EQUALS(result.getIntField("i"), 2);
        ASSERT_EQUALS(c.count(TEST_NS, BSON("_id" << 2 << "i" << 3)), 1U);
    }

    TEST_F(DBClientTest, FindAndModifyProjection) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndModify(
            TEST_NS,
            BSON("i" << 1),
            BSON("$inc" << BSON("i" << 1)),
            false,
            false,
            BSONObj(),
            BSON("_id" << 0)
        );

        ASSERT_FALSE(result.hasField("_id"));
        ASSERT_TRUE(result.hasField("i"));
    }

    TEST_F(DBClientTest, FindAndModifyDuplicateKeyError) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
        c.createIndex(TEST_NS, IndexSpec().addKey("i").unique());
        ASSERT_THROWS(
            c.findAndModify(
                TEST_NS,
                BSON("i" << 1 << "j" << 1),
                BSON("$set" << BSON("k" << 1)),
                true
            ),
            OperationException
        );
    }

    TEST_F(DBClientTest, FindAndRemove) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndRemove(
            TEST_NS,
            BSON("i" << 1)
        );

        ASSERT_EQUALS(result.getIntField("_id"), 1);
        ASSERT_EQUALS(result.getIntField("i"), 1);
        ASSERT_EQUALS(c.count(TEST_NS), 0U);
    }

    TEST_F(DBClientTest, FindAndRemoveNoMatch) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndRemove(
            TEST_NS,
            BSON("i" << 2)
        );

        ASSERT_TRUE(result.isEmpty());
        ASSERT_EQUALS(c.count(TEST_NS), 1U);
    }

    TEST_F(DBClientTest, FindAndRemoveSort) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
        c.insert(TEST_NS, BSON("_id" << 2 << "i" << 2));

        BSONObj result = c.findAndRemove(
            TEST_NS,
            BSONObj(),
            BSON("i" << -1)
        );

        ASSERT_EQUALS(result.getIntField("_id"), 2);
        ASSERT_EQUALS(result.getIntField("i"), 2);
        ASSERT_EQUALS(c.count(TEST_NS), 1U);
        ASSERT_EQUALS(c.count(TEST_NS, BSON("_id" << 1)), 1U);
    }

    TEST_F(DBClientTest, FindAndRemoveProjection) {
        c.insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c.findAndRemove(
            TEST_NS,
            BSON("i" << 1),
            BSONObj(),
            BSON("_id" << 0)
        );

        ASSERT_FALSE(result.hasField("_id"));
        ASSERT_TRUE(result.hasField("i"));
        ASSERT_EQUALS(c.count(TEST_NS), 0U);
    }

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

    TEST_F(DBClientTest, Distinct) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 3));

        BSONObj result = c.distinct(TEST_NS, "a");

        std::vector<BSONElement> results;
        BSONObjIterator iter(result);
        while (iter.more())
            results.push_back(iter.next());

        std::sort(results.begin(), results.end());

        ASSERT_EQUALS(results[0].Int(), 1);
        ASSERT_EQUALS(results[1].Int(), 2);
        ASSERT_EQUALS(results[2].Int(), 3);
    }

    TEST_F(DBClientTest, DistinctWithQuery) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 2));
        c.insert(TEST_NS, BSON("a" << 3));

        BSONObj result = c.distinct(TEST_NS, "a", BSON("a" << GT << 1));

        std::vector<BSONElement> results;
        BSONObjIterator iter(result);
        while (iter.more())
            results.push_back(iter.next());

        std::sort(results.begin(), results.end());

        ASSERT_EQUALS(results[0].Int(), 2);
        ASSERT_EQUALS(results[1].Int(), 3);
    }

    TEST_F(DBClientTest, DistinctDotted) {
        c.insert(TEST_NS, BSON("a" << BSON("b" << "a") << "c" << 12));
        c.insert(TEST_NS, BSON("a" << BSON("b" << "b") << "c" << 12));
        c.insert(TEST_NS, BSON("a" << BSON("b" << "c") << "c" << 12));
        c.insert(TEST_NS, BSON("a" << BSON("b" << "c") << "c" << 12));

        BSONObj result = c.distinct(TEST_NS, "a.b");

        std::vector<BSONElement> results;
        BSONObjIterator iter(result);
        while (iter.more())
            results.push_back(iter.next());

        std::sort(results.begin(), results.end());

        ASSERT_EQUALS(results[0].String(), std::string("a"));
        ASSERT_EQUALS(results[1].String(), std::string("b"));
        ASSERT_EQUALS(results[2].String(), std::string("c"));
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

    TEST_F(DBClientTest, SimpleGroup) {
        c.insert(TEST_NS, BSON("a" << 1 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 2 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 3 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 1 << "color" << "blue"));

        std::string reduce = "function(current, aggregate) {"
            "if (current.color === 'green') { aggregate.green++; }"
            "if (current.a) { aggregate.a += current.a; }"
        "}";

        BSONObj initial = BSON("a" << 0 << "green" << 0);
        BSONObj cond = BSON("a" << LT << 3);
        BSONObj key = BSON("color" << 1);

        std::string finalize = "function(result) {"
            "result.combined = result.green + result.a;"
        "}";

        vector<BSONObj> results;
        c.group(TEST_NS, reduce, &results, initial, cond, key, finalize);

        vector<BSONObj>::const_iterator it = results.begin();
        while (it != results.end()) {
            BSONObj current = *it;
            if (current.getStringField("color") == std::string("green")) {
                ASSERT_EQUALS(current.getField("a").Double(), 3.0);
                ASSERT_EQUALS(current.getField("green").Double(), 2.0);
                ASSERT_EQUALS(current.getField("combined").Double(), 5.0);
            } else {
                ASSERT_EQUALS(current.getField("a").Double(), 1.0);
                ASSERT_EQUALS(current.getField("green").Double(), 0.0);
                ASSERT_EQUALS(current.getField("combined").Double(), 1.0);
            }
            ++it;
        }
    }

    TEST_F(DBClientTest, GroupWithKeyFunction) {
        c.insert(TEST_NS, BSON("a" << 1 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 2 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 3 << "color" << "green"));
        c.insert(TEST_NS, BSON("a" << 1 << "color" << "blue"));

        std::string reduce = "function(current, aggregate) {"
            "if (current.color === 'green') { aggregate.green++; }"
            "if (current.a) { aggregate.a += current.a; }"
        "}";

        BSONObj initial = BSON("a" << 0 << "green" << 0);
        BSONObj cond = BSON("a" << LT << 3);

        std::string key = "function(doc) {"
            "return { 'color': doc.color }"
        "}";

        std::string finalize = "function(result) {"
            "result.combined = result.green + result.a;"
        "}";

        vector<BSONObj> results;
        c.groupWithKeyFunction(TEST_NS, reduce, &results, initial, cond, key, finalize);

        vector<BSONObj>::const_iterator it = results.begin();
        while (it != results.end()) {
            BSONObj current = *it;
            if (current.getStringField("color") == std::string("green")) {
                ASSERT_EQUALS(current.getField("a").Double(), 3.0);
                ASSERT_EQUALS(current.getField("green").Double(), 2.0);
                ASSERT_EQUALS(current.getField("combined").Double(), 5.0);
            } else {
                ASSERT_EQUALS(current.getField("a").Double(), 1.0);
                ASSERT_EQUALS(current.getField("green").Double(), 0.0);
                ASSERT_EQUALS(current.getField("combined").Double(), 1.0);
            }
            ++it;
        }
    }

    void getResults(DBClientCursor* cursor, std::vector<int>* results, boost::mutex* mut) {
        while (cursor->more()) {
            boost::lock_guard<boost::mutex> lock(*mut);
            results->push_back(cursor->next().getIntField("_id"));
        }
    }

    DBClientBase* cloneFromPool(DBClientBase* originalConnection,
                                std::vector<DBClientBase*>* connectionAccumulator) {
        DBClientBase* conn = pool.get(originalConnection->getServerAddress(),
            originalConnection->getSoTimeout());
        connectionAccumulator->push_back(conn);
        return conn;
    }

    TEST_F(DBClientTest, ParallelCollectionScanUsingConnectionPool) {
        bool supported = serverGTE(&c, 2, 6);

        if (supported) {
            const size_t numItems = 8000;
            const size_t seriesSum = (numItems * (numItems - 1)) / 2;

            for (size_t i = 0; i < numItems; ++i)
                c.insert(TEST_NS, BSON("_id" << static_cast<int>(i)));

            std::vector<DBClientBase*> connections;
            std::vector<DBClientCursor*> cursors;

            stdx::function<DBClientBase* ()> factory = stdx::bind(&cloneFromPool, &c, &connections);
            c.parallelScan(TEST_NS, 3, &cursors, factory);

            std::vector<int> results;
            boost::mutex resultsMutex;
            std::vector<boost::thread*> threads;

            // We can get up to 3 cursors back here but the server might give us back less
            for (size_t i = 0; i < cursors.size(); ++i)
                threads.push_back(new boost::thread(getResults, cursors[i], &results, &resultsMutex));

            // Ensure all the threads have completed their scans
            for (size_t i = 0; i < threads.size(); ++i) {
                threads[i]->join();
                delete threads[i];
            }

            // Cleanup the cursors that parallel scan created
            for (size_t i = 0; i < cursors.size(); ++i)
                delete cursors[i];

            size_t sum = 0;
            for (size_t i = 0; i < results.size(); ++i)
                sum += results[i];

            ASSERT_EQUALS(sum, seriesSum);
        }
    }

    DBClientBase* makeNewConnection(DBClientBase* originalConnection,
                                    std::vector<DBClientBase*>* connectionAccumulator) {
        DBClientConnection* newConn = new DBClientConnection();
        newConn->connect(originalConnection->getServerAddress());
        connectionAccumulator->push_back(newConn);
        return newConn;
    }

    TEST_F(DBClientTest, ParallelCollectionScanUsingNewConnections) {
        bool supported = serverGTE(&c, 2, 6);

        if (supported) {
            const size_t numItems = 8000;
            const size_t seriesSum = (numItems * (numItems - 1)) / 2;

            for (size_t i = 0; i < numItems; ++i)
                c.insert(TEST_NS, BSON("_id" << static_cast<int>(i)));

            std::vector<DBClientBase*> connections;
            std::vector<DBClientCursor*> cursors;

            stdx::function<DBClientBase* ()> factory = stdx::bind(&makeNewConnection, &c, &connections);
            c.parallelScan(TEST_NS, 3, &cursors, factory);

            std::vector<int> results;
            boost::mutex resultsMutex;
            std::vector<boost::thread*> threads;

            // We can get up to 3 cursors back here but the server might give us back less
            for (size_t i = 0; i < cursors.size(); ++i)
                threads.push_back(new boost::thread(getResults, cursors[i], &results, &resultsMutex));

            // Ensure all the threads have completed their scans
            for (size_t i = 0; i < threads.size(); ++i) {
                threads[i]->join();
                delete threads[i];
            }

            // Cleanup the cursors that parallel scan created
            for (size_t i = 0; i < cursors.size(); ++i)
                delete cursors[i];

            // Cleanup the connections our connection factory created
            for (size_t i = 0; i < connections.size(); ++i)
                delete connections[i];

            size_t sum = 0;
            for (size_t i = 0; i < results.size(); ++i)
                sum += results[i];

            ASSERT_EQUALS(sum, seriesSum);
        }
    }

    DBClientBase* makeSketchyConnection(DBClientBase* originalConnection,
                                        std::vector<DBClientBase*>* connectionAccumulator) {
        static int connectionCount = 0;

        // Fail creating the second connection
        DBClientConnection* newConn;
        if (connectionCount != 1)
            newConn = new DBClientConnection();
        else
            throw OperationException(BSONObj());

        newConn->connect(originalConnection->getServerAddress());
        connectionAccumulator->push_back(newConn);

        connectionCount++;

        return newConn;
    }

    TEST_F(DBClientTest, ParallelCollectionScanBadConnections) {
        bool supported = serverGTE(&c, 2, 6);

        if (supported) {
            const size_t numItems = 8000;

            for (size_t i = 0; i < numItems; ++i)
                c.insert(TEST_NS, BSON("_id" << static_cast<int>(i)));

            std::vector<DBClientBase*> connections;
            std::vector<DBClientCursor*> cursors;

            stdx::function<DBClientBase* ()> factory = stdx::bind(&makeSketchyConnection, &c, &connections);
            ASSERT_THROWS(
                c.parallelScan(TEST_NS, 3, &cursors, factory),
                OperationException
            );

            ASSERT_EQUALS(cursors.size(), 1U);
            ASSERT_EQUALS(connections.size(), 1U);

            // Cleanup the cursors that parallel scan created
            for (size_t i = 0; i < cursors.size(); ++i)
                delete cursors[i];

            // Cleanup the connections our connection factory created
            for (size_t i = 0; i < connections.size(); ++i)
                delete connections[i];
        }
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

        c.createIndex(TEST_NS, BSON("test" << 1));
        cursor = c.getIndexes(TEST_NS);
        vector<BSONObj> v;
        while(cursor->more())
            v.push_back(cursor->next());
        ASSERT_EQUALS(v.size(), 2U);
        ASSERT_EQUALS(v[0]["name"].String(), "_id_");
        ASSERT_EQUALS(v[1]["name"].String(), "test_1");
    }

    TEST_F(DBClientTest, DropIndexes) {
        c.createIndex(TEST_NS, BSON("test" << 1));
        unsigned index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 2U);
        c.dropIndexes(TEST_NS);
        index_count = c.getIndexes(TEST_NS)->itcount();
        ASSERT_EQUALS(index_count, 1U);
    }

    TEST_F(DBClientTest, DropIndex) {
        c.createIndex(TEST_NS, BSON("test" << 1));
        c.createIndex(TEST_NS, BSON("test2" << -1));
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
        c.createIndex(TEST_NS, BSON("test" << 1));
        c.createIndex(TEST_NS, BSON("test2" << -1));
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

        BSONObj info;
        ASSERT_TRUE(c.runCommand(TEST_DB, BSON("collstats" << TEST_COLL), info));

        bool server26plus = serverGTE(&c, 2, 6);

        ASSERT_EQUALS(info.getIntField("userFlags"), server26plus ? 1 : 0);
        ASSERT_EQUALS(info.getIntField("nindexes"), 1);
    }

    TEST_F(DBClientTest, CreateCollectionAdvanced) {
        BSONObjBuilder opts;
        opts.append("usePowerOf2Sizes", 0);
        opts.append("autoIndexId", false);
        c.createCollectionWithOptions(TEST_NS, 1, true, 1, opts.obj());

        BSONObj info;
        ASSERT_TRUE(c.runCommand(TEST_DB, BSON("collstats" << TEST_COLL), info));

        ASSERT_EQUALS(info.getIntField("userFlags"), 0);
        ASSERT_EQUALS(info.getIntField("nindexes"), 0);
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

        stdx::function<void(const BSONObj &)> f = nop;
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

        c.createIndex(TEST_NS, BSON("a" << 1));
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

        if (serverGTE(&c, 2, 6)) {
            c.runCommand("admin", BSON(
                "configureFailPoint" << "maxTimeAlwaysTimeOut" <<
                "mode" << BSON("times" << 2)
            ), result);

            // First test with a query
            ASSERT_NO_THROW(c.findOne(TEST_NS, Query("{}")););
            ASSERT_THROWS(c.findOne(TEST_NS, Query("{}").maxTimeMs(1)), DBException);

            // Then test with a command
            ASSERT_NO_THROW(c.count(TEST_NS, Query("{}")));
            ASSERT_THROWS(c.count(TEST_NS, Query("{}").maxTimeMs(1)), DBException);
        } else {
            // we are not connected to MongoDB >= 2.6, skip
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

    TEST_F(DBClientTest, CreateSimpleV0Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .version(0));
    }

    TEST_F(DBClientTest, CreateSimpleNamedV0Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .version(0)
                      .name("aFieldV0Index"));
    }

    TEST_F(DBClientTest, CreateCompoundNamedV0Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .addKey("bField", IndexSpec::kIndexTypeDescending)
                      .version(0)
                      .name("aFieldbFieldV0Index"));
    }

    TEST_F(DBClientTest, CreateSimpleV1Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .version(1));
    }

    TEST_F(DBClientTest, CreateSimpleNamedV1Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .version(1)
                      .name("aFieldV1Index"));
    }

    TEST_F(DBClientTest, CreateCompoundNamedV1Index) {
        c.createIndex(TEST_NS, IndexSpec()
                      .addKey("aField")
                      .addKey("bField", IndexSpec::kIndexTypeDescending)
                      .version(1)
                      .name("aFieldbFieldV1Index"));
    }

    TEST_F(DBClientTest, CreateUniqueSparseDropDupsIndexInBackground) {
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField")
                      .background()
                      .unique()
                      .sparse()
                      .dropDuplicates());
    }

    TEST_F(DBClientTest, CreateComplexTextIndex) {
        if (!serverGTE(&c, 2, 6)) {
            BSONObj result;
            c.runCommand("admin",
                         BSON("setParameter" << 1 << "textSearchEnabled" << true),
                         result);
                         }
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField", IndexSpec::kIndexTypeText)
                      .addKey("bField", IndexSpec::kIndexTypeText)
                      .textWeights(BSON("aField" << 100))
                      .textDefaultLanguage("spanish")
                      .textLanguageOverride("lang")
                      .textIndexVersion(serverGTE(&c, 2, 6) ? 2 : 1));
    }

    TEST_F(DBClientTest, Create2DIndex) {
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField", IndexSpec::kIndexTypeGeo2D)
                      .geo2DBits(20)
                      .geo2DMin(-120.0)
                      .geo2DMax(120.0));
    }

    TEST_F(DBClientTest, CreateHaystackIndex) {
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField", IndexSpec::kIndexTypeGeoHaystack)
                      .addKey("otherField", IndexSpec::kIndexTypeDescending)
                      .geoHaystackBucketSize(1.0));
    }

    TEST_F(DBClientTest, Create2DSphereIndex) {
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField", IndexSpec::kIndexTypeGeo2DSphere)
                      .geo2DSphereIndexVersion(serverGTE(&c, 2, 6) ? 2 : 1));
    }

    TEST_F(DBClientTest, CreateHashedIndex) {
        c.createIndex(TEST_NS,
                      IndexSpec()
                      .addKey("aField", IndexSpec::kIndexTypeHashed));
    }

} // namespace
