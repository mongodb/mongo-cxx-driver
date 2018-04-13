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

#include <vector>

#include "mongo/client/write_operation.h"

namespace mongo {

class WriteConcern;
class WriteResult;

class DBClientWriter {
public:
    virtual ~DBClientWriter(){};

    // This function assumes that WriteOperations have been checked to ensure
    // involved objects are less than the client's maximum BSON object size.
    virtual void write(const StringData& ns,
                       const std::vector<WriteOperation*>& write_operations,
                       bool ordered,
                       bool bypassDocumentValidation,
                       const WriteConcern* writeConcern,
                       WriteResult* writeResult) = 0;
};

}  // namespace mongo
