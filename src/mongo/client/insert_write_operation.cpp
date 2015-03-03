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

#include "mongo/client/insert_write_operation.h"

#include "mongo/bson/bsontypes.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/db/namespace_string.h"


namespace mongo {

    namespace {
        const char kCommandKey[] = "insert";
        const char kBatchName[] = "documents";
    } // namespace

    InsertWriteOperation::InsertWriteOperation(const BSONObj& doc)
        : _doc(_ensureId(doc))
    {}

    WriteOpType InsertWriteOperation::operationType() const {
        return dbWriteInsert;
    }

    const char* InsertWriteOperation::batchName() const {
        return kBatchName;
    }

    int InsertWriteOperation::incrementalSize() const {
        return _doc.objsize();
    }

    void InsertWriteOperation::startRequest(const std::string& ns, bool ordered, BufBuilder* builder) const {
        builder->appendNum(ordered ? 0 : 1);
        builder->appendStr(ns);
    }

    void InsertWriteOperation::appendSelfToRequest(BufBuilder* builder) const {
        _doc.appendSelfToBufBuilder(*builder);
    }

    void InsertWriteOperation::startCommand(const std::string& ns, BSONObjBuilder* command) const {
        command->append(kCommandKey, nsToCollectionSubstring(ns));
    }

    void InsertWriteOperation::appendSelfToCommand(BSONArrayBuilder* batch) const {
        batch->append(_doc);
    }

    void InsertWriteOperation::appendSelfToBSONObj(BSONObjBuilder* obj) const {
        obj->appendElements(_doc);
    }

    BSONObj InsertWriteOperation::_ensureId(const BSONObj& doc) {
        BSONElement id = doc.getField("_id");
        if (!id.eoo()) {
            uassert(0, "value of _id element cannot contain any fields starting with $", !id.isABSONObj() || id.Obj().okForStorage());
            return doc;
        }

        BSONObjBuilder bob;
        bob.append("_id", OID::gen());
        bob.appendElements(doc);
        return bob.obj();
    }

} // namespace mongo
