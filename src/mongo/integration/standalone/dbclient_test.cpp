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

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "mongo/stdx/functional.h"
#include "mongo/integration/integration_test.h"
#include "mongo/util/fail_point_service.h"
#include "mongo/util/stringutils.h"

#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"

#include "boost/thread.hpp"

using std::auto_ptr;
using std::list;
using std::string;
using std::vector;

using namespace mongo;
using namespace mongo::integration;

namespace {
const string TEST_NS = "test-dbclient.dbclient";
const string TEST_DB = "test-dbclient";
const string TEST_COLL = "dbclient";

class DBClientTest : public StandaloneTest {
public:
    DBClientTest() {
        std::string errmsg;
        _connStr = ConnectionString::parse(server().mongodbUri(), errmsg);
        c.reset(static_cast<DBClientConnection*>(_connStr.connect(errmsg)));
        c->dropCollection(TEST_NS);
        _uri = server().uri();
    }
    ConnectionString _connStr;
    std::auto_ptr<DBClientConnection> c;
    std::string _uri;
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

bool serverStorageEngine(DBClientBase* c, const StringData& engineName) {
    BSONObj result;
    c->runCommand("admin", BSON("serverStatus" << true), result);

    BSONElement storageEngineField = result.getField("storageEngine");
    if (storageEngineField.eoo() || storageEngineField.type() != Object) {
        return engineName == "mmapv1";
    }

    BSONElement storageEngineNameField = storageEngineField.Obj().getField("name");
    if (storageEngineNameField.eoo()) {
        throw std::runtime_error("storageEngine object does not have field `name`");
    }
    return engineName == storageEngineNameField.String();
}

void createUser(DBClientConnection* c,
                const std::string& db,
                const string& name,
                const string& password) {
    if (serverGTE(c, 2, 6)) {
        BSONObj ret;
        ASSERT_TRUE(c->runCommand(
            db,
            BSON("createUser" << name << "pwd" << password << "roles" << BSON_ARRAY("readWrite")),
            ret));
    } else if (serverGTE(c, 2, 4)) {
        ASSERT_NO_THROW(
            c->insert(db + ".system.users",
                      BSON("user" << name << "pwd" << c->createPasswordDigest(name, password))));
    }
}

#ifdef MONGO_SSL
const bool kCompiledWithSSL = true;
#else
const bool kCompiledWithSSL = false;
#endif

TEST_F(DBClientTest, ByPassDocumentValidation) {
    if (serverGTE(c.get(), 3, 1)) {
        c->createCollectionWithOptions(
            TEST_NS, 0, false, 0, BSON("validator" << BSON("fieldName" << GTE << 1024)));

        ASSERT_THROWS(c->insert(TEST_NS, BSON("fieldName" << 1000)), OperationException);

        ASSERT_EQUALS(c->count(TEST_NS), 0U);

        c->insert(TEST_NS, BSON("fieldName" << 1000), InsertOption_BypassDocumentValidation);

        ASSERT_EQUALS(c->count(TEST_NS), 1U);

        // Validate we fail with ByPassDocumentValidation, and unacknowledged writes against 3.2
        ASSERT_THROWS(c->insert(TEST_NS,
                                BSON("fieldName" << 1000),
                                InsertOption_BypassDocumentValidation,
                                &WriteConcern::unacknowledged),
                      std::exception);

        c->update(TEST_NS,
                  BSON("fieldName" << 1000),
                  BSON("fieldName" << 1001),
                  UpdateOption_BypassDocumentValidation);

        ASSERT_THROWS(c->update(TEST_NS, BSON("fieldName" << 1001), BSON("fieldName" << 1003)),
                      OperationException);
    }
}


TEST_F(DBClientTest, ByPassDocumentValidationBulk) {
    if (serverGTE(c.get(), 3, 1)) {
        // Note: The bulk operation tests do not set a high enough max wire version for this test to
        // work so this test is in this suite.
        c->createCollectionWithOptions(
            TEST_NS, 0, false, 0, BSON("validator" << BSON("fieldName" << GTE << 1024)));

        {
            BulkOperationBuilder bulk(c.get(), TEST_NS, true, false);

            bulk.insert(BSON("fieldName" << 1000));

            WriteResult result;

            ASSERT_THROW(bulk.execute(&WriteConcern::acknowledged, &result), std::exception);
        }

        {
            BulkOperationBuilder bulk(c.get(), TEST_NS, true, true);

            bulk.insert(BSON("fieldName" << 1000));

            WriteResult result;

            ASSERT_NO_THROW(bulk.execute(&WriteConcern::acknowledged, &result));
        }
    }
}


TEST_F(DBClientTest, ByPassDocumentValidationFindAndModify) {
    if (serverGTE(c.get(), 3, 1)) {
        c->createCollectionWithOptions(
            TEST_NS, 0, false, 0, BSON("validator" << BSON("fieldName" << GTE << 1024)));

        c->insert(TEST_NS, BSON("_id" << 1 << "fieldName" << 1024));

        ASSERT_THROWS(
            c->findAndModify(
                TEST_NS, BSON("fieldName" << 1024), BSON("$dec" << BSON("fieldName" << 1)), false),
            OperationException);

        ASSERT_THROWS(c->findAndModify(TEST_NS,
                                       BSON("fieldName" << 1024),
                                       BSON("$dec" << BSON("fieldName" << 1)),
                                       false,
                                       false,
                                       BSONObj(),
                                       BSONObj(),
                                       NULL,
                                       true),
                      OperationException);
    }
}


/* DBClient Tests */
TEST_F(DBClientTest, Save) {
    // Save a doc with autogenerated id
    c->save(TEST_NS,
            BSON("hello"
                 << "world"));
    BSONObj result = c->findOne(TEST_NS, Query());
    BSONElement id = result.getField("_id");
    ASSERT_EQUALS(id.type(), jstOID);

    // Save a doc with explicit id
    c->save(TEST_NS,
            BSON("_id"
                 << "explicit_id"
                 << "hello"
                 << "bar"));
    BSONObj doc = c->findOne(TEST_NS,
                             MONGO_QUERY("_id"
                                         << "explicit_id"));
    ASSERT_EQUALS(doc.getStringField("_id"), std::string("explicit_id"));
    ASSERT_EQUALS(doc.getStringField("hello"), std::string("bar"));

    // Save docs with _id field already present (shouldn't create new docs)
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
    c->save(TEST_NS,
            BSON("_id" << id.OID() << "hello"
                       << "world"));
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
    c->save(TEST_NS,
            BSON("_id"
                 << "explicit_id"
                 << "hello"
                 << "baz"));
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
    ASSERT_EQUALS(c->findOne(TEST_NS,
                             MONGO_QUERY("_id"
                                         << "explicit_id")).getStringField("hello"),
                  std::string("baz"));
}

TEST_F(DBClientTest, FindAndModify) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result =
        c->findAndModify(TEST_NS, BSON("i" << 1), BSON("$inc" << BSON("i" << 1)), false);

    ASSERT_EQUALS(result.getIntField("_id"), 1);
    ASSERT_EQUALS(result.getIntField("i"), 1);
    ASSERT_EQUALS(c->count(TEST_NS), 1U);
}

TEST_F(DBClientTest, FindAndModifyNoMatch) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result =
        c->findAndModify(TEST_NS, BSON("i" << 2), BSON("$inc" << BSON("i" << 1)), false);

