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
#include "mongo/util/assert_util.h"

namespace mongo {

const char WriteConcern::kMajority[] = "majority";

const WriteConcern WriteConcern::unacknowledged = WriteConcern().nodes(0);
const WriteConcern WriteConcern::acknowledged = WriteConcern();
const WriteConcern WriteConcern::journaled = WriteConcern().journal(true);
const WriteConcern WriteConcern::replicated = WriteConcern().nodes(2);
const WriteConcern WriteConcern::majority = WriteConcern().mode(kMajority);

/**
 * The default constructor sets _w to 1 and the _enabled bit for _w to zero.
 *
 * The enabled bit must be unset by default because we should not send the w
 * parameter to the server unless it has been explicity set.
 *
 * If the _w bit were enabled by default it would be serialized with the gle
 * message and overwrite the server side default write concern.
 *
 * The private member _w is set to 1 because the default (simply sending GLE)
 * is logically equivalent to requiring confirmation from a single node. Upon
 * being set by the user via nodes() or mode() it overrides the default value
 * and is sent to the server.
 *
 * See DRIVERS-131 for more information:
 * https://jira.mongodb.org/browse/drivers-131
 */
WriteConcern::WriteConcern() : _enabled(), _w(1), _w_str(), _j(false), _fsync(false), _timeout(0) {}

int32_t WriteConcern::nodes() const {
    invariant(_enabled.test(kW) || (!_enabled.test(kWStr)));
    return _w;
}

const std::string& WriteConcern::mode() const {
    invariant(_enabled.test(kWStr));
    return _w_str;
}

bool WriteConcern::journal() const {
    return _j;
}

bool WriteConcern::fsync() const {
    return _fsync;
}

int32_t WriteConcern::timeout() const {
    return _timeout;
}

WriteConcern& WriteConcern::nodes(int w) {
    _w = w;
    _enabled.set(kW);
    _enabled.reset(kWStr);
    return *this;
}

WriteConcern& WriteConcern::mode(const StringData& w) {
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
 * The only time we don't require confirmation is when w is explicitly set to 0.
 * See DRIVERS-131 for more information: https://jira.mongodb.org/browse/DRIVERS-131
 */
bool WriteConcern::requiresConfirmation() const {
    return !_enabled.test(kW) || _w != 0;
}

bool WriteConcern::hasMode() const {
    return _enabled.test(kWStr);
}

/**
 * Only append options if they have explicitly been set to avoid overwriting server
 * side defaults.
 * See DRIVERS-131 for more information: https://jira.mongodb.org/browse/DRIVERS-131
 */
BSONObj WriteConcern::obj() const {
    BSONObjBuilder write_concern;

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

}  // namespace mongo
