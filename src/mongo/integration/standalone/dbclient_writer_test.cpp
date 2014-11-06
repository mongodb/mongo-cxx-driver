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

#include "mongo/client/command_writer.h"
#include "mongo/client/dbclient.h"
#include "mongo/client/delete_write_operation.h"
#include "mongo/client/insert_write_operation.h"
#include "mongo/client/update_write_operation.h"
#include "mongo/client/wire_protocol_writer.h"
#include "mongo/client/write_result.h"

using std::string;
using std::vector;

using namespace mongo;
using namespace mongo::integration;

namespace {

    const string TEST_NS = "test.dbclient_writer";

    template <typename T>
    struct RequiredWireVersion;

    template <>
    struct RequiredWireVersion<WireProtocolWriter> {
        static const int value = 0;
    };

    template <>
    struct RequiredWireVersion<CommandWriter> {
        static const int value = 2;
    };

    template <typename T>
    class DBClientWriterTest : public StandaloneTest {
    public:
        DBClientWriterTest() {
            c.connect(server().uri());
            c.dropCollection(TEST_NS);
            writer = new T(&c);
        }
        ~DBClientWriterTest() { delete writer; }

        // Returns true if connection supports the parameterized DBClientWriter
        bool testSupported() {
            return (c.getMaxWireVersion() >= RequiredWireVersion<T>::value);
        }

        DBClientWriter* writer;
        DBClientConnection c;
    };

    typedef ::testing::Types<WireProtocolWriter, CommandWriter> DBClientWriters;
    TYPED_TEST_CASE(DBClientWriterTest, DBClientWriters);

