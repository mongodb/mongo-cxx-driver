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

#pragma once

#include <vector>

#include "mongo/bson/bsonobj.h"
#include "mongo/client/bulk_update_builder.h"
#include "mongo/client/write_result.h"

namespace mongo {

class DBClientBase;
class WriteConcern;
class WriteOperation;

/**
 * Class for constructing and executing bulk operations against MongoDB via a
 * fluent API.
 *
 * Example Usage:
 *
 * BulkOperationBuilder bulk(...);
 * bulk.insert(<BSONObj>);
 * bulk.insert(<BSONObj>);
 * bulk.find(<BSONObj>).updateOne(<BSONObj>);
 *
 * vector<BSONObj> results;
 * bulk.execute(<WriteConcern>, &results);
 *
 * Usually not instantiated directly, instead it usually comes into being via
 * the following DBClientBase methods:
 *
 *      initializeOrderedBulkOperation()
 *      intiializeUnorderedBulkOperation()
 *
 * The class is able to optimize unordered operations by grouping similar ones
 * into batches instead of sending them individually to the server. This means
 * that unordered writes are non-deterministic. This is by design.
 */
class MONGO_CLIENT_API BulkOperationBuilder {
    /* Enable operations of this type to append themselves to enqueue themselves */
    friend class BulkUpdateBuilder;

    /* Enable operations of this type to append themselves to enqueue themselves */
    friend class BulkUpsertBuilder;

public:
    /**
     * BulkOperationBuilder constructor
     *
     * DBClientBase::initializeOrderedBulkOperation will set ordered to true
     * DBClientBase::initializeUnorderedBulkOperation will set ordered to false
     *
     * @param client The connection to use.
     * @param ns The namespace to apply the operations to.
     * @param ordered Whether or not ordering matters for these operations.
     * param bypassDocumentValidation Whether to bypass document validation for these operations,
     */
    BulkOperationBuilder(DBClientBase* const client,
                         const std::string& ns,
                         bool ordered,
                         bool bypassDocumentValidation = false);

    /* Deletes all of the WriteOperations that were created during the Builder's lifetime */
    ~BulkOperationBuilder();

    /**
     * Supplies a filter to select a subset of documents on which to apply an operation.
     * The operation that is ultimately enqueued as part of this bulk operation depends on
     * the subsequent method calls made to the returned BulkWriteOperation object.
     *
     * @param selector A BSONObj that describes the objects to modify.
     * @return BulkWriteOperation A BulkWriteOperation with the selector specified.
     */
    BulkUpdateBuilder find(const BSONObj& selector);

    /**
     * Enqueues an insert write operation to be executed as part of the bulk operation.
     *
     * @param doc The document to enqueue.
     */
    void insert(const BSONObj& doc);

    /**
     * Executes the bulk operation.
     *
     * @param wc The Write concern for the entire bulk operation. 0 = default (acknowledged);
     * @param results Vector where the results of operations will go.
     */
    void execute(const WriteConcern* writeConcern, WriteResult* writeResult);

private:
    void enqueue(WriteOperation* const operation);

    DBClientBase* const _client;
    const std::string _ns;
    const bool _ordered;
    const bool _bypassDocumentValidation;
    bool _executed;
    size_t _currentIndex;
    std::vector<WriteOperation*> _write_operations;
};

}  // namespace mongo
