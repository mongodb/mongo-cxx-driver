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

#include "mongo/client/write_result.h"

#include "mongo/client/exceptions.h"
#include "mongo/client/write_operation.h"
#include "mongo/db/jsobj.h"

namespace mongo {

    // For backwards compatibility. See MongoDB src/mongo/base/error_codes.err
    namespace {
        const int kUnknownError = 8;
        const int kWriteConcernErrorCode = 64;
    } // namespace

    WriteResult::WriteResult()
        : _nInserted(0)
        , _nUpserted(0)
        , _nMatched(0)
        , _nModified(0)
        , _nRemoved(0)
        , _hasModifiedCount(true)
        , _requiresDetailedInsertResults(false)
    {}

    bool WriteResult::hasErrors() const {
        return hasWriteErrors() || hasWriteConcernErrors();
    }

    bool WriteResult::hasWriteErrors() const {
        return !_writeErrors.empty();
    }

    bool WriteResult::hasWriteConcernErrors() const {
        return !_writeConcernErrors.empty();
    }

    bool WriteResult::hasModifiedCount() const {
        return _hasModifiedCount;
    }

    int WriteResult::nInserted() const {
        return _nInserted;
    }

    int WriteResult::nUpserted() const {
        return _nUpserted;
    }

    int WriteResult::nMatched() const {
        return _nMatched;
    }

    int WriteResult::nModified() const {
        uassert(0, "this result does not have a modified count", _hasModifiedCount);
        return _nModified;
    }

    int WriteResult::nRemoved() const {
        return _nRemoved;
    }

    const std::vector<BSONObj>& WriteResult::upserted() const {
        return _upserted;
    }

    const std::vector<BSONObj>& WriteResult::writeErrors() const {
        return _writeErrors;
    }

    const std::vector<BSONObj>& WriteResult::writeConcernErrors() const {
        return _writeConcernErrors;
    }

    void WriteResult::_mergeCommandResult(
        const std::vector<WriteOperation*>& ops,
        const BSONObj& result
    ) {
        int affected = _getIntOrDefault(result, "n");

        // Handle Write Batch
        switch (ops.front()->operationType()) {
            case dbWriteInsert:
                _nInserted += affected;
                break;

            case dbWriteDelete:
                _nRemoved += affected;
                break;

            case dbWriteUpdate:
                if (result.hasField("upserted")) {
                    int nUpserted = _createUpserts(result.getField("upserted"), ops);
                    _nUpserted += nUpserted;
                    _nMatched += (affected - nUpserted);
                } else {
                    _nMatched += affected;
                }

                _setModified(result);

                break;
        }

        // Handle Write Errors
        if (result.hasField("writeErrors")) {
            BSONElement writeErrors = result.getField("writeErrors");
            BSONObjIterator arrayIterator(writeErrors.Obj());
            BSONElement current;

            while (arrayIterator.more())
                _createWriteError(arrayIterator.next().Obj(), ops);
        }

        // Handle Write Concern Errors
        if (result.hasField("writeConcernError")) {
            BSONObj writeConcernError = result.getObjectField("writeConcernError");
            _createWriteConcernError(writeConcernError);
        }
    }

    void WriteResult::_mergeGleResult(
        const std::vector<WriteOperation*>& ops,
        const BSONObj& result
    ) {
        int affected = _getIntOrDefault(result, "n");

        // Handle Errors
        std::string error;

        if (result.hasField("errmsg"))
            error = result.getStringField("errmsg");
        else if (result.hasField("err"))
            error = result.getStringField("err");

        if (!error.empty()) {
            // We have errors
            result.hasField("wtimeout")
                ? _createWriteConcernError(result)
                : _createWriteError(result, ops);

            // Don't update the counts for this gle merge as we have encountered an error
            return;
        }

        // Handle Write Batch
        switch (ops.front()->operationType()) {
            case dbWriteInsert:
                _nInserted += 1;
                break;

            case dbWriteDelete:
                _nRemoved += affected;
                break;

            case dbWriteUpdate:
                if (result.hasField("upserted")) {
                    _createUpsert(result.getField("upserted"), ops);
                    _nUpserted += affected;

                // DRIVERS-151 -- handle <2.6 servers when upserted _id not returned
                //
                // Versions of MongoDB before 2.6 don't return the _id for an upsert
                // if _id is not an ObjectID. We need to make sure the counts are ok
                // and extract the _id from the original update operation (the _id in
                // the update document trumps the one in the query spec if both exist).
                } else if (result.hasField("updatedExisting") &&
                           !result.getBoolField("updatedExisting") && affected == 1) {

                    // Get the update information out of the operation
                    BSONObjBuilder bob;
                    ops.front()->appendSelfToBSONObj(&bob);
                    BSONObj updateOp = bob.obj();

                    // Extract the _id -- update doc version wins over selector doc
                    BSONElement id = updateOp.getFieldDotted("u._id");
                    if (id.eoo())
                        id = updateOp.getFieldDotted("q._id");

                    _createUpsert(id, ops);

                    _nUpserted += affected;
                } else {
                    _nMatched += affected;
                }

                _setModified(result);

                break;
        }
    }

