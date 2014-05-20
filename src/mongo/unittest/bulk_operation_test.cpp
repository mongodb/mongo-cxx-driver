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

#include "mongo/unittest/integration_test.h"

#include "mongo/client/dbclient.h"

namespace {

    using std::string;
    using std::vector;

    using namespace mongo;
    using namespace mongo::unittest;

    const string TEST_NS = "test.bulk_operation";

    class BulkOperationTest : public ::testing::Test {
    public:
        BulkOperationTest() {
            c.connect(string("localhost:") + integrationTestParams.port);
            c.dropCollection(TEST_NS);
        }
        ~BulkOperationTest() {}

        DBClientConnection c;
    };

    TEST_F(BulkOperationTest, Insert) {
        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.insert(BSON("a" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        BSONObj doc = c.findOne(TEST_NS, Query("{}").obj);
        ASSERT_EQUALS(doc["a"].numberInt(), 1);
    }

    TEST_F(BulkOperationTest, UpdateOneMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).updateOne(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
    }

    TEST_F(BulkOperationTest, UpdateMultiMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).update(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{a: 1, x: 1}").obj), 2U);
    }

    TEST_F(BulkOperationTest, UpdateAllDocuments) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(fromjson("{}")).update(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 3U);
    }

    TEST_F(BulkOperationTest, ReplaceEntireDocument) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).replaceOne(BSON("x" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_FALSE(c.findOne(TEST_NS, Query("{x: 1}").obj).hasField("a"));
    }

    TEST_F(BulkOperationTest, UpsertOneMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).upsert().updateOne(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, UpsertOneNotMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 2)).upsert().updateOne(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_TRUE(c.findOne(TEST_NS, Query("{x: 1}").obj).hasField("a"));
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, UpsertMultiMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).upsert().update(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 2U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, UpsertMultiNotMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 2)).upsert().update(BSON("$inc" << BSON("x" << 1)));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_TRUE(c.findOne(TEST_NS, Query("{x: 1}").obj).hasField("a"));
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, UpsertReplaceMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).upsert().replaceOne(BSON("x" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{a: 1}").obj), 1U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, UpsertReplaceNotMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 2)).upsert().replaceOne(BSON("x" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_TRUE(results.front().hasField("upserted"));
        ASSERT_EQUALS(c.count(TEST_NS, Query("{x: 1}").obj), 1U);
        ASSERT_FALSE(c.findOne(TEST_NS, Query("{x: 1}").obj).hasField("a"));
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 3U);
    }

    TEST_F(BulkOperationTest, RemoveOneMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).removeOne();

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.front().getIntField("n"), 1);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 2U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{a: 1}").obj), 1U);
    }

    TEST_F(BulkOperationTest, RemoveAllMatchingSelector) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(BSON("a" << 1)).remove();

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.front().getIntField("n"), 2);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 1U);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{a: 1}").obj), 0U);
    }

    TEST_F(BulkOperationTest, RemoveAll) {
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("a" << 1));
        c.insert(TEST_NS, BSON("b" << 1));

        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.find(fromjson("{}")).remove();

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.front().getIntField("n"), 3);
        ASSERT_EQUALS(c.count(TEST_NS, Query("{}").obj), 0U);
    }

    TEST_F(BulkOperationTest, MultipleOrderedOperations) {
        BulkOperationBuilder bulk(&c, TEST_NS, true);
        bulk.insert(BSON("a" << 1));
        bulk.insert(BSON("a" << 1));
        bulk.insert(BSON("b" << 1));
        bulk.find(BSON("a" << 1)).updateOne(BSON("$set" << BSON("a" << 2)));
        bulk.find(BSON("a" << 1)).remove();
        bulk.insert(BSON("b" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.size(), 4U);
    }

    TEST_F(BulkOperationTest, MultipleUnorderedOperations) {
        BulkOperationBuilder bulk(&c, TEST_NS, false);
        bulk.insert(BSON("a" << 1));
        bulk.insert(BSON("a" << 1));
        bulk.insert(BSON("b" << 1));
        bulk.find(BSON("a" << 1)).updateOne(BSON("$set" << BSON("a" << 2)));
        bulk.find(BSON("a" << 1)).remove();
        bulk.insert(BSON("b" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.size(), 3U);
    }

    TEST_F(BulkOperationTest, TwoBatches) {
        BulkOperationBuilder bulk(&c, TEST_NS, false);
        for (int i=0; i < c.getMaxWriteBatchSize() + 1; ++i)
            bulk.insert(BSON("a" << 1));

        vector<BSONObj> results;
        bulk.execute(&WriteConcern::acknowledged, &results);

        ASSERT_EQUALS(results.size(), 2U);
    }

} // namespace