    ASSERT_TRUE(result.isEmpty());
    ASSERT_EQUALS(c->count(TEST_NS), 1U);
}

TEST_F(DBClientTest, FindAndModifyNoMatchUpsert) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result =
        c->findAndModify(TEST_NS, BSON("i" << 2), BSON("$inc" << BSON("i" << 1)), true);

    ASSERT_TRUE(result.isEmpty());
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
}

TEST_F(DBClientTest, FindAndModifyReturnNew) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result =
        c->findAndModify(TEST_NS, BSON("i" << 1), BSON("$inc" << BSON("i" << 1)), false, true);

    ASSERT_EQUALS(result.getIntField("_id"), 1);
    ASSERT_EQUALS(result.getIntField("i"), 2);
    ASSERT_EQUALS(c->count(TEST_NS), 1U);
}

TEST_F(DBClientTest, FindAndModifyNoMatchUpsertReturnNew) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result =
        c->findAndModify(TEST_NS, BSON("i" << 2), BSON("$inc" << BSON("i" << 1)), true, true);

    ASSERT_TRUE(result.hasField("_id"));
    ASSERT_EQUALS(result.getIntField("i"), 3);
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
}

TEST_F(DBClientTest, FindAndModifySort) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
    c->insert(TEST_NS, BSON("_id" << 2 << "i" << 2));

    BSONObj result = c->findAndModify(
        TEST_NS, BSONObj(), BSON("$inc" << BSON("i" << 1)), false, false, BSON("i" << -1));

    ASSERT_EQUALS(result.getIntField("_id"), 2);
    ASSERT_EQUALS(result.getIntField("i"), 2);
    ASSERT_EQUALS(c->count(TEST_NS, BSON("_id" << 2 << "i" << 3)), 1U);
}

TEST_F(DBClientTest, FindAndModifyProjection) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result = c->findAndModify(TEST_NS,
                                      BSON("i" << 1),
                                      BSON("$inc" << BSON("i" << 1)),
                                      false,
                                      false,
                                      BSONObj(),
                                      BSON("_id" << 0));

    ASSERT_FALSE(result.hasField("_id"));
    ASSERT_TRUE(result.hasField("i"));
}

TEST_F(DBClientTest, FindAndModifyDuplicateKeyError) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
    c->createIndex(TEST_NS, IndexSpec().addKey("i").unique());
    ASSERT_THROWS(
        c->findAndModify(TEST_NS, BSON("i" << 1 << "j" << 1), BSON("$set" << BSON("k" << 1)), true),
        OperationException);
}

TEST_F(DBClientTest, FindAndModifyWriteConcern) {
    if (serverGTE(c.get(), 3, 1)) {
        c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c->findAndModify(TEST_NS,
                                          BSON("i" << 1),
                                          BSON("$inc" << BSON("i" << 1)),
                                          false,
                                          false,
                                          BSONObj(),
                                          BSONObj(),
                                          &WriteConcern::journaled);

        ASSERT_EQUALS(result.getIntField("_id"), 1);
        ASSERT_EQUALS(result.getIntField("i"), 1);
        ASSERT_EQUALS(c->count(TEST_NS), 1U);
    }
}

TEST_F(DBClientTest, FindAndRemove) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result = c->findAndRemove(TEST_NS, BSON("i" << 1));

    ASSERT_EQUALS(result.getIntField("_id"), 1);
    ASSERT_EQUALS(result.getIntField("i"), 1);
    ASSERT_EQUALS(c->count(TEST_NS), 0U);
}

TEST_F(DBClientTest, FindAndRemoveNoMatch) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result = c->findAndRemove(TEST_NS, BSON("i" << 2));

    ASSERT_TRUE(result.isEmpty());
    ASSERT_EQUALS(c->count(TEST_NS), 1U);
}

TEST_F(DBClientTest, FindAndRemoveSort) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));
    c->insert(TEST_NS, BSON("_id" << 2 << "i" << 2));

    BSONObj result = c->findAndRemove(TEST_NS, BSONObj(), BSON("i" << -1));

    ASSERT_EQUALS(result.getIntField("_id"), 2);
    ASSERT_EQUALS(result.getIntField("i"), 2);
    ASSERT_EQUALS(c->count(TEST_NS), 1U);
    ASSERT_EQUALS(c->count(TEST_NS, BSON("_id" << 1)), 1U);
}

TEST_F(DBClientTest, FindAndRemoveProjection) {
    c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

    BSONObj result = c->findAndRemove(TEST_NS, BSON("i" << 1), BSONObj(), BSON("_id" << 0));

    ASSERT_FALSE(result.hasField("_id"));
    ASSERT_TRUE(result.hasField("i"));
    ASSERT_EQUALS(c->count(TEST_NS), 0U);
}

TEST_F(DBClientTest, FindAndRemoveWriteConcern) {
    if (serverGTE(c.get(), 3, 1)) {
        c->insert(TEST_NS, BSON("_id" << 1 << "i" << 1));

        BSONObj result = c->findAndRemove(
            TEST_NS, BSON("i" << 1), BSONObj(), BSONObj(), &WriteConcern::journaled);

        ASSERT_EQUALS(result.getIntField("_id"), 1);
        ASSERT_EQUALS(result.getIntField("i"), 1);
        ASSERT_EQUALS(c->count(TEST_NS), 0U);
    }
}

TEST_F(DBClientTest, ManualGetMore) {
    // Ported from dbtests/querytests.cpp
    for (int i = 0; i < 3; ++i) {
        c->insert(TEST_NS, BSON("num" << i));
    }
    auto_ptr<DBClientCursor> cursor = c->query(TEST_NS, Query("{}"), 0, 0, 0, 0, 2);

    uint64_t cursor_id = cursor->getCursorId();
    cursor->decouple();
    cursor.reset();
    cursor = c->getMore(TEST_NS, cursor_id);
    ASSERT_TRUE(cursor->more());
    ASSERT_EQUALS(cursor->next().getIntField("num"), 2);
}

TEST_F(DBClientTest, Distinct) {
    c->insert(TEST_NS, BSON("a" << 1));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 3));

    BSONObj result = c->distinct(TEST_NS, "a");

    std::vector<BSONElement> results;
    BSONObjIterator iter(result);
    while (iter.more())
        results.push_back(iter.next());

    std::sort(results.begin(), results.end());

    ASSERT_EQUALS(results[0].Int(), 1);
    ASSERT_EQUALS(results[1].Int(), 2);
    ASSERT_EQUALS(results[2].Int(), 3);
}

TEST_F(DBClientTest, GetCollectionInfos) {
    c->dropDatabase(TEST_DB);
    ASSERT_EQUALS(c->getCollectionNames(TEST_DB).size(), 0U);

    c->createCollection(TEST_DB + ".normal");
    c->createCollection(TEST_DB + ".capped", 1, true);

    list<BSONObj> infos = c->getCollectionInfos(TEST_DB);

    // Greater than because we sometimes create system.indexes, etc...
    ASSERT_GE(infos.size(), 2U);

    size_t count_non_system = 0;

    list<BSONObj>::iterator it = infos.begin();

    while (it != infos.end()) {
        BSONObj current = *it;
        ASSERT_TRUE(current.hasField("name"));
        string name = current["name"].String();
        if (name == "normal") {
            ++count_non_system;
        } else if (name == "capped") {
            ASSERT_TRUE(current.hasField("options"));
            ++count_non_system;
        }
        ++it;
    }

    ASSERT_EQUALS(count_non_system, 2U);
}

