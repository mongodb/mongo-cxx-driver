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

/*
 * Shim to allow mongoclient tests to build with gtest instead of
 * built in MongoDB unittest framework.
 *
 * Note: FAIL(str) has been converted to FAIL() << str; in tests.
 *
 */

#pragma once

#include <gtest/gtest.h>

#include <boost/shared_ptr.hpp>

#include <mongo/util/mongoutils/str.h>

#define ASSERT ASSERT_TRUE
#define ASSERT_EQUALS ASSERT_EQ
#define ASSERT_NOT_EQUALS ASSERT_NE
#define ASSERT_GREATER_THAN ASSERT_GT
#define ASSERT_LESS_THAN_OR_EQUALS ASSERT_LE
#define ASSERT_LESS_THAN ASSERT_LT
#define ASSERT_GREATER_THAN_OR_EQUALS ASSERT_GE
#define ASSERT_GREATER_THAN ASSERT_GT
#define ASSERT_APPROX_EQUAL ASSERT_NEAR
#define ASSERT_OK(EXPRESSION) ASSERT_EQUALS(::mongo::Status::OK(), (EXPRESSION))
#define ASSERT_NOT_OK(EXPRESSION) ASSERT_NOT_EQUALS(::mongo::Status::OK(), (EXPRESSION))
#define ASSERT_THROWS(STATEMENT, EXCEPTION) ASSERT_THROW(STATEMENT, EXCEPTION)

namespace mongo {

    namespace unittest {
        class Test : public ::testing::Test {
            virtual void setUp() {}
            virtual void SetUp() {
                setUp();
            }

            virtual void tearDown() {}
            virtual void TearDown() {
                tearDown();
            }
        };
    } // unittest
} // mongo

using namespace mongo;
using namespace mongoutils;
using boost::shared_ptr;

#define DBTEST_SHIM_TEST_NAMED(klass, name, ...)     \
    TEST(klass, name) {                              \
        klass(__VA_ARGS__).run();                    \
    }

#define DBTEST_SHIM_TEST(klass, ...)                 \
    DBTEST_SHIM_TEST_NAMED(klass, Test, __VA_ARGS__)
