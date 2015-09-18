/*
 *    Copyright 2014 MongoDB Inc.
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

#include "mongo/platform/cstdint.h"

namespace mongo {

class Timestamp_t {
public:
    Timestamp_t() : _seconds(), _increment() {}

    Timestamp_t(const uint32_t seconds, const uint32_t increment)
        : _seconds(seconds), _increment(increment) {}

    uint32_t seconds() const {
        return _seconds;
    }
    uint32_t increment() const {
        return _increment;
    }

private:
    const uint32_t _seconds;
    const uint32_t _increment;
};

}  // namespace mongo
