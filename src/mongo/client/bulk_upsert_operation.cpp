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

#include "mongo/client/bulk_upsert_operation.h"

#include "mongo/client/bulk_operation_builder.h"
#include "mongo/client/update_write_operation.h"
#include "mongo/client/write_options.h"

namespace mongo {

    BulkUpsertOperation::BulkUpsertOperation(BulkOperationBuilder* const builder, const BSONObj& selector)
        : _builder(builder)
        , _selector(selector)
        {}

    void BulkUpsertOperation::updateOne(const BSONObj& update) {
        UpdateWriteOperation* update_op = new UpdateWriteOperation(
            _selector, update, UpdateOption_Upsert);
        _builder->enqueue(update_op);
    }

    void BulkUpsertOperation::update(const BSONObj& update) {
        UpdateWriteOperation* update_op = new UpdateWriteOperation(
            _selector, update, UpdateOption_Upsert + UpdateOption_Multi);
        _builder->enqueue(update_op);
    }

    void BulkUpsertOperation::replaceOne(const BSONObj& replacement) {
        UpdateWriteOperation* update_op = new UpdateWriteOperation(
            _selector, replacement, UpdateOption_Upsert);
        _builder->enqueue(update_op);
    }

}
