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

#include <string>
#include <bitset>

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"

namespace mongo {

    /**
     * Class to encapsulate client side "Write Concern" concept.
     *
     * Write concern describes the guarantee that MongoDB provides when reporting
     * on the success of a write operation.
     *
     * Example usage:
     *     WriteConcern wc = WriteConcern().nodes(3).journal(false);
     */
    class MONGO_CLIENT_API WriteConcern {
    public:
        /** Default write concern */
        WriteConcern();

        /** the character string "majority" */
        static const char kMajority[];

        //
        // Standard write concern levels as defined in the MongoDB manual:
        // http://docs.mongodb.org/manual/core/write-concern/
        //

        /** Fire and forget */
        static const WriteConcern unacknowledged;
        /** A single node acknowledges the write, equivalent to default constructor */
        static const WriteConcern acknowledged;
        /** A single node acknowledges the write operation was committed to journal */
        static const WriteConcern journaled;
        /** Two nodes have acknowledged receipt of the write operation */
        static const WriteConcern replicated;
        /** A majority of nodes acknowledges (replica set) */
        static const WriteConcern majority;

        /**
         * Returns an integer representing the number of nodes required for write
         * to be successful. Only makes sense if hasNodeStr() is false.
         *
         * If set this becomes the "w" parameter when sent to the server.
         *
         * @return number of nodes required
         */
        int nodes() const;

        /**
         * Returns a string representing the number of nodes required for write
         * to be successful. Only makes sense if hasNodeStr() is true.
         *
         * If set this becomes the "w" parameter when sent to the server.
         *
         * @return nodes required as std::string
         */
        const std::string& nodes_str() const;

        /**
         * If write will only be considered successfull when committed to journal.
         *
         * @return true if write will block on MongoDB journal
         */
        bool journal() const;

        /**
         * If write will only be considered successfull when committed to data files.
         *
         * @return true if write will block on MongoDB fsync
         */
        bool fsync() const;

        /**
         * Length of time to block waiting for nodes, journal, or fsync.
         *
         * @return int representing milliseconds to wait for write
         */
        int timeout() const;

        /** Sets the number of nodes required for write to be successful. */
        WriteConcern& nodes(int w);

        /** Sets the type of nodes required for write to be successful. */
        WriteConcern& nodes(const StringData& w);

        /** Sets whether journal is required for write to be successful. */
        WriteConcern& journal(bool j);

        /** Sets whether fsync is required for write to be successful. */
        WriteConcern& fsync(bool fsync);

        /** Sets timeout to wait for write to be successful. */
        WriteConcern& timeout(int timeout);

        /** Whether we need to send getLastError for this WriteConcern */
        bool requiresConfirmation() const;

        /** Whether nodes parameter is currently represented as a string */
        bool hasNodeStr() const;

        /** Turn write concern into getLastError BSONObj suitable for command */
        BSONObj toBson() const;

        /** Returns the string representation of BSONObj from toBson() */
        std::string toString() const;

    private:
        // Enabled option book keeping
        static const size_t kNumOptions = 5;
        enum Options { kW, kWStr, kJ, kFsync, kTimeout };
        std::bitset<kNumOptions> _enabled;

        // Actual option values
        int _w;
        std::string _w_str;
        bool _j;
        bool _fsync;
        int32_t _timeout;
    };

} // namespace mongo
