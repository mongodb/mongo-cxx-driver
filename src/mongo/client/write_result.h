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

#include "mongo/client/export_macros.h"
#include "mongo/util/net/operation.h"

namespace mongo {

class BSONObj;
class BSONElement;
class StringData;
class WriteOperation;

/**
 * Class representing the result of a write operations sent to the server.
 */
class MONGO_CLIENT_API WriteResult {
    friend class WireProtocolWriter;
    friend class CommandWriter;
    friend class BulkOperationBuilder;
    friend class DBClientBase;

public:
    /**
     * Creates an empty write result.
     */
    WriteResult();

    //
    // Introspection
    //

    /**
     * Returns true if there have been write errors of any kind.
     *
     * This includes write errors and write concern errors.
     */
    bool hasErrors() const;

    /**
     * Returns true if there have been write errors.
     *
     * These include duplicate key errors, etc... that are possibly encountered in the process
     * of write execution on the server.
     */
    bool hasWriteErrors() const;

    /**
     * Returns true if there have been write concern errors.
     *
     * This class of error can occur after the execution of writes on the server if they
     * cannot be fully persisted or replicated to fulfill given supplied level of write concern.
     */
    bool hasWriteConcernErrors() const;

    /**
     * Returns true if the number of modified documents is available.
     *
     * This will not be true if any write response comes from a node running MongoDB <2.6.
     *
     * Note: You can only call nModified() if hasModifiedCount() is true.
     */
    bool hasModifiedCount() const;


    //
    // Data
    //

    /**
     * The number of documents that were inserted.
     *
     * Note: This field is always available.
     */
    int nInserted() const;

    /**
     * The number of documents that were upserted.
     *
     * Note: This field is always available.
     */
    int nUpserted() const;

    /**
     * The number of documents that were matched.
     *
     * Note: This field is always available.
     */
    int nMatched() const;

    /**
     * The number of documents that were modified.
     *
     * Warning: This field is only available if hasModifiedCount() is true.
     */
    int nModified() const;

    /**
     * The number of documents that were removed.
     *
     * Note: This field is always available.
     */
    int nRemoved() const;

    /**
     * The information about documents that were upserted.
     *
     * Note: The objects in the vector have an "index" and "_id" field.
     */
    const std::vector<BSONObj>& upserted() const;


    //
    // Errors Data
    //

    /**
     * Vector with the write errors that occurred.
     */
    const std::vector<BSONObj>& writeErrors() const;

    /**
     * Vector with the write concern errors that occurred.
     */
    const std::vector<BSONObj>& writeConcernErrors() const;

private:
    void _mergeWriteConcern(const BSONObj& result);
    void _mergeCommandResult(const std::vector<WriteOperation*>& ops, const BSONObj& result);
    void _mergeGleResult(const std::vector<WriteOperation*>& ops, const BSONObj& result);

    void _check(bool throwSoftErrors);
    void _setModified(const BSONObj& result);
    int _getIntOrDefault(const BSONObj& obj, const StringData& field, const int defaultValue = 0);

    int _createUpserts(const BSONElement& upsert, const std::vector<WriteOperation*>& ops);
    void _createUpsert(const BSONElement& upsert, const std::vector<WriteOperation*>& ops);
    void _createWriteError(const BSONObj& error, const std::vector<WriteOperation*>& ops);
    void _createWriteConcernError(const BSONObj& error);

    int _nInserted;
    int _nUpserted;
    int _nMatched;
    int _nModified;
    int _nRemoved;

    std::vector<BSONObj> _upserted;
    std::vector<BSONObj> _writeErrors;
    std::vector<BSONObj> _writeConcernErrors;

    bool _hasModifiedCount;
    bool _requiresDetailedInsertResults;
};

}  // namespace mongo
