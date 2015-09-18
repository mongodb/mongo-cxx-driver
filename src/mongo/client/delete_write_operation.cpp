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

#include "mongo/client/delete_write_operation.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/client/write_options.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

namespace {
const char kCommandKey[] = "delete";
const char kBatchName[] = "deletes";
const char kSelectorKey[] = "q";
const char kLimitKey[] = "limit";
}  // namespace

DeleteWriteOperation::DeleteWriteOperation(const BSONObj& selector, int flags)
    : _selector(selector), _flags(flags) {}

WriteOpType DeleteWriteOperation::operationType() const {
    return dbWriteDelete;
}

int DeleteWriteOperation::incrementalSize() const {
    return _selector.objsize();
}

const char* DeleteWriteOperation::batchName() const {
    return kBatchName;
}

void DeleteWriteOperation::startRequest(const std::string& ns, bool, BufBuilder* builder) const {
    builder->appendNum(0);
    builder->appendStr(ns);
    builder->appendNum(_flags);
}

void DeleteWriteOperation::appendSelfToRequest(BufBuilder* builder) const {
    _selector.appendSelfToBufBuilder(*builder);
}

void DeleteWriteOperation::startCommand(const std::string& ns, BSONObjBuilder* command) const {
    command->append(kCommandKey, nsToCollectionSubstring(ns));
}

void DeleteWriteOperation::appendSelfToCommand(BSONArrayBuilder* batch) const {
    BSONObjBuilder updateBuilder;
    appendSelfToBSONObj(&updateBuilder);
    batch->append(updateBuilder.obj());
}

void DeleteWriteOperation::appendSelfToBSONObj(BSONObjBuilder* obj) const {
    obj->append(kSelectorKey, _selector);
    obj->append(kLimitKey, _flags & RemoveOption_JustOne);
}

}  // namespace mongo
