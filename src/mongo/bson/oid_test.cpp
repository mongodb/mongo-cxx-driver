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

#include "mongo/bson/oid.h"

#include "mongo/platform/endian.h"
#include "mongo/unittest/unittest.h"

namespace {

    using mongo::OID;

    TEST(Equals, Simple) {
        OID o1 = OID::gen();

        ASSERT_EQUALS(o1, o1);
        ASSERT_TRUE(o1 == o1);
        ASSERT_EQUALS(o1.compare(o1), 0);
    }

    TEST(NotEquals, Simple) {
        OID o1 = OID::gen();
        OID o2 = OID::gen();

        ASSERT_FALSE(o1 == o2);
        ASSERT_TRUE(o1 != o2);
        ASSERT_NOT_EQUALS(o1.compare(o2), 0);
    }

    TEST(Increasing, Simple) {
        OID o1 = OID::gen();
        OID o2 = OID::gen();
        ASSERT_TRUE(o1 < o2);
    }

    TEST(IsSet, Simple) {
        OID o;
        ASSERT_FALSE(o.isSet());
        o.init();
        ASSERT_TRUE(o.isSet());
    }

    TEST(JustForked, Simple) {
        OID o1 = OID::gen();
        OID::justForked();
        OID o2 = OID::gen();

        ASSERT_TRUE(std::memcmp(o1.getInstanceUnique().bytes, o2.getInstanceUnique().bytes,
                                OID::kInstanceUniqueSize) != 0);
    }

    TEST(TimestampIsBigEndian, Endianness) {
        OID o1;  // zeroed
        OID::Timestamp ts = 123;
        o1.setTimestamp(ts);

        int32_t ts_big = mongo::endian::nativeToBig<int32_t>(123);

        const char* oidBytes = o1.view().view();
        ASSERT(std::memcmp(&ts_big, oidBytes, sizeof(int32_t)) == 0);
    }

    TEST(IncrementIsBigEndian, Endianness) {
        OID o1; // zeroed
        OID::Increment incr;
        // Increment is a 3 byte counter big endian
        incr.bytes[0] = 0xBEu;
        incr.bytes[1] = 0xADu;
        incr.bytes[2] = 0xDEu;

        o1.setIncrement(incr);

        const char* oidBytes = o1.view().view();
        oidBytes += OID::kTimestampSize + OID::kInstanceUniqueSize;

        // now at start of increment
        ASSERT_EQUALS(uint8_t(oidBytes[0]), 0xBEu);
        ASSERT_EQUALS(uint8_t(oidBytes[1]), 0xADu);
        ASSERT_EQUALS(uint8_t(oidBytes[2]), 0xDEu);
    }

    TEST(Basic, Deserialize) {

        uint8_t OIDbytes[] = {
            0xDEu, 0xADu, 0xBEu, 0xEFu,        // timestamp is -559038737 (signed)
            0x00u, 0x00u, 0x00u, 0x00u, 0x00u, // unique is 0
            0x11u, 0x22u, 0x33u                // increment is 1122867
        };

        OID o1 = OID::from(OIDbytes);

        ASSERT_EQUALS(o1.getTimestamp(), -559038737);
        OID::InstanceUnique u = o1.getInstanceUnique();
        for (std::size_t i = 0; i < OID::kInstanceUniqueSize; ++i) {
            ASSERT_EQUALS(u.bytes[i], 0x00u);
        }
        OID::Increment i = o1.getIncrement();

        // construct a uint32_t from increment
        // recall that i is a big-endian 3 byte unsigned integer
        uint32_t incr =
            ((uint32_t(i.bytes[0]) << 16)) |
            ((uint32_t(i.bytes[1]) << 8))  |
              uint32_t(i.bytes[2]);

        ASSERT_EQUALS(1122867u, incr);
    }

    TEST(Basic, FromString) {

        std::string oidStr("541b1a00e8a23afa832b218e");
        uint8_t oidBytes[] = {0x54u, 0x1Bu, 0x1Au, 0x00u,
                            0xE8u, 0xA2u, 0x3Au, 0xFAu,
                            0x83u, 0x2Bu, 0x21u, 0x8Eu};

        ASSERT_EQUALS(OID(oidStr), OID::from(oidBytes));
    }

    TEST(Basic, FromStringToString) {
        std::string fromStr("541b1a00e8a23afa832b218e");
        ASSERT_EQUALS(OID(fromStr).toString(), fromStr);
    }
}
