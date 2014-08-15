/**
 *    Copyright (C) 2014 MongoDB Inc.
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

#include "mongo/base/data_cursor.h"

#include "mongo/platform/endian.h"
#include "mongo/unittest/unittest.h"

namespace mongo {

    TEST(DataCursor, ConstDataCursor) {
        char buf[100];

        DataView(buf).writeNative<uint16_t>(1);
        DataView(buf).writeLE<uint32_t>(2, sizeof(uint16_t));
        DataView(buf).writeBE<uint64_t>(3, sizeof(uint16_t) + sizeof(uint32_t));

        ConstDataCursor cdc(buf);

        ASSERT_EQUALS(static_cast<uint16_t>(1), cdc.readNativeAndAdvance<uint16_t>());
        ASSERT_EQUALS(static_cast<uint32_t>(2), cdc.readLEAndAdvance<uint32_t>());
        ASSERT_EQUALS(static_cast<uint64_t>(3), cdc.readBEAndAdvance<uint64_t>());

        // test skip()
        cdc = buf;
        cdc.skip<uint32_t>();
        ASSERT_EQUALS(buf + sizeof(uint32_t), cdc.view());

        // test x +
        cdc = buf;
        ASSERT_EQUALS(buf + sizeof(uint32_t), (cdc + sizeof(uint32_t)).view());

        // test x -
        cdc = buf + sizeof(uint32_t);
        ASSERT_EQUALS(buf, (cdc - sizeof(uint32_t)).view());

        // test x += and x -=
        cdc = buf;
        cdc += sizeof(uint32_t);
        ASSERT_EQUALS(buf + sizeof(uint32_t), cdc.view());
        cdc -= sizeof(uint16_t);
        ASSERT_EQUALS(buf + sizeof(uint16_t), cdc.view());

        // test ++x
        cdc = buf;
        ASSERT_EQUALS(buf + sizeof(uint8_t), (++cdc).view());
        ASSERT_EQUALS(buf + sizeof(uint8_t), cdc.view());

        // test x++
        cdc = buf;
        ASSERT_EQUALS(buf, (cdc++).view());
        ASSERT_EQUALS(buf + sizeof(uint8_t), cdc.view());

        // test --x
        cdc = buf + sizeof(uint8_t);
        ASSERT_EQUALS(buf, (--cdc).view());
        ASSERT_EQUALS(buf, cdc.view());

        // test x--
        cdc = buf + sizeof(uint8_t);
        ASSERT_EQUALS(buf + sizeof(uint8_t), (cdc--).view());
        ASSERT_EQUALS(buf, cdc.view());

    }

    TEST(DataCursor, DataCursor) {
        char buf[100];

        DataCursor dc(buf);

        dc.writeNativeAndAdvance<uint16_t>(1);
        dc.writeLEAndAdvance<uint32_t>(2);
        dc.writeBEAndAdvance<uint64_t>(3);

        ConstDataCursor cdc(buf);

        ASSERT_EQUALS(static_cast<uint16_t>(1), cdc.readNativeAndAdvance<uint16_t>());
        ASSERT_EQUALS(static_cast<uint32_t>(2), cdc.readLEAndAdvance<uint32_t>());
        ASSERT_EQUALS(static_cast<uint64_t>(3), cdc.readBEAndAdvance<uint64_t>());
    }

} // namespace mongo
