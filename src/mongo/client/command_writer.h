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

#include "mongo/client/dbclient_writer.h"

namespace mongo {

class DBClientBase;

class CommandWriter : public DBClientWriter {
public:
    explicit CommandWriter(DBClientBase* client);

    virtual void write(const StringData& ns,
                       const std::vector<WriteOperation*>& write_operations,
                       bool ordered,
                       bool bypassDocumentValidation,
                       const WriteConcern* writeConcern,
                       WriteResult* writeResult);

private:
    void _endCommand(BSONArrayBuilder* batch,
                     WriteOperation* op,
                     bool ordered,
                     bool bypassDocumentValidation,
                     BSONObjBuilder* command);

    BSONObj _send(BSONObjBuilder* command, const WriteConcern* writeConcern, const StringData& ns);

    bool _fits(BSONArrayBuilder* builder, WriteOperation* operation);

    DBClientBase* const _client;
};

}  // namespace mongo
