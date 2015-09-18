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

#include "mongo/unittest/unittest.h"

#include <string>

#include "mongo/client/dbclientinterface.h"

namespace mongo {

using namespace std;

/* Connection String */
TEST(ConnectionString, SameLogicalEndpoint) {
    string err1;
    string err2;
    ConnectionString cs1;
    ConnectionString cs2;

    // INVALID -- default non parsed state
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));
    cs2 = ConnectionString::parse("mongodb://host1,host2,host3", err1);
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

    // MASTER
    cs1 = ConnectionString::parse("mongodb://localhost:1234", err1);
    cs2 = ConnectionString::parse("mongodb://localhost:1234", err2);
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

    // PAIR -- compares the host + port even in swapped order
    cs1 = cs1.parse("mongodb://localhost:1234,localhost:5678", err1);
    cs2 = cs2.parse("mongodb://localhost:1234,localhost:5678", err2);
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));
    cs2 = cs2.parse("mongodb://localhost:5678,localhost:1234", err2);
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

    // SET -- compares the set name only
    cs1 = cs1.parse("mongodb://localhost:1234,localhost:5678/?replicaSet=testset", err1);
    cs2 = cs2.parse("mongodb://localhost:5678,localhost:1234/?replicaSet=testset", err2);
    ASSERT_TRUE(cs1.sameLogicalEndpoint(cs2));

    // Different parsing methods
    cs1 = cs1.parseDeprecated("testset/localhost:1234,localhost:5678", err1);
    cs2 = cs2.parse("mongodb://localhost:5678,localhost:1234", err2);
    ASSERT_FALSE(cs1.sameLogicalEndpoint(cs2));
}

TEST(ConnectionString, TypeToString) {
    ASSERT_EQUALS(ConnectionString::typeToString(ConnectionString::INVALID), "invalid");
    ASSERT_EQUALS(ConnectionString::typeToString(ConnectionString::MASTER), "master");
    ASSERT_EQUALS(ConnectionString::typeToString(ConnectionString::PAIR), "pair");
    ASSERT_EQUALS(ConnectionString::typeToString(ConnectionString::SET), "set");
    ASSERT_EQUALS(ConnectionString::typeToString(ConnectionString::CUSTOM), "custom");
}

}  // namespace mongo
