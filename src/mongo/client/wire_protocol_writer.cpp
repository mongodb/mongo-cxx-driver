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
        bool inRequest = false;
        int opsInRequest = 0;
        Operations requestType;

        BufBuilder builder;

        std::vector<WriteOperation*>::const_iterator iter = write_operations.begin();

        while (iter != write_operations.end()) {
            // We don't have a pending request yet
            if (!inRequest) {
                (*iter)->startRequest(ns.toString(), ordered, &builder);
                inRequest = true;
                requestType = (*iter)->operationType();
            }

            // now we have a pending request, can we add to it?
            if (requestType == (*iter)->operationType() &&
                opsInRequest < _client->getMaxWriteBatchSize()) {

                // We can add to the request, lets see if it will fit and we can batch
                if(_fits(&builder, *iter)) {
                    (*iter)->appendSelfToRequest(&builder);
                    ++opsInRequest;
                    ++iter;

                    if (_batchableRequest(requestType))
                        continue;
                }
            }

            // Send the current request to the server, record the response, start a new request
            results->push_back(_send(requestType, builder, wc, ns));
            inRequest = false;
            opsInRequest = 0;
            builder.reset();
        }

        // Last batch
        if (opsInRequest != 0)
            results->push_back(_send(requestType, builder, wc, ns));
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
