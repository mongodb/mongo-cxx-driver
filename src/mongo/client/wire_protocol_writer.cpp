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

#include "mongo/client/wire_protocol_writer.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/client/write_result.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

    WireProtocolWriter::WireProtocolWriter(DBClientBase* client) : _client(client) {
    }

    void WireProtocolWriter::write(
        const StringData& ns,
        const std::vector<WriteOperation*>& write_operations,
        bool ordered,
        const WriteConcern* writeConcern,
        WriteResult* writeResult
    ) {
        // Effectively a map of batch relative indexes to WriteOperations
        std::vector<WriteOperation*> batchOps;

        BufBuilder builder;

        std::vector<WriteOperation*>::const_iterator batch_begin = write_operations.begin();
        const std::vector<WriteOperation*>::const_iterator end = write_operations.end();

        while (batch_begin != end) {

            std::vector<WriteOperation*>::const_iterator batch_iter = batch_begin;

            // We must be able to fit the first item of the batch. Otherwise, the calling code
            // passed an over size write operation in violation of our contract.
            invariant(_fits(&builder, *batch_iter));

            // Set the current operation type for this batch
            const WriteOpType batchOpType = (*batch_iter)->operationType();

            // Begin the command for this batch.
            (*batch_iter)->startRequest(ns.toString(), ordered, &builder);

            while (true) {

                // Always safe to append here: either we just entered the loop, or all the
                // below checks passed.
                (*batch_iter)->appendSelfToRequest(&builder);

                // Associate batch index with WriteOperation
                batchOps.push_back(*batch_iter);

                // If the operation we just queued isn't batchable, issue what we have.
                if (!_batchableRequest(batchOpType, writeResult))
                    break;

                // Peek at the next operation.
                const std::vector<WriteOperation*>::const_iterator next = boost::next(batch_iter);

                // If we are out of operations, issue what we have.
                if (next == end)
                    break;

                // If the next operation is of a different type, issue what we have.
                if ((*next)->operationType() != batchOpType)
                    break;

                // If adding the next op would put us over the limit of ops in a batch, issue
                // what we have.
                if (std::distance(batch_begin, next) >= _client->getMaxWriteBatchSize())
                    break;

                // If we can't put the next item into the current batch, issue what we have.
                if (!_fits(&builder, *next))
                    break;

                // OK to proceed to next op
                batch_iter = next;
            }

            // Issue the complete command.
            BSONObj batchResult = _send(batchOpType, builder, writeConcern, ns);

            // Merge this batch's result into the result for all batches written.
            writeResult->_mergeGleResult(batchOps, batchResult);
            batchOps.clear();

            // Check write result for errors if we are doing ordered processing or last op
            bool lastOp = *batch_iter == write_operations.back();
            if (ordered || lastOp)
                writeResult->_check(lastOp);

            // Reset the builder so we can build the next request.
            builder.reset();

            // The next batch begins with the op after the last one in the just issued batch.
            batch_begin = ++batch_iter;
        }

    }

    bool WireProtocolWriter::_fits(BufBuilder* builder, WriteOperation* op) {
        return (builder->len() + op->incrementalSize()) <= _client->getMaxMessageSizeBytes();
    }

    BSONObj WireProtocolWriter::_send(
        WriteOpType opCode,
        const BufBuilder& builder,
        const WriteConcern* writeConcern,
        const StringData& ns
    ) {
        Message request;
        request.setData(opCode, builder.buf(), builder.len());
        _client->say(request);

        BSONObj result;

        if (writeConcern->requiresConfirmation()) {
            BSONObjBuilder bob;
            bob.append("getlasterror", true);
            bob.appendElements(writeConcern->obj());

            bool commandWorked = _client->runCommand(nsToDatabase(ns), bob.obj(), result);

            if (!commandWorked) throw OperationException(result);
        }

        return result;
    }

    bool WireProtocolWriter::_batchableRequest(WriteOpType opCode, const WriteResult* const writeResult) {
        /*
         * In order to get detailed write information using the legacy MongoDB wire protocol
         * you must send individual messages for each write. Inserts are the only type of write
         * that is batchable using the wire protocol so we must take care to only batch them
         * if and only if we do not require detailed insert results.
         *
         * As an example: Legacy inserts of a vector of BSONObj do not require detailed insert
         * results and should be performed in a batch to increase performance. However, bulk
         * operations require detailed results and thus must be executed serially when using
         * the wire protocol.
         */
        return (opCode == dbWriteInsert && !(writeResult->_requiresDetailedInsertResults));
    }

} // namespace mongo
