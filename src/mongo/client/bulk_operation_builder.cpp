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
        {}

    BulkOperationBuilder::~BulkOperationBuilder() {
        std::vector<WriteOperation*>::iterator it;
        for (it = _write_operations.begin(); it != _write_operations.end(); ++it)
            delete *it;
    }

    BulkWriteOperation BulkOperationBuilder::find(const BSONObj& selector) {
        return BulkWriteOperation(this, selector);
    }

    void BulkOperationBuilder::insert(const BSONObj& doc) {
        InsertWriteOperation* insert_op = new InsertWriteOperation(doc);
        enqueue(insert_op);
    }

    void BulkOperationBuilder::execute(const WriteConcern* wc, std::vector<BSONObj>* results) {
        if (!_ordered)
            std::sort(_write_operations.begin(), _write_operations.end(), compare);
        _client->_write(_ns, _write_operations, _ordered, wc, results);
    }

    void BulkOperationBuilder::enqueue(WriteOperation* operation) {
        _write_operations.push_back(operation);
    }

} // namespace mongo
