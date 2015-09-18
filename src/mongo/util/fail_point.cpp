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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kControl

#include "mongo/util/fail_point.h"

#include <boost/thread/locks.hpp>

#include "mongo/util/mongoutils/str.h"
#include "mongo/util/log.h"
#include "mongo/util/time_support.h"

using mongoutils::str::stream;

namespace mongo {

using std::endl;

FailPoint::FailPoint() : _fpInfo(0), _mode(off), _timesOrPeriod(0), _modMutex() {}

void FailPoint::shouldFailCloseBlock() {
    _fpInfo.subtractAndFetch(1);
}

void FailPoint::setMode(Mode mode, ValType val, const BSONObj& extra) {
    /**
     * Outline:
     *
     * 1. Deactivates fail point to enter write-only mode
     * 2. Waits for all current readers of the fail point to finish
     * 3. Sets the new mode.
     */

    boost::lock_guard<boost::mutex> scoped(_modMutex);

    // Step 1
    disableFailPoint();

    // Step 2
    while (_fpInfo.load() != 0) {
        sleepmillis(50);
    }

    // Step 3
    uassert(16442,
            stream() << "mode not supported " << static_cast<int>(mode),
            mode >= off && mode < numModes);

    _mode = mode;
    _timesOrPeriod.store(val);

    _data = extra.copy();

    if (_mode != off) {
        enableFailPoint();
    }
}

const BSONObj& FailPoint::getData() const {
    return _data;
}

void FailPoint::enableFailPoint() {
    // TODO: Better to replace with a bitwise OR, once available for AU32
    ValType currentVal = _fpInfo.load();
    ValType expectedCurrentVal;
    ValType newVal;

    do {
        expectedCurrentVal = currentVal;
        newVal = expectedCurrentVal | ACTIVE_BIT;
        currentVal = _fpInfo.compareAndSwap(expectedCurrentVal, newVal);
    } while (expectedCurrentVal != currentVal);
}

void FailPoint::disableFailPoint() {
    // TODO: Better to replace with a bitwise AND, once available for AU32
    ValType currentVal = _fpInfo.load();
    ValType expectedCurrentVal;
    ValType newVal;

    do {
        expectedCurrentVal = currentVal;
        newVal = expectedCurrentVal & REF_COUNTER_MASK;
        currentVal = _fpInfo.compareAndSwap(expectedCurrentVal, newVal);
    } while (expectedCurrentVal != currentVal);
}

FailPoint::RetCode FailPoint::slowShouldFailOpenBlock() {
    ValType localFpInfo = _fpInfo.addAndFetch(1);

    if ((localFpInfo & ACTIVE_BIT) == 0) {
        return slowOff;
    }

    switch (_mode) {
        case alwaysOn:
            return slowOn;

        case random:
            // TODO: randomly determine if should be active or not
            error() << "FailPoint Mode random is not yet supported." << endl;
            fassertFailed(16443);

        case nTimes: {
            AtomicInt32::WordType newVal = _timesOrPeriod.subtractAndFetch(1);

            if (newVal <= 0) {
                disableFailPoint();
            }

            return slowOn;
        }

        default:
            error() << "FailPoint Mode not supported: " << static_cast<int>(_mode) << endl;
            fassertFailed(16444);
    }
}

BSONObj FailPoint::toBSON() const {
    BSONObjBuilder builder;

    boost::lock_guard<boost::mutex> scoped(_modMutex);
    builder.append("mode", _mode);
    builder.append("data", _data);

    return builder.obj();
}

ScopedFailPoint::ScopedFailPoint(FailPoint* failPoint)
    : _failPoint(failPoint), _once(false), _shouldClose(false) {}

ScopedFailPoint::~ScopedFailPoint() {
    if (_shouldClose) {
        _failPoint->shouldFailCloseBlock();
    }
}

const BSONObj& ScopedFailPoint::getData() const {
    // Assert when attempting to get data without incrementing ref counter.
    fassert(16445, _shouldClose);
    return _failPoint->getData();
}
}
