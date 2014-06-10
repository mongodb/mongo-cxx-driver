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

#include "mongo/client/bulk_operation_builder.h"

#include <algorithm>

#include "mongo/client/dbclientinterface.h"
#include "mongo/client/insert_write_operation.h"
#include "mongo/client/write_options.h"
#include "mongo/client/write_result.h"

namespace mongo {

    namespace {
        inline bool compare(WriteOperation* const lhs, WriteOperation* const rhs) {
            return lhs->operationType() > rhs->operationType();
        }
    } // namespace

    BulkOperationBuilder::BulkOperationBuilder(DBClientBase* const client, const std::string& ns, bool ordered)
        : _client(client)
        , _ns(ns)
        , _ordered(ordered)
        , _executed(false)
        , _currentIndex(0)
    {}

    BulkOperationBuilder::~BulkOperationBuilder() {
        std::vector<WriteOperation*>::iterator it;
        for (it = _write_operations.begin(); it != _write_operations.end(); ++it)
            delete *it;
    }

    BulkUpdateBuilder BulkOperationBuilder::find(const BSONObj& selector) {
        return BulkUpdateBuilder(this, selector);
    }

    void BulkOperationBuilder::insert(const BSONObj& doc) {
        InsertWriteOperation* insert_op = new InsertWriteOperation(doc);
        enqueue(insert_op);
    }

    void BulkOperationBuilder::execute(const WriteConcern* writeConcern, WriteResult* writeResult) {
        uassert(0, "Bulk operations cannot be re-executed", !_executed);
        uassert(0, "Bulk operations cannot be executed without any operations",
            !_write_operations.empty());

        _executed = true;

        if (!_ordered)
            std::sort(_write_operations.begin(), _write_operations.end(), compare);

        // This signals to the DBClientWriter that we cannot batch inserts together
        // over the wire protocol and must send them individually to the server in
        // order to understand what happened to them.
        writeResult->_requiresDetailedInsertResults = true;

        _client->_write(_ns, _write_operations, _ordered, writeConcern, writeResult);
    }

    void BulkOperationBuilder::enqueue(WriteOperation* operation) {
        operation->setBulkIndex(_currentIndex++);
        _write_operations.push_back(operation);
    }

} // namespace mongo
