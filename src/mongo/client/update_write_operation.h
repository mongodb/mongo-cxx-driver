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

#include "mongo/client/write_operation_base.h"

namespace mongo {

class UpdateWriteOperation : public WriteOperationBase {
public:
    UpdateWriteOperation(const BSONObj& selector, const BSONObj& update, int flags);

    virtual WriteOpType operationType() const;
    virtual const char* batchName() const;
    virtual int incrementalSize() const;

    virtual void startRequest(const std::string& ns, bool ordered, BufBuilder* builder) const;
    virtual void appendSelfToRequest(BufBuilder* builder) const;

    virtual void startCommand(const std::string& ns, BSONObjBuilder* command) const;
    virtual void appendSelfToCommand(BSONArrayBuilder* batch) const;

    virtual void appendSelfToBSONObj(BSONObjBuilder* obj) const;

private:
    const BSONObj _selector;
    const BSONObj _update;
    const int _flags;
};

}  // namespace mongo
