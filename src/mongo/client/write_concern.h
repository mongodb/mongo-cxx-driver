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

#include <bitset>
#include <string>

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
    /** Default write concern: equivalent to acknowledged */
    WriteConcern();

    /**
     * Using nodes(kMajority) confirms that write operations have propagated
     * to the majority of a replica set. This allows you to avoid hard coding
     * assumptions about the size of your replica set into your application.
     */
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
     * to be considered successful.
     *
     * If set this becomes the "w" parameter when sent to the server.
     *
     * @return number of nodes required
     */
    int32_t nodes() const;

    /**
     * Returns a string representing the write concern mode.
     *
     * If set this becomes the "w" parameter when sent to the server.
     *
     * @return nodes required as std::string
     */
    const std::string& mode() const;

    /**
     * If write will only be considered successful when committed to journal.
     *
     * @return true if write will block on MongoDB journal
     */
    bool journal() const;

    /**
     * If fsync is set and the server is running without journaling,
     * the write will only be considered successful when the server has synced
     * all data files to disk. If the server is running with journaling, this acts
     * the same as if journal() was set. Cannot be used in combination with journal(true).
     *
     * @return true if the fsync option is set on the write.
     */
    bool fsync() const;

    /**
     * Length of time to block waiting for nodes, journal, or fsync.
     *
     * @return int representing milliseconds to wait for write
     */
    int32_t timeout() const;

    /** Sets the number of nodes required for write to be successful. */
    WriteConcern& nodes(int w);

    /** Sets the type of nodes required for write to be successful. */
    WriteConcern& mode(const StringData& w);

    /** Sets whether journal is required for write to be successful. */
    WriteConcern& journal(bool j);

    /**
     * Sets the value of the fsync parameter.
     *
     * @note The behavior of this option is dependent on server configuration.
     * @see The comment on fsync() for details.
     */
    WriteConcern& fsync(bool fsync);

    /** Sets timeout to wait for write to be successful. */
    WriteConcern& timeout(int timeout);

    /** Whether we need to send getLastError for this WriteConcern */
    bool requiresConfirmation() const;

    /** Whether the write concern currently reflects a mode */
    bool hasMode() const;

    /** Turn write concern into an object for inclusion in GetLastError or write command */
    BSONObj obj() const;

private:
    // Enabled option book keeping
    static const size_t kNumOptions = 5;
    enum Options { kW, kWStr, kJ, kFsync, kTimeout };
    std::bitset<kNumOptions> _enabled;

    // Actual option values
    int32_t _w;
    std::string _w_str;
    bool _j;
    bool _fsync;
    int32_t _timeout;
};

}  // namespace mongo
