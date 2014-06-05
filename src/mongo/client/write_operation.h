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

#include <string>
#include <vector>

#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/util/net/operation.h"

namespace mongo {

    /**
     * Represents a single server side write operation and encapsulates
     * the process for encoding the operation into either a wire protocol
     * request message or a command.
     */
    class WriteOperation {
    public:
        virtual ~WriteOperation() {}

        /**
         * Returns the MongoDB wire protocol operation type represented
         * by an instance of this particular write operation.
         */
        virtual WriteOpType operationType() const = 0;

        /**
         * Returns the name for a batch of this type of write operation.
         */
        virtual const char* batchName() const = 0;

        /**
         * Returns the incremental size (in bytes) required to add this write
         * operation to a batch.
         */
        virtual int incrementalSize() const = 0;

        /**
         * Appends the preamble for a wire protocol message into the supplied
         * BufBuilder.
         *
         * This is the part of the wire protocol message after the header but
         * before the bson document portion. It typically contains the namespace,
         * flags, and potentially space reserved by the protocol for future use.
         *
         * NOTE: The size of the preamble is fixed but operation type dependant.
         */
        virtual void startRequest(const std::string& ns, bool ordered, BufBuilder* builder) const = 0;

        /**
         * Appends a document (or documents in the case of update) which describe
         * the write operation represented by an instance of this class into the
         * supplied BufBuilder.
         *
         * This method may be called multiple times by a WireProtocolWriter in order
         * to batch operations of the same type into a single wire protocol request.
         *
         * NOTE: The size of this portion of the message is flexible but the size of
         * the message itself is bounded by the server's maxMessageSizeBytes.
         */
        virtual void appendSelfToRequest(BufBuilder* builder) const = 0;

        /**
         * Appends the preamble for a write command into the supplied BSONObjBuilder.
         *
         * Typically this involves a single bson element having a key that represents
         * the operation type and a value which represents the collection to which the
         * operation will be applied. This write op element must be the first element
         * in the document according to the specification.
         */
        virtual void startCommand(const std::string& ns, BSONObjBuilder* command) const = 0;

        /**
         * Appends a single document that describes the write operation represented by
         * an instance of this class into the supplied BSONArrayBuilder.
         *
         * This method will be called multiple times by a DBClientWriter in order to batch
         * operations of the same type into a single command.
         *
         * NOTE: The ultimate size of the of the command is bounded by the sever's
         * maxBsonObjectSize.
         */
        virtual void appendSelfToCommand(BSONArrayBuilder* batch) const = 0;

        /**
         * Appends the data represented by an instance of this class to a BSONObjBuilder.
         */
        virtual void appendSelfToBSONObj(BSONObjBuilder* obj) const = 0;

        /**
         * Sets the index of this WriteOperation in the context of a larger bulk operation.
         */
        virtual void setBulkIndex(size_t index) = 0;

        /**
         * The index of this WriteOperation in the context of a larger bulk operation.
         */
        virtual size_t getBulkIndex() const = 0;
    };

} // namespace mongo
