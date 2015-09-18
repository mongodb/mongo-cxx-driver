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

#include "mongo/bson/bsonobj.h"
#include "mongo/client/bulk_upsert_builder.h"

namespace mongo {

class BulkOperationBuilder;

/**
 * Class for constructing bulk write operations which have an applied filter.
 *
 * Not to be instantiated directly. Comes into being via the find() method on
 * BulkOperationBuilder.
 */
class MONGO_CLIENT_API BulkUpdateBuilder {
    friend class BulkOperationBuilder;

public:
    /**
     * Enqueues an operation which updates a single document matching the selector by
     * applying the supplied update document.
     *
     * @param update The update to apply to matching documents.
     */
    void updateOne(const BSONObj& update);

    /**
     * Enqueues an operation which updates any document matching the selector by applying
     * the supplied update document.
     *
     * @param update The update to apply to matching documents.
     */
    void update(const BSONObj& update);

    /**
     * Enqueues an operation which replaces a single document matching the selector
     * with the supplied replacement.
     */
    void replaceOne(const BSONObj& replacement);

    /**
     * Enqueues an operation which removes any document matching the selector.
     */
    void remove();

    /**
     * Enqueues an operation which removes a single document matching the selector.
     */
    void removeOne();

    /**
     * Specifies that this write operation will be an upsert.
     *
     * @return BulkUpsertOperation A BulkUpdateBuilder that is an upsert.
     */
    BulkUpsertBuilder upsert();

private:
    BulkOperationBuilder* const _builder;
    const BSONObj& _selector;

    /* Only created by freind class BulkOperationBuilder */
    BulkUpdateBuilder(BulkOperationBuilder* const builder, const BSONObj& selector);
};

}  // namespace mongo