TEST_F(DBClientTest, GetCollectionNames) {
    c->dropDatabase(TEST_DB);
    ASSERT_EQUALS(c->getCollectionNames(TEST_DB).size(), 0U);

    c->createCollection(TEST_DB + ".normal");
    c->createCollection(TEST_DB + ".capped", 1, true);

    list<string> names = c->getCollectionNames(TEST_DB);

    // Greater than because we sometimes create system.indexes, etc...
    ASSERT_GE(names.size(), 2U);

    list<string>::iterator it = names.begin();

    bool saw_normal = false;
    bool saw_capped = false;
    bool saw_indexes = false;

    while (it != names.end()) {
        if (*it == "normal") {
            ASSERT_FALSE(saw_normal);
            saw_normal = true;
        } else if (*it == "capped") {
            ASSERT_FALSE(saw_capped);
            saw_capped = true;
        } else {
            ASSERT_FALSE(saw_indexes);
            saw_indexes = true;
        }
        ++it;
    }

    ASSERT_TRUE(saw_normal);
    ASSERT_TRUE(saw_capped);
}

TEST_F(DBClientTest, EnumerateCollections) {
    const int n_collections = 9;
    const int batch_size = 3;

    for (int i = 0; i < n_collections; ++i) {
        std::stringstream ss;
        ss << TEST_DB + ".COLL" << i;
        c->createCollection(ss.str());
    }

    BSONObjBuilder bob;
    bob.appendRegex("name", "COLL\\d$");
    auto_ptr<DBClientCursor> cursor = c->enumerateCollections(TEST_DB, bob.obj(), batch_size);

    if (serverGTE(c.get(), 2, 8))
        ASSERT_EQUALS(cursor->getns(), TEST_DB + ".$cmd.listCollections");
    else
        ASSERT_EQUALS(cursor->getns(), TEST_DB + ".system.namespaces");

    int coll_count = 0;
    while (cursor->more()) {
        // The shims don't support the objsLeftInBatch cursor interface for initial batch.
        //
        // In server versions < 2.8 we apply a transformation over the cursor to filter
        // out the special collections. Since we skip some results in the batch we can't
        // apply this test here but you can see it work with enumerateIndexes which does
        // not do the same transformation.
        if ((coll_count > batch_size) && serverGTE(c.get(), 2, 8))
            ASSERT_EQUALS(cursor->objsLeftInBatch(), batch_size - (coll_count % 3));
        cursor->next();
        ++coll_count;
    }

    ASSERT_EQUALS(coll_count, n_collections);
}

TEST_F(DBClientTest, EnumerateIndexes) {
    const int n_indexes = 8;
    const int batch_size = 3;

    for (int i = 0; i < n_indexes; ++i) {
        std::stringstream ss;
        ss << i;
        c->createIndex(TEST_NS, BSON(ss.str() << 1));
    }

    auto_ptr<DBClientCursor> cursor = c->enumerateIndexes(TEST_NS, 0, batch_size);

    // Note: as per spec we aren't supposed to check the 2.8+ ns explicitly because it's
    // subject to change... but we do. If this test ever fails just safely adjust/remove.
    if (serverGTE(c.get(), 2, 8))
        ASSERT_EQUALS(cursor->getns(), TEST_DB + ".$cmd.listIndexes." + TEST_COLL);
    else
        ASSERT_EQUALS(cursor->getns(), TEST_DB + ".system.indexes");

    int index_count = 0;
    while (cursor->more()) {
        // The shims don't support the objsLeftInBatch cursor interface for initial batch.
        if (index_count > batch_size)
            ASSERT_EQUALS(cursor->objsLeftInBatch(), batch_size - (index_count % 3));
        cursor->next();
        ++index_count;
    }

    // _id index is there when we started so adding 1
    ASSERT_EQUALS(index_count, n_indexes + 1);
}

TEST_F(DBClientTest, GetCollectionNamesFiltered) {
    c->dropDatabase(TEST_DB);
    ASSERT_EQUALS(c->getCollectionNames(TEST_DB).size(), 0U);

    c->createCollection(TEST_DB + ".normal");
    c->createCollection(TEST_DB + ".capped", 1, true);

    list<string> names = c->getCollectionNames(TEST_DB, BSON("options.capped" << true));

    ASSERT_EQUALS(names.size(), 1U);

    list<string>::iterator it = names.begin();

    bool saw_capped = false;

    while (it != names.end()) {
        if (*it == "capped") {
            ASSERT_FALSE(saw_capped);
            saw_capped = true;
        }
        ++it;
    }

    ASSERT_TRUE(saw_capped);
}

TEST_F(DBClientTest, DistinctWithQuery) {
    c->insert(TEST_NS, BSON("a" << 1));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 2));
    c->insert(TEST_NS, BSON("a" << 3));

    BSONObj result = c->distinct(TEST_NS, "a", BSON("a" << GT << 1));

    std::vector<BSONElement> results;
    BSONObjIterator iter(result);
    while (iter.more())
        results.push_back(iter.next());

    std::sort(results.begin(), results.end());

    ASSERT_EQUALS(results[0].Int(), 2);
    ASSERT_EQUALS(results[1].Int(), 3);
}

TEST_F(DBClientTest, DistinctDotted) {
    c->insert(TEST_NS,
              BSON("a" << BSON("b"
                               << "a") << "c" << 12));
    c->insert(TEST_NS,
              BSON("a" << BSON("b"
                               << "b") << "c" << 12));
    c->insert(TEST_NS,
              BSON("a" << BSON("b"
                               << "c") << "c" << 12));
    c->insert(TEST_NS,
              BSON("a" << BSON("b"
                               << "c") << "c" << 12));

    BSONObj result = c->distinct(TEST_NS, "a.b");

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
    ASSERT_TRUE(serverAlive(_uri));
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
    c->insert(TEST_NS, v);
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
}

