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

#include "mongo/client/wire_protocol_writer.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

    WireProtocolWriter::WireProtocolWriter(DBClientBase* client) : _client(client) {
    }

    void WireProtocolWriter::write(
        const StringData& ns,
        const std::vector<WriteOperation*>& write_operations,
        bool ordered,
        const WriteConcern* wc,
        std::vector<BSONObj>* results
    ) {

        BufBuilder builder;

        std::vector<WriteOperation*>::const_iterator batch_begin = write_operations.begin();
        const std::vector<WriteOperation*>::const_iterator end = write_operations.end();

        while (batch_begin != end) {

            std::vector<WriteOperation*>::const_iterator batch_iter = batch_begin;

            // We must be able to fit the first item of the batch. Otherwise, the calling code
            // passed an over size write operation in violation of our contract.
            invariant(_fits(&builder, *batch_iter));

            // Begin the command for this batch.
            (*batch_iter)->startRequest(ns.toString(), ordered, &builder);

            while (true) {

                // Always safe to append here: either we just entered the loop, or all the
                // below checks passed.
                (*batch_iter)->appendSelfToRequest(&builder);

                // If the operation we just queued isn't batchable, issue what we have.
                if (!_batchableRequest((*batch_iter)->operationType()))
                    break;

                // Peek at the next operation.
                const std::vector<WriteOperation*>::const_iterator next = boost::next(batch_iter);

                // If we are out of operations, issue what we have.
                if (next == end)
                    break;

                // If the next operation is of a different type, issue what we have.
                if ((*next)->operationType() != (*batch_iter)->operationType())
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
            results->push_back(_send((*batch_iter)->operationType(), builder, wc, ns));

            // Reset the builder so we can build the next request.
            builder.reset();

            // The next batch begins with the op after the last one in the just issued batch.
            batch_begin = ++batch_iter;
        }

    }

    bool WireProtocolWriter::_fits(BufBuilder* builder, WriteOperation* op) {
        return (builder->len() + op->incrementalSize()) <= _client->getMaxMessageSizeBytes();
    }

    BSONObj WireProtocolWriter::_send(Operations opCode, const BufBuilder& builder, const WriteConcern* wc, const StringData& ns) {
        Message request;
        request.setData(opCode, builder.buf(), builder.len());
        _client->say(request);

        BSONObj result;

        if (wc->requiresConfirmation()) {
            BSONObjBuilder bob;
            bob.append("getlasterror", true);
            bob.appendElements(wc->obj());
            _client->runCommand(nsToDatabase(ns), bob.obj(), result);

            if (!result["err"].isNull()) {
                throw OperationException(result);
            }
        }

        return result;
    }

    bool WireProtocolWriter::_batchableRequest(Operations opCode) {
        return opCode == dbInsert;
    }

} // namespace mongo