    void WriteResult::_check(bool throwSoftErrors) {
        if (hasWriteErrors())
            throw OperationException(writeErrors().back());
        if (throwSoftErrors && hasWriteConcernErrors()) {
            throw OperationException(writeConcernErrors().front());
        }
    }

    /**
     * SERVER-13001 - mixed sharded cluster could return nModified
     * (servers >= 2.6) or not (servers <= 2.4). If any call does
     * not return nModified we cannot report a valid final count.
     */
    void WriteResult::_setModified(const BSONObj& result) {
        int nModified = result.getIntField("nModified");

        if (_hasModifiedCount && nModified >= 0)
            _nModified += nModified;
        else
            _hasModifiedCount = false;
    }

    int WriteResult::_getIntOrDefault(const BSONObj& obj, const StringData& field, const int defaultValue) {
        return obj.hasField(field) ? obj.getIntField(field) : defaultValue;
    }

    int WriteResult::_createUpserts(const BSONElement& upserted, const std::vector<WriteOperation*>& ops) {
        int nUpserted = 0;

        BSONObjIterator arrayIterator(upserted.Obj());

        while (arrayIterator.more()) {
            _createUpsert(arrayIterator.next(), ops);
            nUpserted++;
        }

        return nUpserted;
    }

    void WriteResult::_createUpsert(const BSONElement& upsert, const std::vector<WriteOperation*>& ops) {
        int batchIndex = 0;
        BSONElement id;

        if (upsert.isABSONObj()) {
            // Upsert result came from MongoDB 2.6+
            batchIndex = _getIntOrDefault(upsert.Obj(), "index");
            id = upsert["_id"];
        } else {
            // Upsert result came from MongoDB <2.6
            id = upsert;
        }

        BSONObjBuilder bob;
        bob.append("index", static_cast<long long>(ops[batchIndex]->getBulkIndex()));
        bob.appendAs(id, "_id");

        _upserted.push_back(bob.obj());
    }

    void WriteResult::_createWriteError(const BSONObj& error, const std::vector<WriteOperation*>& ops) {
        int batchIndex = _getIntOrDefault(error, "index");
        int code = _getIntOrDefault(error, "code", kUnknownError);

        BSONObjBuilder bob;
        bob.append("index", static_cast<long long>(ops[batchIndex]->getBulkIndex()));
        bob.append("code", code);
        bob.append("errmsg", error.getStringField("errmsg"));

        BSONObjBuilder builder;
        ops[batchIndex]->appendSelfToBSONObj(&builder);
        bob.append("op", builder.obj());

        if (error.hasField("errInfo"))
            bob.append("details", error.getObjectField("errInfo"));

        _writeErrors.push_back(bob.obj());
    }

    void WriteResult::_createWriteConcernError(const BSONObj& error) {
        BSONObjBuilder bob;
        int code = _getIntOrDefault(error, "code", kWriteConcernErrorCode);

        bob.append("code", code);
        bob.append("errmsg", error.getStringField("errmsg"));

        if (error.hasField("errInfo"))
            bob.append("details", error.getObjectField("errInfo"));

        _writeConcernErrors.push_back(bob.obj());
    }

} // namespace mongo
