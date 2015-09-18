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

#include "mongo/platform/endian.h"

#include <cstring>

#include "mongo/unittest/unittest.h"

namespace mongo {

using namespace endian;

TEST(EndianTest, TestSlow16) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    uint16_t le;
    uint16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

    ASSERT_EQUALS(be, endian::bswap_slow16(le));
    ASSERT_EQUALS(le, endian::bswap_slow16(be));
}

TEST(EndianTest, TestSlow32) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    uint32_t le;
    uint32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

    ASSERT_EQUALS(be, endian::bswap_slow32(le));
    ASSERT_EQUALS(le, endian::bswap_slow32(be));
}

TEST(EndianTest, TestSlow64) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint64_t le;
    uint64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

    ASSERT_EQUALS(be, endian::bswap_slow64(le));
    ASSERT_EQUALS(le, endian::bswap_slow64(be));
}

TEST(EndianTest, NativeToBig_uint16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    uint16_t le;
    uint16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_uint32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    uint32_t le;
    uint32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_uint64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint64_t le;
    uint64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_int16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    int16_t le;
    int16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_int32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    int32_t le;
    int32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_int64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    int64_t le;
    int64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_float) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    float le;
    float be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToBig_double) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    double le;
    double be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(be, nativeToBig(le));
#else
    ASSERT_EQUALS(be, nativeToBig(be));
#endif
}

TEST(EndianTest, NativeToLittle_uint16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    uint16_t le;
    uint16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_uint32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    uint32_t le;
    uint32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_uint64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint64_t le;
    uint64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_int16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    int16_t le;
    int16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_int32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    int32_t le;
    int32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_int64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    int64_t le;
    int64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_float) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    float le;
    float be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, NativeToLittle_double) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    double le;
    double be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, nativeToLittle(le));
#else
    ASSERT_EQUALS(le, nativeToLittle(be));
#endif
}

TEST(EndianTest, LittleToNative_uint16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    uint16_t le;
    uint16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_uint32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    uint32_t le;
    uint32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_uint64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint64_t le;
    uint64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_int16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    int16_t le;
    int16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_int32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    int32_t le;
    int32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_int64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    int64_t le;
    int64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_float) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    float le;
    float be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, LittleToNative_double) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    double le;
    double be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, littleToNative(le));
#else
    ASSERT_EQUALS(be, littleToNative(le));
#endif
}

TEST(EndianTest, BigToNative_uint16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    uint16_t le;
    uint16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_uint32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    uint32_t le;
    uint32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_uint64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    uint64_t le;
    uint64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_int16_t) {
    uint8_t le_buf[] = {0x01, 0x02};
    uint8_t be_buf[] = {0x02, 0x01};
    int16_t le;
    int16_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_int32_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    int32_t le;
    int32_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_int64_t) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    int64_t le;
    int64_t be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_float) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t be_buf[] = {0x04, 0x03, 0x02, 0x01};
    float le;
    float be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

TEST(EndianTest, BigToNative_double) {
    uint8_t le_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t be_buf[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    double le;
    double be;
    std::memcpy(&le, le_buf, sizeof(le));
    std::memcpy(&be, be_buf, sizeof(be));

#if MONGO_BYTE_ORDER == 1234
    ASSERT_EQUALS(le, bigToNative(be));
#else
    ASSERT_EQUALS(be, bigToNative(be));
#endif
}

}  // namespace mongo
