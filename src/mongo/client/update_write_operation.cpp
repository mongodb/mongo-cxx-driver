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

#include "mongo/client/update_write_operation.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/db/namespace_string.h"

namespace mongo {

    namespace {
        const char kCommandKey[] = "update";
        const char kBatchName[] = "updates";
        const char kSelectorKey[] = "q";
        const char kUpdateKey[] = "u";
        const char kMultiKey[] = "multi";
        const char kUpsertKey[] = "upsert";
    } // namespace

    UpdateWriteOperation::UpdateWriteOperation(const BSONObj& selector, const BSONObj& update, int flags)
        : _selector(selector)
        , _update(update)
        , _flags(flags)
        {}

    Operations UpdateWriteOperation::operationType() const {
        return dbUpdate;
    }

    const char* UpdateWriteOperation::batchName() const {
        return kBatchName;
    }

    int UpdateWriteOperation::incrementalSize() const {
        return _selector.objsize() + _update.objsize();
    }

    void UpdateWriteOperation::startRequest(const std::string& ns, bool, BufBuilder* builder) const {
        builder->appendNum(0);
        builder->appendStr(ns);
        builder->appendNum(_flags);
    }

    void UpdateWriteOperation::appendSelfToRequest(BufBuilder* builder) const {
        _selector.appendSelfToBufBuilder(*builder);
        _update.appendSelfToBufBuilder(*builder);
    }

    void UpdateWriteOperation::startCommand(const std::string& ns, BSONObjBuilder* command) const {
        command->append(kCommandKey, nsToCollectionSubstring(ns));
    }

    void UpdateWriteOperation::appendSelfToCommand(BSONArrayBuilder* batch) const {
        BSONObjBuilder updateBuilder;
        updateBuilder.append(kSelectorKey, _selector);
        updateBuilder.append(kUpdateKey, _update);
        updateBuilder.append(kMultiKey, bool(_flags & UpdateOption_Multi));
        updateBuilder.append(kUpsertKey, bool(_flags & UpdateOption_Upsert));
        batch->append(updateBuilder.obj());
    }

} // namespace mongo
