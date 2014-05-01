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

#include "mongo/client/write_concern.h"

namespace mongo {

    const char WriteConcern::kMajority[] = "majority";

    const WriteConcern WriteConcern::unacknowledged = WriteConcern().nodes(0);
    const WriteConcern WriteConcern::acknowledged = WriteConcern();
    const WriteConcern WriteConcern::journaled = WriteConcern().journal(true);
    const WriteConcern WriteConcern::replicated = WriteConcern().nodes(2);
    const WriteConcern WriteConcern::majority = WriteConcern().nodes(kMajority);

    WriteConcern::WriteConcern()
    : _enabled()
    , _w(1)
    , _w_str()
    , _j(false)
    , _fsync(false)
    , _timeout(0) {}

    int WriteConcern::nodes() const {
        return _w;
    }

    const std::string& WriteConcern::nodes_str() const {
        return _w_str;
    }

    bool WriteConcern::journal() const {
        return _j;
    }

    bool WriteConcern::fsync() const {
        return _fsync;
    }

    int WriteConcern::timeout() const {
        return _timeout;
    }

    WriteConcern& WriteConcern::nodes(int w) {
        _w = w;
        _enabled.set(kW);
        _enabled.reset(kWStr);
        return *this;
    }

    WriteConcern& WriteConcern::nodes(const StringData& w) {
        _w_str = w.toString();
        _enabled.set(kWStr);
        _enabled.reset(kW);
        return *this;
    }

    WriteConcern& WriteConcern::journal(bool j) {
        _j = j;
        _enabled.set(kJ);
        return *this;
    }

    WriteConcern& WriteConcern::fsync(bool fsync) {
        _fsync = fsync;
        _enabled.set(kFsync);
        return *this;
    }

    WriteConcern& WriteConcern::timeout(int timeout) {
        _timeout = timeout;
        _enabled.set(kTimeout);
        return *this;
    }

    /**
     * The only time we don't require confirmation is when nodes is explicitly set to 0.
     * See DRIVERS-131 for more information: https://jira.mongodb.org/browse/DRIVERS-131
     */
    bool WriteConcern::requiresConfirmation() const {
        return !(_enabled.test(kW) && _w == 0);
    }

    bool WriteConcern::hasNodeStr() const {
        return _enabled.test(kWStr);
    }

    /**
     * Only append options if they have explicitly been set to avoid overwriting server
     * side defaults.
     * See DRIVERS-131 for more information: https://jira.mongodb.org/browse/DRIVERS-131
     */
    BSONObj WriteConcern::toBson() const {
        BSONObjBuilder write_concern;
        write_concern.append("getlasterror", true);

        if (_enabled.test(kW))
            write_concern.append("w", _w);
        if (_enabled.test(kWStr))
            write_concern.append("w", _w_str);
        if (_enabled.test(kJ))
            write_concern.append("j", _j);
        if (_enabled.test(kFsync))
            write_concern.append("fsync", _fsync);
        if (_enabled.test(kTimeout))
            write_concern.append("wtimeout", _timeout);

        return write_concern.obj();
    }

    std::string WriteConcern::toString() const {
        return toBson().toString();
    }

} // namespace mongo
