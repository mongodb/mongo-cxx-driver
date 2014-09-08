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

#include "mongo/base/data_view.h"
#include "mongo/platform/cstdint.h"

namespace mongo {

    class OpTime {
        static const std::size_t kIncrementOffset = 0;
        static const std::size_t kTimeOffset = 4;
    public:
        OpTime(uint32_t timestamp, uint32_t increment) {
            setTimestamp(timestamp);
            setIncrement(increment);
        }

        // Caller must ensure that the buffer is valid for at least 8 bytes past the start
        explicit OpTime(const char *src) {
            std::memcpy(_buf, src, sizeof(_buf));
        }

        OpTime() : _buf() {}

        void setTimestamp(uint32_t v) { DataView(_buf).writeLE(v, kTimeOffset); }
        void setIncrement(uint32_t v) { DataView(_buf).writeLE(v, kIncrementOffset); }

        uint32_t getTimestamp() const { return ConstDataView(_buf).readLE<uint32_t>(kTimeOffset); }
        uint32_t getIncrement() const { return ConstDataView(_buf).readLE<uint32_t>(kIncrementOffset); }

        bool isNull() const { return getTimestamp() == 0u; }

        static OpTime max();
    private:
        char _buf[8];
    };

}  // namespace mongo
