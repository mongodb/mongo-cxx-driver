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

/**
 * tests for BSONObjBuilder
 */

#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"

#include <sstream>
#include "mongo/unittest/unittest.h"

namespace {

using std::string;
using std::stringstream;
using mongo::BSONElement;
using mongo::BSONObj;
using mongo::BSONObjBuilder;
using mongo::BSONType;

const long long maxEncodableInt = (1 << 30) - 1;
const long long minEncodableInt = -maxEncodableInt;

const long long maxInt = (std::numeric_limits<int>::max)();
const long long minInt = (std::numeric_limits<int>::min)();

const long long maxEncodableDouble = (1LL << 40) - 1;
const long long minEncodableDouble = -maxEncodableDouble;

const long long maxDouble = (1LL << std::numeric_limits<double>::digits) - 1;
const long long minDouble = -maxDouble;

const long long maxLongLong = (std::numeric_limits<long long>::max)();
const long long minLongLong = (std::numeric_limits<long long>::min)();

template <typename T>
void assertBSONTypeEquals(BSONType actual, BSONType expected, T value, int i) {
    if (expected != actual) {
        stringstream ss;
        ss << "incorrect type in bson object for " << (i + 1) << "-th test value " << value
           << ". actual: " << mongo::typeName(actual)
           << "; expected: " << mongo::typeName(expected);
        const string msg = ss.str();
        FAIL() << msg;
    }
}

/**
 * current conversion ranges in append(unsigned n)
 * dbl/int max/min in comments refer to max/min encodable constants
 *                  0 <= n <= uint_max          -----> int
 */

TEST(BSONObjBuilderTest, AppendUnsignedInt) {
    struct {
        unsigned int v;
        BSONType t;
    } data[] = {{0, mongo::NumberInt},
                {100, mongo::NumberInt},
                {maxEncodableInt, mongo::NumberInt},
                {maxEncodableInt + 1, mongo::NumberInt},
                {static_cast<unsigned int>(maxInt), mongo::NumberInt},
                {static_cast<unsigned int>(maxInt) + 1U, mongo::NumberInt},
                {(std::numeric_limits<unsigned int>::max)(), mongo::NumberInt},
                {0, mongo::Undefined}};
    for (int i = 0; data[i].t != mongo::Undefined; i++) {
        unsigned int v = data[i].v;
        BSONObjBuilder b;
        b.append("a", v);
        BSONObj o = b.obj();
        ASSERT_EQUALS(o.nFields(), 1);
        BSONElement e = o.getField("a");
        unsigned int n = e.numberLong();
        ASSERT_EQUALS(n, v);
        assertBSONTypeEquals(e.type(), data[i].t, v, i);
    }
}

/**
 * current conversion ranges in appendIntOrLL(long long n)
 * dbl/int max/min in comments refer to max/min encodable constants
 *                       n <  dbl_min            -----> long long
 *            dbl_min <= n <  int_min            -----> double
 *            int_min <= n <= int_max            -----> int
 *            int_max <  n <= dbl_max            -----> double
 *            dbl_max <  n                       -----> long long
 */

TEST(BSONObjBuilderTest, AppendIntOrLL) {
    struct {
        long long v;
        BSONType t;
    } data[] = {{0, mongo::NumberInt},
                {-100, mongo::NumberInt},
                {100, mongo::NumberInt},
                {-(maxInt / 2 - 1), mongo::NumberInt},
                {maxInt / 2 - 1, mongo::NumberInt},
                {-(maxInt / 2), mongo::NumberLong},
                {maxInt / 2, mongo::NumberLong},
                {minEncodableInt, mongo::NumberLong},
                {maxEncodableInt, mongo::NumberLong},
                {minEncodableInt - 1, mongo::NumberLong},
                {maxEncodableInt + 1, mongo::NumberLong},
                {minInt, mongo::NumberLong},
                {maxInt, mongo::NumberLong},
                {minInt - 1, mongo::NumberLong},
                {maxInt + 1, mongo::NumberLong},
                {minLongLong, mongo::NumberLong},
                {maxLongLong, mongo::NumberLong},
                {0, mongo::Undefined}};
    for (int i = 0; data[i].t != mongo::Undefined; i++) {
        long long v = data[i].v;
        BSONObjBuilder b;
        b.appendIntOrLL("a", v);
        BSONObj o = b.obj();
        ASSERT_EQUALS(o.nFields(), 1);
        BSONElement e = o.getField("a");
        long long n = e.numberLong();
        ASSERT_EQUALS(n, v);
        assertBSONTypeEquals(e.type(), data[i].t, v, i);
    }
}

/**
 * current conversion ranges in appendNumber(size_t n)
 * dbl/int max/min in comments refer to max/min encodable constants
 *                  0 <= n <= int_max            -----> int
 *            int_max <  n                       -----> long long
 */

TEST(BSONObjBuilderTest, AppendNumberSizeT) {
    struct {
        size_t v;
        BSONType t;
    } data[] = {{0, mongo::NumberInt},
                {100, mongo::NumberInt},
                {maxEncodableInt, mongo::NumberInt},
                {maxEncodableInt + 1, mongo::NumberLong},
                {size_t(maxInt), mongo::NumberLong},
                {size_t(maxInt) + 1U, mongo::NumberLong},
                {(std::numeric_limits<size_t>::max)(), mongo::NumberLong},
                {0, mongo::Undefined}};
    for (int i = 0; data[i].t != mongo::Undefined; i++) {
        size_t v = data[i].v;
        BSONObjBuilder b;
        b.appendNumber("a", v);
        BSONObj o = b.obj();
        ASSERT_EQUALS(o.nFields(), 1);
        BSONElement e = o.getField("a");
        size_t n = e.numberLong();
        ASSERT_EQUALS(n, v);
        assertBSONTypeEquals(e.type(), data[i].t, v, i);
    }
}

/**
 * current conversion ranges in appendNumber(long long n)
 * dbl/int max/min in comments refer to max/min encodable constants
 *                       n <  dbl_min            -----> long long
 *            dbl_min <= n <  int_min            -----> double
 *            int_min <= n <= int_max            -----> int
 *            int_max <  n <= dbl_max            -----> double
 *            dbl_max <  n                       -----> long long
 */

TEST(BSONObjBuilderTest, AppendNumberLongLong) {
    struct {
        long long v;
        BSONType t;
    } data[] = {{0, mongo::NumberInt},
                {-100, mongo::NumberInt},
                {100, mongo::NumberInt},
                {minEncodableInt, mongo::NumberInt},
                {maxEncodableInt, mongo::NumberInt},
                {minEncodableInt - 1, mongo::NumberDouble},
                {maxEncodableInt + 1, mongo::NumberDouble},
                {minInt, mongo::NumberDouble},
                {maxInt, mongo::NumberDouble},
                {minInt - 1, mongo::NumberDouble},
                {maxInt + 1, mongo::NumberDouble},
                {minEncodableDouble, mongo::NumberDouble},
                {maxEncodableDouble, mongo::NumberDouble},
                {minEncodableDouble - 1, mongo::NumberLong},
                {maxEncodableDouble + 1, mongo::NumberLong},
                {minDouble, mongo::NumberLong},
                {maxDouble, mongo::NumberLong},
                {minDouble - 1, mongo::NumberLong},
                {maxDouble + 1, mongo::NumberLong},
                {minLongLong, mongo::NumberLong},
                {maxLongLong, mongo::NumberLong},
                {0, mongo::Undefined}};
    for (int i = 0; data[i].t != mongo::Undefined; i++) {
        long long v = data[i].v;
        BSONObjBuilder b;
        b.appendNumber("a", v);
        BSONObj o = b.obj();
        ASSERT_EQUALS(o.nFields(), 1);
        BSONElement e = o.getField("a");
        if (data[i].t != mongo::NumberDouble) {
            long long n = e.numberLong();
            ASSERT_EQUALS(n, v);
        } else {
            double n = e.numberDouble();
            ASSERT_APPROX_EQUAL(n, static_cast<double>(v), 0.001);
        }
        assertBSONTypeEquals(e.type(), data[i].t, v, i);
    }
}

TEST(BSONObjBuilderTest, StreamLongLongMin) {
    BSONObj o = BSON("a" << std::numeric_limits<long long>::min());
    ASSERT_EQUALS(o.nFields(), 1);
    BSONElement e = o.getField("a");
    long long n = e.numberLong();
    ASSERT_EQUALS(n, std::numeric_limits<long long>::min());
}

TEST(BSONObjBuilderTest, AppendNumberLongLongMinCompareObject) {
    BSONObjBuilder b;
    b.appendNumber("a", std::numeric_limits<long long>::min());
    BSONObj o1 = b.obj();

    BSONObj o2 = BSON("a" << std::numeric_limits<long long>::min());

    ASSERT_EQUALS(o1, o2);
}

TEST(BSONObjBuilderTest, AppendMaxTimestampOpTimeConversion) {
    BSONObjBuilder b;
    b.appendMaxForType("a", mongo::Timestamp);
    BSONObj o1 = b.obj();

    BSONElement e = o1.getField("a");
    ASSERT_FALSE(e.eoo());
}

TEST(BSONObjBuilderTest, AppendFieldNameContainingNullThrows) {
    BSONObjBuilder b;
    std::string middleNull("foo\0bar", 7);
    std::string beginNull("\0foobar", 7);
    std::string endNull("foobar\0", 7);
    ASSERT_THROWS(b.append(middleNull, "baz"), UserException);
    ASSERT_THROWS(b.append(beginNull, "baz"), UserException);
    ASSERT_THROWS(b.append(endNull, "baz"), UserException);
}

TEST(BSONObjBuilderTest, AppendFieldNameContainingNullIsExceptionSafe) {
    BSONObjBuilder b;
    b.append("foo", 1);
    b.append("bar", 2);

    std::string trollField("foo\0bar", 7);
    ASSERT_THROWS(b.append(trollField, "baz"), UserException);
    b.append("garply", "yoooo");
    BSONObj bo = b.done();
    ASSERT_TRUE(bo.valid());
}

}  // unnamed namespace