TEST_F(DBClientTest, SimpleGroup) {
    c->insert(TEST_NS,
              BSON("a" << 1 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 2 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 3 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 1 << "color"
                       << "blue"));

    std::string reduce =
        "function(current, aggregate) {"
        "if (current.color === 'green') { aggregate.green++; }"
        "if (current.a) { aggregate.a += current.a; }"
        "}";

    BSONObj initial = BSON("a" << 0 << "green" << 0);
    BSONObj cond = BSON("a" << LT << 3);
    BSONObj key = BSON("color" << 1);

    std::string finalize =
        "function(result) {"
        "result.combined = result.green + result.a;"
        "}";

    vector<BSONObj> results;
    c->group(TEST_NS, reduce, &results, initial, cond, key, finalize);

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
    c->insert(TEST_NS,
              BSON("a" << 1 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 2 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 3 << "color"
                       << "green"));
    c->insert(TEST_NS,
              BSON("a" << 1 << "color"
                       << "blue"));

    std::string reduce =
        "function(current, aggregate) {"
        "if (current.color === 'green') { aggregate.green++; }"
        "if (current.a) { aggregate.a += current.a; }"
        "}";

    BSONObj initial = BSON("a" << 0 << "green" << 0);
    BSONObj cond = BSON("a" << LT << 3);

    std::string key =
        "function(doc) {"
        "return { 'color': doc.color }"
        "}";

    std::string finalize =
        "function(result) {"
        "result.combined = result.green + result.a;"
        "}";

    vector<BSONObj> results;
    c->groupWithKeyFunction(TEST_NS, reduce, &results, initial, cond, key, finalize);

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

void getResults(DBClientCursor* cursor, std::vector<size_t>* results, boost::mutex* mut) {
    while (cursor->more()) {
        boost::lock_guard<boost::mutex> lock(*mut);
        results->push_back(cursor->next().getIntField("_id"));
    }
}

DBClientBase* makeNewConnection(const ConnectionString& connStr,
                                std::vector<DBClientBase*>* connectionAccumulator) {
    std::string errmsg;
    DBClientBase* newConn = static_cast<DBClientBase*>(connStr.connect(errmsg));
    connectionAccumulator->push_back(newConn);
    return newConn;
}

TEST_F(DBClientTest, ParallelCollectionScanUsingNewConnections) {
    bool supported = serverGTE(c.get(), 2, 6);

    if (supported) {
        const size_t numItems = 8000;
        const size_t seriesSum = (numItems * (numItems - 1)) / 2;

        for (size_t i = 0; i < numItems; ++i)
            c->insert(TEST_NS, BSON("_id" << static_cast<int>(i)));

        std::vector<DBClientBase*> connections;
        std::vector<DBClientCursor*> cursors;

        stdx::function<DBClientBase*()> factory =
            stdx::bind(&makeNewConnection, _connStr, &connections);
        c->parallelScan(TEST_NS, 3, &cursors, factory);

        std::vector<size_t> results;
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
    bool supported = serverGTE(c.get(), 2, 6);

    if (supported && serverStorageEngine(c.get(), "mmapv1")) {
        const size_t numItems = 8000;

        for (size_t i = 0; i < numItems; ++i)
            c->insert(TEST_NS, BSON("_id" << static_cast<int>(i)));

        std::vector<DBClientBase*> connections;
        std::vector<DBClientCursor*> cursors;

        stdx::function<DBClientBase*()> factory =
            stdx::bind(&makeSketchyConnection, c.get(), &connections);
        ASSERT_THROWS(c->parallelScan(TEST_NS, 3, &cursors, factory), OperationException);

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
    ASSERT_THROWS(c->insert(TEST_NS, v, InsertOption_ContinueOnError), OperationException);
    ASSERT_EQUALS(c->count(TEST_NS), 2U);
}

TEST_F(DBClientTest, GetIndexNames) {
    ASSERT_TRUE(c->getIndexNames(TEST_DB + ".fake").empty());
    ASSERT_TRUE(c->getIndexNames(TEST_NS).empty());

    c->insert(TEST_NS,
              BSON("test"
                   << "random data"));
    ASSERT_EQUALS(1U, c->getIndexNames(TEST_NS).size());

    c->createIndex(TEST_NS, BSON("test" << 1));

    list<string> names = c->getIndexNames(TEST_NS);
    ASSERT_EQUALS(2U, names.size());
    ASSERT_EQUALS("_id_", names.front());
    names.pop_front();
    ASSERT_EQUALS("test_1", names.front());
}

TEST_F(DBClientTest, GetIndexSpecs) {
    ASSERT_TRUE(c->getIndexSpecs(TEST_DB + ".fake").empty());
    ASSERT_TRUE(c->getIndexSpecs(TEST_NS).empty());

    c->insert(TEST_NS,
              BSON("test"
                   << "random data"));
    ASSERT_EQUALS(1U, c->getIndexSpecs(TEST_NS).size());

    IndexSpec spec;
    spec.addKey("test", IndexSpec::kIndexTypeAscending);
    spec.unique(true);
    c->createIndex(TEST_NS, spec);

    list<BSONObj> specs = c->getIndexSpecs(TEST_NS);

    ASSERT_EQUALS(2U, specs.size());
    ASSERT_EQUALS("_id_", specs.front()["name"].String());
    ASSERT_EQUALS(BSON("_id" << 1), specs.front()["key"].Obj());
    ASSERT_EQUALS(TEST_NS, specs.front()["ns"].String());
    specs.pop_front();
    ASSERT_EQUALS("test_1", specs.front()["name"].String());
    ASSERT_EQUALS(BSON("test" << 1), specs.front()["key"].Obj());
    ASSERT_EQUALS(TEST_NS, specs.front()["ns"].String());
    ASSERT_TRUE(specs.front()["unique"].trueValue());
}

TEST_F(DBClientTest, DropIndexes) {
    c->createIndex(TEST_NS, BSON("test" << 1));
    ASSERT_EQUALS(2U, c->getIndexNames(TEST_NS).size());
    c->dropIndexes(TEST_NS);
    ASSERT_EQUALS(1U, c->getIndexNames(TEST_NS).size());
}

TEST_F(DBClientTest, DropIndex) {
    c->createIndex(TEST_NS, BSON("test" << 1));
    c->createIndex(TEST_NS, BSON("test2" << -1));
    ASSERT_EQUALS(3U, c->getIndexNames(TEST_NS).size());

    // Interface that takes an index key obj
    c->dropIndex(TEST_NS, BSON("test" << 1));
    ASSERT_EQUALS(2U, c->getIndexNames(TEST_NS).size());

    // Interface that takes an index name
    c->dropIndex(TEST_NS, "test2_-1");
    ASSERT_EQUALS(1U, c->getIndexNames(TEST_NS).size());

    // Drop of unknown index should throw an error
    ASSERT_THROWS(c->dropIndex(TEST_NS, "test3_1"), DBException);
}

TEST_F(DBClientTest, ReIndex) {
    c->createIndex(TEST_NS, BSON("test" << 1));
    c->createIndex(TEST_NS, BSON("test2" << -1));
    ASSERT_EQUALS(3U, c->getIndexNames(TEST_NS).size());
    c->reIndex(TEST_NS);
    ASSERT_EQUALS(3U, c->getIndexNames(TEST_NS).size());
}

TEST_F(DBClientTest, Aggregate) {
    if (serverGTE(c.get(), 2, 2)) {
        BSONObj doc = BSON("hello"
                           << "world");

        BSONObj pipeline = BSON("0" << BSON("$match" << doc));

        c->insert(TEST_NS, doc);
        c->insert(TEST_NS, doc);

        std::auto_ptr<DBClientCursor> cursor = c->aggregate(TEST_NS, pipeline);
        ASSERT_TRUE(cursor.get());

        for (int i = 0; i < 2; i++) {
            ASSERT_TRUE(cursor->more());

            BSONObj result = cursor->next();
            ASSERT_TRUE(result.valid());
            ASSERT_EQUALS(result["hello"].String(), "world");
        }

        ASSERT_FALSE(cursor->more());
    }
}

TEST_F(DBClientTest, AggregateNoResults) {
    std::auto_ptr<DBClientCursor> cursor = c->aggregate(TEST_NS, BSONObj());
    ASSERT_TRUE(cursor.get());
    ASSERT_FALSE(cursor->more());
}

TEST_F(DBClientTest, CreateCollection) {
    ASSERT_FALSE(c->exists(TEST_NS));
    ASSERT_TRUE(c->createCollection(TEST_NS));
    ASSERT_FALSE(c->createCollection(TEST_NS));
    ASSERT_TRUE(c->exists(TEST_NS));

    BSONObj info;
    ASSERT_TRUE(c->runCommand(TEST_DB, BSON("collstats" << TEST_COLL), info));

    bool server26plus = serverGTE(c.get(), 2, 6);

    if (serverGTE(c.get(), 2, 2) && serverStorageEngine(c.get(), "mmapv1")) {
        ASSERT_EQUALS(info.getIntField("userFlags"), server26plus ? 1 : 0);
    }
    ASSERT_EQUALS(info.getIntField("nindexes"), 1);
}

TEST_F(DBClientTest, CreateCollectionAdvanced) {
    BSONObjBuilder opts;
    opts.append("flags", 0);
    opts.append("autoIndexId", false);
    c->createCollectionWithOptions(TEST_NS, 1, true, 1, opts.obj());

    BSONObj info;
    ASSERT_TRUE(c->runCommand(TEST_DB, BSON("collstats" << TEST_COLL), info));

    if (serverGTE(c.get(), 2, 2) && serverStorageEngine(c.get(), "mmapv1")) {
        ASSERT_EQUALS(info.getIntField("userFlags"), 0);
    }
    ASSERT_EQUALS(info.getIntField("nindexes"), 0);
}

TEST_F(DBClientTest, CountWithHint) {
    c->insert(TEST_NS, BSON("a" << 1));
    c->insert(TEST_NS, BSON("a" << 2));

    IndexSpec normal_spec;
    normal_spec.addKey("a");
    c->createIndex(TEST_NS, normal_spec);

    ASSERT_EQUALS(c->count(TEST_NS, Query("{'a': 1}").hint("_id_")), 1U);
    ASSERT_EQUALS(c->count(TEST_NS, Query().hint("_id_")), 2U);

    IndexSpec sparse_spec;
    sparse_spec.addKey("b").sparse(true);
    c->createIndex(TEST_NS, sparse_spec);

    Query good = Query("{'a': 1}").hint("b_1");
    Query bad = Query("{'a': 1}").hint("badhint");

    if (serverGTE(c.get(), 2, 6)) {
        ASSERT_EQUALS(c->count(TEST_NS, good), 0U);
        ASSERT_THROWS(c->count(TEST_NS, bad), DBException);
    } else {
        ASSERT_EQUALS(c->count(TEST_NS, good), 1U);
        ASSERT_NO_THROW(c->count(TEST_NS, bad));
    }

    ASSERT_EQUALS(c->count(TEST_NS, Query().hint("b_1")), 2U);
}

TEST_F(DBClientTest, CopyDatabaseNoAuth) {
    c->dropDatabase("copy");
    c->insert(TEST_NS, BSON("test" << true));
    ASSERT_TRUE(c->copyDatabase(TEST_DB, "copy"));
    string copy_ns = string("copy.") + TEST_COLL;
    c->exists(copy_ns);
    BSONObj doc = c->findOne(copy_ns, Query("{}"));
    ASSERT_TRUE(doc["test"].boolean());
}

TEST_F(DBClientTest, CopyDatabaseMONGODBCR) {
    // only run this test if we have auth
    if (serverGTE(c.get(), 2, 4) && !serverGTE(c.get(), 2, 7)) {
        std::string errmsg;
        createUser(c.get(), TEST_DB, "jane", "pwd");
        ASSERT_TRUE(c->auth(TEST_DB, "jane", "pwd", errmsg));

        c->dropDatabase("copy");
        c->insert(TEST_NS,
                  BSON("flavor"
                       << "vanilla"));
        // NOTE: auth doesn't run if 'fromhost' is empty, because auth is
        // not needed to copydb locally. For testing's sake just take the
        // current host and use it.
        ASSERT_TRUE(c->copyDatabase(TEST_DB, "copy", _uri, "MONGODB-CR", "jane", "pwd"));

        string copy_ns = string("copy.") + TEST_COLL;
        c->exists(copy_ns);
        BSONObj doc = c->findOne(copy_ns, Query("{}"));
        ASSERT_TRUE(doc["flavor"].type() == String);
    }
}

TEST_F(DBClientTest, CopyDatabaseSCRAMSHA1) {
    // only run this test if we have SSL and a recent enough server version
    if (serverGTE(c.get(), 2, 7)) {
        if (kCompiledWithSSL) {
            createUser(c.get(), TEST_DB, "jane", "pwd");
            std::string errmsg;
            ASSERT_TRUE(c->auth(TEST_DB, "jane", "pwd", errmsg));

            c->dropDatabase("copy");
            c->insert(TEST_NS,
                      BSON("flavor"
                           << "vanilla"));
            ASSERT_TRUE(c->copyDatabase(TEST_DB, "copy", _uri, "SCRAM-SHA-1", "jane", "pwd"));

            string copy_ns = string("copy.") + TEST_COLL;
            c->exists(copy_ns);
            BSONObj doc = c->findOne(copy_ns, Query("{}"));
            ASSERT_TRUE(doc["flavor"].type() == String);
        } else {
            ASSERT_THROWS(c->copyDatabase(TEST_DB, "copy", _uri, "SCRAM-SHA-1", "user", "pass"),
                          UserException);
        }
    }
}

TEST_F(DBClientTest, CopyDatabaseInvalidMechanism) {
    ASSERT_THROWS(c->copyDatabase(TEST_DB, "copy", _uri, "BLOOP", "user", "pass"), UserException);
}

TEST_F(DBClientTest, DBProfilingLevel) {
    DBClientWithCommands::ProfilingLevel level;
    ASSERT_TRUE(c->setDbProfilingLevel(TEST_DB, c->ProfileAll));
    ASSERT_TRUE(c->getDbProfilingLevel(TEST_DB, level, 0));
    ASSERT_EQUALS(level, c->ProfileAll);

    ASSERT_TRUE(c->setDbProfilingLevel(TEST_DB, c->ProfileSlow));
    ASSERT_TRUE(c->getDbProfilingLevel(TEST_DB, level, 0));
    ASSERT_EQUALS(level, c->ProfileSlow);

    ASSERT_TRUE(c->setDbProfilingLevel(TEST_DB, c->ProfileOff));
    ASSERT_TRUE(c->getDbProfilingLevel(TEST_DB, level, 0));
    ASSERT_EQUALS(level, c->ProfileOff);
}

TEST_F(DBClientTest, QueryJSON) {
    Query q(string("{name: 'Tyler'}"));
    BSONObj filter = q.getFilter();
    ASSERT_TRUE(filter.hasField("name"));
    ASSERT_EQUALS(filter["name"].String(), "Tyler");
}

// Used to test exhaust via query below
void nop(const BSONObj&) { /* nop */
}

// This also excercises availableOptions (which is protected)
TEST_F(DBClientTest, Exhaust) {
    for (int i = 0; i < 1000; ++i)
        c->insert(TEST_NS, BSON("num" << i));

    stdx::function<void(const BSONObj&)> f = nop;
    c->query(f, TEST_NS, Query("{}"));
}

TEST_F(DBClientTest, GetPrevError) {
    c->insert(TEST_NS, BSON("_id" << 1));
    ASSERT_THROWS(c->insert(TEST_NS, BSON("_id" << 1)), OperationException);
    c->insert(TEST_NS, BSON("_id" << 2));
    ASSERT_TRUE(c->getLastError().empty());
    ASSERT_FALSE(c->getPrevError().isEmpty());
}

TEST_F(DBClientTest, MaxScan) {
    for (int i = 0; i < 100; ++i) {
        c->insert(TEST_NS, fromjson("{}"));
    }
    vector<BSONObj> results;
    c->findN(results, TEST_NS, Query("{}"), 100);
    ASSERT_EQUALS(results.size(), 100U);
    results.clear();
    c->findN(results, TEST_NS, Query("{$query: {}, $maxScan: 50}"), 100);
    ASSERT_EQUALS(results.size(), 50U);
}

TEST_F(DBClientTest, ReturnKey) {
    c->insert(TEST_NS, BSON("a" << true << "b" << true));

    BSONObj result;

    result = c->findOne(TEST_NS, Query("{$query: {a: true}}"));
    ASSERT_TRUE(result.hasField("a"));
    ASSERT_TRUE(result.hasField("b"));

    result = c->findOne(TEST_NS, Query("{$query: {a: true}, $returnKey: true}"));
    ASSERT_FALSE(result.hasField("a"));
    ASSERT_FALSE(result.hasField("b"));

    c->createIndex(TEST_NS, BSON("a" << 1));
    result = c->findOne(TEST_NS, Query("{$query: {a: true}, $returnKey: true}"));
    ASSERT_TRUE(result.hasField("a"));
    ASSERT_FALSE(result.hasField("b"));
}

TEST_F(DBClientTest, ShowDiskLoc) {
    if (!serverGTE(c.get(), 3, 1)) {
        c->insert(TEST_NS, BSON("a" << true));

        BSONObj result;
        result = c->findOne(TEST_NS, Query("{$query: {}}"));
        ASSERT_FALSE(result.hasField("$diskLoc"));

        result = c->findOne(TEST_NS, Query("{$query: {}, $showDiskLoc: true}"));
        ASSERT_TRUE(result.hasField("$diskLoc"));
    }
}

TEST_F(DBClientTest, MaxTimeMS) {
    // Requires --setParameter=enableTestCommands=1
    c->insert(TEST_NS, BSON("a" << true));
    BSONObj result;

    // Use a dummy query in order to check if maxTimeMs() is correctly applied
    Query dummyQuery("{}");
    ASSERT_FALSE(dummyQuery.hasMaxTimeMs());
    ASSERT_EQUALS(&dummyQuery.maxTimeMs(10), &dummyQuery);
    ASSERT_TRUE(dummyQuery.hasMaxTimeMs());
    ASSERT_EQUALS(dummyQuery.getMaxTimeMs(), 10);

    if (serverGTE(c.get(), 2, 6)) {
        c->runCommand("admin",
                      BSON("configureFailPoint"
                           << "maxTimeAlwaysTimeOut"
                           << "mode" << BSON("times" << 2)),
                      result);

        Query query = Query("{}");
        Query query_max_time = Query("{}").maxTimeMs(1);

        // Check the boolean method
        ASSERT_FALSE(query.hasMaxTimeMs());
        ASSERT_TRUE(query_max_time.hasMaxTimeMs());

        // Check the getter
        ASSERT_EQUALS(query_max_time.getMaxTimeMs(), 1);

        // First test with a query
        ASSERT_NO_THROW(c->findOne(TEST_NS, query););
        ASSERT_THROWS(c->findOne(TEST_NS, query_max_time), DBException);

        // Then test with a command
        ASSERT_NO_THROW(c->count(TEST_NS, query));
        ASSERT_THROWS(c->count(TEST_NS, query_max_time), DBException);
    } else {
        // we are not connected to MongoDB >= 2.6, skip
        SUCCEED();
    }
}

TEST_F(DBClientTest, Comment) {
    c->insert(TEST_NS, BSON("a" << true));
    string profile_coll = TEST_DB + ".system.profile";
    c->dropCollection(profile_coll);
    c->setDbProfilingLevel(TEST_DB, c->ProfileAll);
    c->findOne(TEST_NS, Query("{$query: {a: 'z'}, $comment: 'wow'})"));
    c->setDbProfilingLevel(TEST_DB, c->ProfileOff);
    BSONObj result;
    if (serverGTE(c.get(), 3, 1)) {
        result = c->findOne(profile_coll,
                            BSON("ns" << TEST_NS << "op"
                                      << "query"
                                      << "query.comment"
                                      << "wow"));
    } else {
        result = c->findOne(profile_coll,
                            BSON("ns" << TEST_NS << "op"
                                      << "query"
                                      << "query.$comment"
                                      << "wow"));
    }
    ASSERT_FALSE(result.isEmpty());
}

TEST_F(DBClientTest, LazyCursor) {
    c->insert(TEST_NS, BSON("test" << true));

    DBClientCursor cursor(c.get(), TEST_NS, Query("{}").obj, 0, 0, 0, 0, 0);
    bool is_retry = false;
    cursor.initLazy(is_retry);
    ASSERT_TRUE(cursor.initLazyFinish(is_retry));

    vector<BSONObj> docs;
    while (cursor.more())
        docs.push_back(cursor.next());

    ASSERT_EQUALS(docs.size(), 1U);
    ASSERT_TRUE(docs.front()["test"].value());
}

void nop_hook(BSONObjBuilder* bob) {
    (void)bob;
}
void nop_hook_post(BSONObj, string) {}

TEST_F(DBClientTest, LazyCursorCommand) {
    c->setRunCommandHook(nop_hook);
    c->setPostRunCommandHook(nop_hook_post);
    DBClientCursor cursor(c.get(), TEST_DB + ".$cmd", Query("{dbStats: 1}").obj, 1, 0, 0, 0, 0);
    bool is_retry = false;
    cursor.initLazy(is_retry);
    ASSERT_TRUE(cursor.initLazyFinish(is_retry));

    ASSERT_TRUE(cursor.more());
    ASSERT_TRUE(cursor.next().hasField("db"));
    ASSERT_FALSE(cursor.more());
}

TEST_F(DBClientTest, InitCommand) {
    DBClientCursor cursor(c.get(), TEST_DB + ".$cmd", Query("{dbStats: 1}").obj, 1, 0, 0, 0, 0);
    ASSERT_TRUE(cursor.initCommand());

    ASSERT_TRUE(cursor.more());
    ASSERT_TRUE(cursor.next().hasField("db"));
    ASSERT_FALSE(cursor.more());
}

TEST_F(DBClientTest, GetMoreLimit) {
    c->insert(TEST_NS, BSON("num" << 1));
    c->insert(TEST_NS, BSON("num" << 2));
    c->insert(TEST_NS, BSON("num" << 3));
    c->insert(TEST_NS, BSON("num" << 4));

    // set nToReturn to 3 but batch size to 1
    // This verifies:
    //   * we can manage with multiple batches
    //   * we can correctly upgrade batchSize 1 to 2 to avoid automatic
    //     cursor closing when nReturn = 1 (wire protocol edge case)
    auto_ptr<DBClientCursor> cursor = c->query(TEST_NS, Query("{}"), 3, 0, 0, 0, 1);
    vector<BSONObj> docs;
    while (cursor->more())
        docs.push_back(cursor->next());

    ASSERT_EQUALS(docs.size(), 3U);
}

TEST_F(DBClientTest, NoGetMoreLimit) {
    c->insert(TEST_NS, BSON("num" << 1));
    c->insert(TEST_NS, BSON("num" << 2));
    c->insert(TEST_NS, BSON("num" << 3));
    c->insert(TEST_NS, BSON("num" << 4));

    // set nToReturn to 2 but batch size to 4
    // check to see if a limit of 2 takes despite the larger batch
    auto_ptr<DBClientCursor> cursor = c->query(TEST_NS, Query("{}"), 2, 0, 0, 0, 4);
    vector<BSONObj> docs;
    while (cursor->more())
        docs.push_back(cursor->next());

    ASSERT_EQUALS(docs.size(), 2U);
}

TEST_F(DBClientTest, PeekError) {
    BSONObj result;
    c->runCommand("admin", BSON("buildinfo" << true), result);

    // TODO: figure out if we can come up with query that produces $err on 2.4.x
    if (versionCmp(result["version"].toString(), "2.5.3") >= 0) {
        auto_ptr<DBClientCursor> cursor = c->query(TEST_NS, Query("{'$fake': true}"));
        ASSERT_TRUE(cursor->peekError());
    } else {
        SUCCEED();
    }
}

TEST_F(DBClientTest, DefaultWriteConcernInsert) {
    c->insert(TEST_NS, BSON("_id" << 1));
    ASSERT_THROWS(c->insert(TEST_NS, BSON("_id" << 1)), OperationException);
    ASSERT_EQUALS(c->count(TEST_NS, BSON("_id" << 1)), 1U);
}

TEST_F(DBClientTest, DefaultWriteConcernUpdate) {
    c->insert(TEST_NS, BSON("a" << true));
    ASSERT_THROWS(c->update(TEST_NS,
                            BSON("a" << true),
                            BSON("$badOp"
                                 << "blah")),
                  OperationException);
    ASSERT_EQUALS(c->count(TEST_NS, BSON("a" << true)), 1U);
}

TEST_F(DBClientTest, DefaultWriteConcernRemove) {
    ASSERT_THROWS(c->remove("BAD.$NS", BSON("a" << true)), OperationException);
}

TEST_F(DBClientTest, UnacknowledgedInsert) {
    c->insert(TEST_NS, BSON("_id" << 1));
    ASSERT_NO_THROW(c->insert(TEST_NS, BSON("_id" << 1), 0, &WriteConcern::unacknowledged));
    ASSERT_EQUALS(c->count(TEST_NS, BSON("_id" << 1)), 1U);
}

TEST_F(DBClientTest, UnacknowledgedUpdate) {
    c->insert(TEST_NS, BSON("a" << true));
    ASSERT_NO_THROW(c->update(TEST_NS,
                              BSON("a" << true),
                              BSON("$badOp"
                                   << "blah"),
                              false,
                              false,
                              &WriteConcern::unacknowledged));
    ASSERT_EQUALS(c->count(TEST_NS, BSON("a" << true)), 1U);
}

TEST_F(DBClientTest, UnacknowledgedRemove) {
    ASSERT_NO_THROW(c->remove("BAD.$NS", BSON("a" << true), false, &WriteConcern::unacknowledged));
}

TEST_F(DBClientTest, AcknowledgeMultipleNodesNonReplicated) {
    WriteConcern wc = WriteConcern().nodes(2).timeout(3000);
    ASSERT_THROWS(c->insert(TEST_NS, BSON("_id" << 1), 0, &wc), OperationException);
}

TEST_F(DBClientTest, CreateSimpleV0Index) {
    if (serverStorageEngine(c.get(), "mmapv1") && !serverGTE(c.get(), 3, 1)) {
        c->createIndex(TEST_NS, IndexSpec().addKey("aField").version(0));
    }
}

TEST_F(DBClientTest, CreateSimpleNamedV0Index) {
    if (serverStorageEngine(c.get(), "mmapv1") && !serverGTE(c.get(), 3, 1)) {
        c->createIndex(TEST_NS, IndexSpec().addKey("aField").version(0).name("aFieldV0Index"));
    }
}

TEST_F(DBClientTest, CreateCompoundNamedV0Index) {
    if (serverStorageEngine(c.get(), "mmapv1") && !serverGTE(c.get(), 3, 1)) {
        c->createIndex(TEST_NS,
                       IndexSpec()
                           .addKey("aField")
                           .addKey("bField", IndexSpec::kIndexTypeDescending)
                           .version(0)
                           .name("aFieldbFieldV0Index"));
    }
}

TEST_F(DBClientTest, CreateSimpleV1Index) {
    c->createIndex(TEST_NS, IndexSpec().addKey("aField").version(1));
}

TEST_F(DBClientTest, CreateSimpleNamedV1Index) {
    c->createIndex(TEST_NS, IndexSpec().addKey("aField").version(1).name("aFieldV1Index"));
}

TEST_F(DBClientTest, CreateCompoundNamedV1Index) {
    c->createIndex(TEST_NS,
                   IndexSpec()
                       .addKey("aField")
                       .addKey("bField", IndexSpec::kIndexTypeDescending)
                       .version(1)
                       .name("aFieldbFieldV1Index"));
}

TEST_F(DBClientTest, CreateUniqueSparseDropDupsIndexInBackground) {
    c->createIndex(
        TEST_NS,
        IndexSpec().addKey("aField").background().unique().sparse().dropDuplicatesDeprecated());
}

TEST_F(DBClientTest, CreateComplexTextIndex) {
    if (!serverGTE(c.get(), 2, 6)) {
        BSONObj result;
        c->runCommand("admin", BSON("setParameter" << 1 << "textSearchEnabled" << true), result);
    }
    c->createIndex(TEST_NS,
                   IndexSpec()
                       .addKey("aField", IndexSpec::kIndexTypeText)
                       .addKey("bField", IndexSpec::kIndexTypeText)
                       .textWeights(BSON("aField" << 100))
                       .textDefaultLanguage("spanish")
                       .textLanguageOverride("lang")
                       .textIndexVersion(serverGTE(c.get(), 2, 6) ? 2 : 1));
}

TEST_F(DBClientTest, Create2DIndex) {
    c->createIndex(TEST_NS,
                   IndexSpec()
                       .addKey("aField", IndexSpec::kIndexTypeGeo2D)
                       .geo2DBits(20)
                       .geo2DMin(-120.0)
                       .geo2DMax(120.0));
}

TEST_F(DBClientTest, CreateHaystackIndex) {
    c->createIndex(TEST_NS,
                   IndexSpec()
                       .addKey("aField", IndexSpec::kIndexTypeGeoHaystack)
                       .addKey("otherField", IndexSpec::kIndexTypeDescending)
                       .geoHaystackBucketSize(1.0));
}

TEST_F(DBClientTest, Create2DSphereIndex) {
    c->createIndex(TEST_NS,
                   IndexSpec()
                       .addKey("aField", IndexSpec::kIndexTypeGeo2DSphere)
                       .geo2DSphereIndexVersion(serverGTE(c.get(), 2, 6) ? 2 : 1));
}

TEST_F(DBClientTest, CreateHashedIndex) {
    c->createIndex(TEST_NS, IndexSpec().addKey("aField", IndexSpec::kIndexTypeHashed));
}

TEST_F(DBClientTest, CreatePartialIndex) {
    if (serverGTE(c.get(), 3, 1)) {
        c->createIndex(
            TEST_NS,
            IndexSpec().addKey("aField").partialFilterExpression(BSON("aField" << GT << 5)));

        std::list<BSONObj> indexes = c->getIndexSpecs(TEST_NS);
        ASSERT_EQUALS(2U, indexes.size());
        indexes.pop_front();
        ASSERT_EQUALS(BSON("aField" << GT << 5), indexes.front()["partialFilterExpression"].Obj());
    }
}

TEST_F(DBClientTest, CreateUser) {
    createUser(c.get(), TEST_DB, "user1", "password1");
}

// This test would fail against a fresh 2.8 if we did not use the SCRAM-SHA-1 auth mechanism
TEST_F(DBClientTest, AuthenticateUserSuccess) {
    /*  Run this test if version indicates that the server supports auth, and we can
     *  communicate with it. The server must be at least as new as 2.4, when auth was
     *  introduced. If the server is as new as 2.8, then the driver must have SSL support,
     *  to use the SCRAM-SHA-1 auth mechanism which it will likely require.
     */
    if (serverGTE(c.get(), 2, 4) && (!serverGTE(c.get(), 2, 7) || kCompiledWithSSL)) {
        createUser(c.get(), TEST_DB, "user2", "password2");
        std::string errmsg;
        ASSERT_TRUE(c->auth(TEST_DB, "user2", "password2", errmsg));
    }
}

TEST_F(DBClientTest, AuthenticateUserFailure) {
    // Run test if the server can be authed into, as per AuthenticateUserSuccess
    if (serverGTE(c.get(), 2, 4) && (!serverGTE(c.get(), 2, 7) || kCompiledWithSSL)) {
        createUser(c.get(), TEST_DB, "user3", "password3");
        std::string errmsg;
        ASSERT_FALSE(c->auth(TEST_DB, "user3", "notPassword3", errmsg));
    }
}

TEST_F(DBClientTest, ConnectionStringWithNoDB) {
    if (serverGTE(c.get(), 2, 4) && (!serverGTE(c.get(), 2, 7) || kCompiledWithSSL)) {
        createUser(c.get(), "admin", "user4", "password4");
        std::string url = "mongodb://user4:password4@" + _uri;
        std::string error;
        ConnectionString connString = ConnectionString::parse(url, error);
        ASSERT_TRUE(error.empty());
        ASSERT_NO_THROW(connString.connect(url));
    }
}

TEST_F(DBClientTest, ConnectionStringWithTestDB) {
    if (serverGTE(c.get(), 2, 4) && (!serverGTE(c.get(), 2, 7) || kCompiledWithSSL)) {
        createUser(c.get(), TEST_DB, "user5", "password5");
        std::string url = "mongodb://user5:password5@" + _uri + "/" + TEST_DB;
        std::string error;
        ConnectionString connString = ConnectionString::parse(url, error);
        ASSERT_TRUE(error.empty());
        ASSERT_NO_THROW(connString.connect(url));
    }
}

std::size_t cursorCount(DBClientCursor* cursor) {
    std::size_t count = 0;
    while (cursor->more()) {
        cursor->nextSafe();
        ++count;
    }
    return count;
}

TEST_F(DBClientTest, LimitWithMultipleBatches) {
    // We need to insert enough documents in to our collection such that we can require multiple
    // batches to fill our limit.
    std::size_t totalInserts = 25000;
    std::size_t bulkSize = 1000;
    for (std::size_t i = 0; i < totalInserts; i += bulkSize) {
        BulkOperationBuilder bulk(c.get(), TEST_NS, false, false);
        WriteResult result;
        for (std::size_t j = 0; j < bulkSize; ++j) {
            bulk.insert(BSON("i" << static_cast<long long>(i) << "j" << static_cast<long long>(j)
                                 << "a"
                                 << "bbbbbbbbbbbbbbbbbbbbbbbbbb"
                                 << "c"
                                 << "dddddddddddddddddddddddddddd"));
        }
        bulk.execute(&WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(static_cast<std::size_t>(result.nInserted()), bulkSize);
    }

    {
        // A sanity check.
        auto_ptr<DBClientCursor> limitQuery1 = c->query(TEST_NS, mongo::Query("{}"), 1);
        ASSERT_EQUALS(cursorCount(limitQuery1.get()), 1u);
    }
    {
        // One more sanity check.
        auto_ptr<DBClientCursor> limitQuery2 = c->query(TEST_NS, mongo::Query("{}"), 101);
        ASSERT_EQUALS(cursorCount(limitQuery2.get()), 101u);
    }
    {
        auto_ptr<DBClientCursor> limitQuery3 = c->query(TEST_NS, mongo::Query("{}"), 9000);
        ASSERT_EQUALS(cursorCount(limitQuery3.get()), 9000u);
    }
    {
        auto_ptr<DBClientCursor> limitQuery4 = c->query(TEST_NS, mongo::Query("{}"), 6000);
        ASSERT_EQUALS(cursorCount(limitQuery4.get()), 6000u);
    }
    {
        auto_ptr<DBClientCursor> limitQuery5 = c->query(TEST_NS, mongo::Query("{}"), 15000);
        ASSERT_EQUALS(cursorCount(limitQuery5.get()), 15000u);
    }
    {
        auto_ptr<DBClientCursor> limitQuery6 = c->query(TEST_NS, mongo::Query("{}"), 24999);
        ASSERT_EQUALS(cursorCount(limitQuery6.get()), 24999u);
    }
    {
        // Try with nToReturn 24999, batchSize 5
        auto_ptr<DBClientCursor> limitQuery6 =
            c->query(TEST_NS, mongo::Query("{}"), 24999, 0, 0, 0, 5);
        ASSERT_EQUALS(cursorCount(limitQuery6.get()), 24999u);
    }
    {
        // Try with nToReturn 1, batchSize 5
        auto_ptr<DBClientCursor> limitQuery6 = c->query(TEST_NS, mongo::Query("{}"), 1, 0, 0, 0, 5);
        ASSERT_EQUALS(cursorCount(limitQuery6.get()), 1u);
    }
    {
        // Try with nToReturn 10000, batchSize 4
        auto_ptr<DBClientCursor> limitQuery6 =
            c->query(TEST_NS, mongo::Query("{}"), 10000, 0, 0, 0, 4);
        ASSERT_EQUALS(cursorCount(limitQuery6.get()), 10000u);
    }
    {
        // Try with nToReturn 23000, batchSize 10000
        auto_ptr<DBClientCursor> limitQuery6 =
            c->query(TEST_NS, mongo::Query("{}"), 23000, 0, 0, 0, 10000);
        ASSERT_EQUALS(cursorCount(limitQuery6.get()), 23000u);
    }
}

}  // namespace