    TYPED_TEST(DBClientWriterTest, SingleInsert) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert(BSON("a" << 1));
        inserts.push_back(&insert);
        WriteResult result;
        this->writer->write(TEST_NS, inserts, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.findOne(TEST_NS, Query())["a"].numberInt(), 1);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedInserts) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("a" << 1));
        InsertWriteOperation insert2(BSON("a" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        this->writer->write(TEST_NS, inserts, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_FALSE(result.hasErrors());
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedInserts) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("a" << 1));
        InsertWriteOperation insert2(BSON("a" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        this->writer->write(TEST_NS, inserts, false, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_FALSE(result.hasErrors());
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedInsertsWithError) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("_id" << 1));
        InsertWriteOperation insert2(BSON("_id" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        ASSERT_THROWS(
            this->writer->write(TEST_NS, inserts, true, &WriteConcern::acknowledged, &result),
            OperationException
        );
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 2}")), 0U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedInsertsWithError) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("_id" << 1));
        InsertWriteOperation insert2(BSON("_id" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        ASSERT_THROWS(
            this->writer->write(TEST_NS, inserts, false, &WriteConcern::acknowledged, &result),
            OperationException
        );
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedInsertsWithErrorNoConcern) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("_id" << 1));
        InsertWriteOperation insert2(BSON("_id" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        this->writer->write(TEST_NS, inserts, true, &WriteConcern::unacknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 2}")), 0U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedInsertsWithErrorNoConcern) {
        if (!this->testSupported()) return;
        vector<WriteOperation*> inserts;
        InsertWriteOperation insert1(BSON("_id" << 1));
        InsertWriteOperation insert2(BSON("_id" << 2));
        inserts.push_back(&insert1);
        inserts.push_back(&insert1);
        inserts.push_back(&insert2);
        WriteResult result;
        this->writer->write(TEST_NS, inserts, false, &WriteConcern::unacknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{_id: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, SingleUpdate) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation update(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        updates.push_back(&update);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, SingleMultiUpdate) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation update(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), UpdateOption_Multi);
        updates.push_back(&update);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 2U);
    }

    TYPED_TEST(DBClientWriterTest, SingleUpsertDoesUpdate) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 2));

        vector<WriteOperation*> updates;
        UpdateWriteOperation update(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), UpdateOption_Upsert);
        updates.push_back(&update);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 2U);
    }

    TYPED_TEST(DBClientWriterTest, SingleUpsertDoesUpsert) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 2));

        vector<WriteOperation*> updates;
        UpdateWriteOperation update(BSON("a" << 3), BSON("$set" << BSON("a" << 2)), UpdateOption_Upsert);
        updates.push_back(&update);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 2U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedUpdates) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("b" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedUpdates) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("b" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        this->writer->write(TEST_NS, updates, false, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedUpdatesWithError) {
        if (!this->testSupported()) return;
        this->c.createIndex(TEST_NS, IndexSpec().addKeys(BSON("a" << 1)).unique());
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("a" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        ASSERT_THROWS(
            this->writer->write(TEST_NS, updates, true, &WriteConcern::acknowledged, &result),
            OperationException
        );
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedUpdatesWithError) {
        if (!this->testSupported()) return;
        this->c.createIndex(TEST_NS, IndexSpec().addKeys(BSON("a" << 1)).unique());
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("a" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        ASSERT_THROWS(
            this->writer->write(TEST_NS, updates, false, &WriteConcern::acknowledged, &result),
            OperationException
        );
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedUpdatesWithErrorNoConcern) {
        if (!this->testSupported()) return;
        this->c.createIndex(TEST_NS, IndexSpec().addKeys(BSON("a" << 1)).unique());
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("a" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        this->writer->write(TEST_NS, updates, true, &WriteConcern::unacknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedUpdatesWithErrorNoConcern) {
        if (!this->testSupported()) return;
        this->c.createIndex(TEST_NS, IndexSpec().addKeys(BSON("a" << 1)).unique());
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> updates;
        UpdateWriteOperation updateA(BSON("a" << 1), BSON("$set" << BSON("a" << 2)), 0);
        UpdateWriteOperation updateB(BSON("b" << 1), BSON("$set" << BSON("a" << 2)), 0);
        updates.push_back(&updateA);
        updates.push_back(&updateB);
        WriteResult result;
        this->writer->write(TEST_NS, updates, false, &WriteConcern::unacknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, SingleDelete) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 0));
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 2));

        vector<WriteOperation*> deletes;
        DeleteWriteOperation delete_op(BSON("a" << BSON("$gt" << 0)), 0);
        deletes.push_back(&delete_op);
        WriteResult result;
        this->writer->write(TEST_NS, deletes, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 0}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 0U);
    }

    TYPED_TEST(DBClientWriterTest, SingleDeleteJustOne) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 0));
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("a" << 2));

        vector<WriteOperation*> deletes;
        DeleteWriteOperation delete_op(BSON("a" << BSON("$gt" << 0)), RemoveOption_JustOne);
        deletes.push_back(&delete_op);
        WriteResult result;
        this->writer->write(TEST_NS, deletes, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 0}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 2}")), 1U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleOrderedDeletes) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> deletes;
        DeleteWriteOperation deleteA(BSON("a" << 1), 0);
        DeleteWriteOperation deleteB(BSON("b" << 1), 0);
        deletes.push_back(&deleteA);
        deletes.push_back(&deleteB);
        WriteResult result;
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
        this->writer->write(TEST_NS, deletes, true, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 0U);
    }

    TYPED_TEST(DBClientWriterTest, MultipleUnorderedDeletes) {
        if (!this->testSupported()) return;
        this->c.insert(TEST_NS, BSON("a" << 1));
        this->c.insert(TEST_NS, BSON("b" << 1));

        vector<WriteOperation*> deletes;
        DeleteWriteOperation deleteA(BSON("a" << 1), 0);
        DeleteWriteOperation deleteB(BSON("b" << 1), 0);
        deletes.push_back(&deleteA);
        deletes.push_back(&deleteB);
        WriteResult result;
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 1U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 1U);
        this->writer->write(TEST_NS, deletes, false, &WriteConcern::acknowledged, &result);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{a: 1}")), 0U);
        ASSERT_EQUALS(this->c.count(TEST_NS, Query("{b: 1}")), 0U);
    }

}
