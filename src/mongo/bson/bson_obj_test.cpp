/*    Copyright 2013 10gen Inc.
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

#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"

#include "mongo/unittest/unittest.h"

namespace {

    TEST(BSONObjToString, EmptyArray) {
        const char text[] = "{ x: [] }";
        mongo::BSONObj o1 = mongo::fromjson(text);
        const std::string o1_str = o1.toString();
        ASSERT_EQUALS(text, o1_str);
    }

    TEST(BSONObjCompare, NumberDouble) {
        ASSERT_LT(BSON("" << 0.0), BSON("" << 1.0));
        ASSERT_LT(BSON("" << -1.0), BSON("" << 0.0));
        ASSERT_LT(BSON("" << -1.0), BSON("" << 1.0));

        ASSERT_LT(BSON("" << 0.0), BSON("" << 0.1));
        ASSERT_LT(BSON("" << 0.1), BSON("" << 1.0));
        ASSERT_LT(BSON("" << -1.0), BSON("" << -0.1));
        ASSERT_LT(BSON("" << -0.1), BSON("" << 0.0));
        ASSERT_LT(BSON("" << -0.1), BSON("" << 0.1));

        ASSERT_LT(BSON("" << 0.0), BSON("" << std::numeric_limits<double>::denorm_min()));
        ASSERT_GT(BSON("" << 0.0), BSON("" << -std::numeric_limits<double>::denorm_min()));

        ASSERT_LT(BSON("" << 1.0), BSON("" << (1.0 + std::numeric_limits<double>::epsilon())));
        ASSERT_GT(BSON("" << -1.0), BSON("" << (-1.0 - std::numeric_limits<double>::epsilon())));

        ASSERT_EQ(BSON("" << 0.0), BSON("" << -0.0));

        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()), BSON("" << 0.0));
        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<double>::max())); // max is finite
        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()),
                  BSON("" << -std::numeric_limits<double>::infinity()));

        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()), BSON("" << 0.0));
        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()),
                  BSON("" << -std::numeric_limits<double>::max()));
        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<double>::infinity()));

        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()), BSON("" << 0.0));
        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()),
                  BSON("" << -std::numeric_limits<double>::max()));
        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()),
                  BSON("" << std::numeric_limits<double>::infinity()));
        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()),
                  BSON("" << -std::numeric_limits<double>::infinity()));

        // TODO in C++11 use hex floating point to test distinct NaN representations
        ASSERT_EQ(BSON("" << std::numeric_limits<double>::quiet_NaN()),
                  BSON("" << std::numeric_limits<double>::signaling_NaN()));
    }

    TEST(BSONObjCompare, NumberLong_Double) {
        ASSERT_EQ(BSON("" << 0ll), BSON("" << 0.0));
        ASSERT_EQ(BSON("" << 0ll), BSON("" << -0.0));

        ASSERT_EQ(BSON("" << 1ll), BSON("" << 1.0));
        ASSERT_EQ(BSON("" << -1ll), BSON("" << -1.0));

        ASSERT_LT(BSON("" << 0ll), BSON("" << 1.0));
        ASSERT_LT(BSON("" << -1ll), BSON("" << 0.0));
        ASSERT_LT(BSON("" << -1ll), BSON("" << 1.0));

        ASSERT_LT(BSON("" << 0ll), BSON("" << 0.1));
        ASSERT_LT(BSON("" << 0.1), BSON("" << 1ll));
        ASSERT_LT(BSON("" << -1ll), BSON("" << -0.1));
        ASSERT_LT(BSON("" << -0.1), BSON("" << 0ll));

        ASSERT_LT(BSON("" << 0ll), BSON("" << std::numeric_limits<double>::denorm_min()));
        ASSERT_GT(BSON("" << 0ll), BSON("" << -std::numeric_limits<double>::denorm_min()));

        ASSERT_LT(BSON("" << 1ll), BSON("" << (1.0 + std::numeric_limits<double>::epsilon())));
        ASSERT_GT(BSON("" << -1ll), BSON("" << (-1.0 - std::numeric_limits<double>::epsilon())));

        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()), BSON("" << 0ll));
        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<long long>::max()));
        ASSERT_GT(BSON("" << std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<long long>::min()));

        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()), BSON("" << 0ll));
        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<long long>::max()));
        ASSERT_LT(BSON("" << -std::numeric_limits<double>::infinity()),
                  BSON("" << std::numeric_limits<long long>::min()));

        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()), BSON("" << 0ll));
        ASSERT_LT(BSON("" << std::numeric_limits<double>::quiet_NaN()),
                  BSON("" << std::numeric_limits<long long>::min()));

        for (int powerOfTwo = 0; powerOfTwo < 63; powerOfTwo++) {
            const long long lNum = 1ll << powerOfTwo;
            const double dNum = double(lNum);

            // All powers of two in this range can be represented exactly as doubles.
            invariant(lNum == static_cast<long long>(dNum));

            ASSERT_EQ(BSON("" << lNum), BSON("" << dNum));
            ASSERT_EQ(BSON("" << -lNum), BSON("" << -dNum));

            ASSERT_GT(BSON("" << (lNum + 1)), BSON("" << dNum));
            ASSERT_LT(BSON("" << (lNum - 1)), BSON("" << dNum));
            ASSERT_GT(BSON("" << (-lNum + 1)), BSON("" << -dNum));
            ASSERT_LT(BSON("" << (-lNum - 1)), BSON("" << -dNum));

            if (powerOfTwo <= 52) { // is dNum - 0.5 representable?
                ASSERT_GT(BSON("" << lNum), BSON("" << (dNum - 0.5)));
                ASSERT_LT(BSON("" << -lNum), BSON("" << -(dNum - 0.5)));
            }

            if (powerOfTwo <= 51) { // is dNum + 0.5 representable?
                ASSERT_LT(BSON("" << lNum), BSON("" << (dNum + 0.5)));
                ASSERT_GT(BSON("" << -lNum), BSON("" << -(dNum + 0.5)));
            }
        }

        {
            // Numbers around +/- numeric_limits<long long>::max() which can't be represented
            // precisely as a double.
            const long long maxLL = std::numeric_limits<long long>::max();
            const double closestAbove = 9223372036854775808.0; // 2**63
            const double closestBelow = 9223372036854774784.0; // 2**63 - epsilon

            ASSERT_GT(BSON("" << maxLL), BSON("" << (maxLL - 1)));
            ASSERT_LT(BSON("" << maxLL), BSON("" << closestAbove));
            ASSERT_GT(BSON("" << maxLL), BSON("" << closestBelow));

            ASSERT_LT(BSON("" << -maxLL), BSON("" << -(maxLL - 1)));
            ASSERT_GT(BSON("" << -maxLL), BSON("" << -closestAbove));
            ASSERT_LT(BSON("" << -maxLL), BSON("" << -closestBelow));
        }

        {
            // Numbers around numeric_limits<long long>::min() which can be represented precisely as
            // a double, but not as a positive long long.
            const long long minLL = std::numeric_limits<long long>::min();
            const double closestBelow = -9223372036854777856.0; // -2**63 - epsilon
            const double equal = -9223372036854775808.0; // 2**63
            const double closestAbove = -9223372036854774784.0; // -2**63 + epsilon

            invariant(static_cast<double>(minLL) == equal);
            invariant(static_cast<long long>(equal) == minLL);

            ASSERT_LT(BSON("" << minLL), BSON("" << (minLL + 1)));

            ASSERT_EQ(BSON("" << minLL), BSON("" << equal));
            ASSERT_LT(BSON("" << minLL), BSON("" << closestAbove));
            ASSERT_GT(BSON("" << minLL), BSON("" << closestBelow));
        }
    }

} // unnamed namespace
