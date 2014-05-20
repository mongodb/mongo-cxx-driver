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

#include "mongo/client/command_writer.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

    const int kOverhead = 8 * 1024;
    const char kOrderedKey[] = "ordered";

    CommandWriter::CommandWriter(DBClientBase* client) : _client(client) {
    }

    void CommandWriter::write(
        const StringData& ns,
        const std::vector<WriteOperation*>& write_operations,
        bool ordered,
        const WriteConcern* wc,
        std::vector<BSONObj>* results
    ) {
        bool inRequest = false;
        int opsInRequest = 0;
        Operations requestType;

        BSONObjBuilder command;
        BSONArrayBuilder batch;

        std::vector<WriteOperation*>::const_iterator iter = write_operations.begin();

        while (iter != write_operations.end()) {
            // We don't have a pending command yet
            if (!inRequest) {
                (*iter)->startCommand(ns.toString(), &command);
                inRequest = true;
                requestType = (*iter)->operationType();
            }

            // Now we have a pending request, can we add to it?
            if (requestType == (*iter)->operationType() &&
                opsInRequest < _client->getMaxWriteBatchSize()) {

                // We can add to the command, lets see if it will fit and we can batch
                if(_fits(&batch, *iter)) {
                    (*iter)->appendSelfToCommand(&batch);
                    ++opsInRequest;
                    ++iter;
                    continue;
                }

            }

            // Send the current request to the server, record the response, start a new request
            _endCommand(&batch, *iter, ordered, &command);
            results->push_back(_send(&command, wc, ns));
            inRequest = false;
            opsInRequest = 0;
        }

        // Last batch
        if (opsInRequest != 0) {
            // All of the flags are the same so just use the ones from the final op in batch
            --iter;
            _endCommand(&batch, *iter, ordered, &command);
            results->push_back(_send(&command, wc, ns));
        }
    }

    bool CommandWriter::_fits(BSONArrayBuilder* builder, WriteOperation* op) {
        int opSize = op->incrementalSize();
        int maxSize = _client->getMaxBsonObjectSize();

        // This update is too large to ever be sent as a command, assert
        uassert(0, "update command exceeds maxBsonObjectSize", opSize <= maxSize);

        return (builder->len() + opSize + kOverhead) <= maxSize;
    }

    void CommandWriter::_endCommand(
        BSONArrayBuilder* batch,
        WriteOperation* op,
        bool ordered,
        BSONObjBuilder* command
    ) {
        command->append(op->batchName(), batch->arr());
        command->append(kOrderedKey, ordered);
    }

    BSONObj CommandWriter::_send(BSONObjBuilder* builder, const WriteConcern* wc, const StringData& ns) {
        builder->append("writeConcern", wc->obj());

        BSONObj result;
        bool commandWorked = _client->runCommand(nsToDatabase(ns), builder->obj(), result);

        if (!commandWorked || result.hasField("writeErrors") || result.hasField("writeConcernError")) {
            throw OperationException(result);
        }

        return result;
    }

} // namespace mongo
