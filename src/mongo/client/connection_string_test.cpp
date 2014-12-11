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

#include "mongo/client/dbclientinterface.h"

namespace {
    using mongo::ConnectionString;

    struct URITestCase {
        std::string URI;
        std::string uname;
        std::string password;
        mongo::ConnectionString::ConnectionType type;
        std::string setname;
        size_t numservers;
        size_t numOptions;
        std::string database;
    };

    struct InvalidURITestCase {
        std::string URI;
    };

    const mongo::ConnectionString::ConnectionType kMaster = mongo::ConnectionString::MASTER;
    const mongo::ConnectionString::ConnectionType kSet = mongo::ConnectionString::SET;
    const mongo::ConnectionString::ConnectionType kPair = mongo::ConnectionString::PAIR;

    const URITestCase validCases[] = {

        { "mongodb://user:pwd@127.0.0.1", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@127.0.0.1", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://127.0.0.1/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@127.0.0.1:1234", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@127.0.0.1:1234", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://127.0.0.1:1234/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@127.0.0.1,127.0.0.2/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@127.0.0.1,127.0.0.2/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://127.0.0.1,127.0.0.2/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName" },

        { "mongodb://user:pwd@127.0.0.1:1234,127.0.0.2:1234/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@127.0.0.1:1234,127.0.0.2:1234/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://127.0.0.1:1234,127.0.0.1:1234/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName" },

        { "mongodb://user:pwd@[::1]", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@[::1]", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://[::1]/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@[::1]:1234", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@[::1]:1234", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://[::1]:1234/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@[::1],127.0.0.2/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@[::1],127.0.0.2/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://[::1],127.0.0.2/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName" },

        { "mongodb://user:pwd@[::1]:1234,127.0.0.2:1234/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@[::1]:1234,127.0.0.2:1234/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://[::1]:1234,[::1]:1234/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName" },

        { "mongodb://user:pwd@[::1]", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@[::1]", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://[::1]/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@[::1]:1234", "user", "pwd", kMaster, "", 1, 0, "" },

        { "mongodb://user@[::1]:1234", "user", "", kMaster, "", 1, 0, "" },

        { "mongodb://[::1]:1234/dbName?foo=a&c=b", "", "", kMaster, "", 1, 2, "dbName" },

        { "mongodb://user:pwd@[::1],127.0.0.2/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@[::1],127.0.0.2/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://[::1],127.0.0.2/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName" },

        { "mongodb://user:pwd@[::1]:1234,127.0.0.2:1234/?replicaSet=replName", "user", "pwd", kSet, "replName", 2, 1, "" },

        { "mongodb://user@[::1]:1234,127.0.0.2:1234/?replicaSet=replName", "user", "", kSet, "replName", 2, 1, "" },

        { "mongodb://[::1]:1234,[::1]:1234/dbName?foo=a&c=b&replicaSet=replName", "", "", kSet, "replName", 2, 3, "dbName"},

        { "mongodb://user:pwd@[::1]/?authMechanism=GSSAPI&authMechanismProperties=SERVICE_NAME:foobar", "user", "pwd", kMaster, "", 1, 2, "" },

        { "mongodb://user:pwd@[::1]/?authMechanism=GSSAPI&gssapiServiceName=foobar", "user", "pwd", kMaster, "", 1, 2, "" },
        { "mongodb:///tmp/mongodb-27017.sock", "", "", kMaster, "", 1, 0, "" },

        { "mongodb:///tmp/mongodb-27017.sock,/tmp/mongodb-27018.sock/?replicaSet=replName", "", "", kSet, "replName", 2, 1, "" }
    };

    const InvalidURITestCase invalidCases[] = {

        { "localhost:27017" },

        { "127.0.0.2:1234,104.9.12.3:27017" },

        { "127.0.0.2:1234,104.9.12.3:27017/?replName=shaun" },

        { "replSetName/localhost:27027" },

        { "anything,anything/?thatDoesntStartWith=mongodb://" },

        { "mongodb://" },

        { "mongodb://localhost:27017,localhost:27018?replicaSet=missingSlash" },
    };

    TEST(ConnectionString, GoodTrickyURIs) {

        const size_t numCases = sizeof(validCases) / sizeof(validCases[0]);

        for (size_t i = 0; i != numCases; ++i) {
            const URITestCase testCase = validCases[i];
            std::cout << "Testing URI: " << testCase.URI << '\n';
            std::string errMsg;
            const ConnectionString result = ConnectionString::parse(testCase.URI, errMsg);
            if (!errMsg.empty()) { std::cout << "error with uri: " << errMsg << std::endl; }
            ASSERT_TRUE(result.isValid());
            ASSERT_TRUE(errMsg.empty());
            ASSERT_EQ(testCase.uname, result.getUser());
            ASSERT_EQ(testCase.password, result.getPassword());
            ASSERT_EQ(testCase.type, result.type());
            ASSERT_EQ(testCase.setname, result.getSetName());
            ASSERT_EQ(testCase.numservers, result.getServers().size());
            BSONObj options = result.getOptions();
            std::set<std::string> fieldNames;
            options.getFieldNames(fieldNames);
            ASSERT_EQ(testCase.numOptions, fieldNames.size());
            ASSERT_EQ(testCase.database, result.getDatabase());
        }
    }

    TEST(ConnectionString, InvalidURIs) {
        const size_t numCases = sizeof(invalidCases) / sizeof(invalidCases[0]);

        for (size_t i = 0; i != numCases; ++i) {
            const InvalidURITestCase testCase = invalidCases[i];
            std::cout << "Testing URI: " << testCase.URI << '\n';
            std::string errMsg;
            const ConnectionString result = ConnectionString::parse(testCase.URI, errMsg);
            ASSERT_FALSE(result.isValid());
        }
    }

    // Test Deprecated URI Parsing

    struct DeprecatedURITestCase {
        std::string URI;
        size_t numservers;
        mongo::ConnectionString::ConnectionType type;
        std::string setname;
    };

    const DeprecatedURITestCase validDeprecatedCases[] = {

        { "localhost:27017", 1, kMaster, "" },

        { "localhost:27017,localhost:30000", 2, kPair, "" },

        { "replName/localhost:27017,127.0.0.2:1234", 2, kSet, "replName" },

        { "localhost:1050,localhost:1055/?replicaSet=rs-1234", 1, kSet, "localhost:1050,localhost:1055" },
    };

    const InvalidURITestCase invalidDeprecatedCases[] = {

        { "1.2.3.4:5555,6.7.8.9:1000,127.0.0.2:1234" },

        { "localhost:27017,localhost:27018,localhost:27019,localhost:27020?replicaSet=myReplicaSet" },

    };

    TEST(ConnectionString, GoodDeprecatedURIs) {
        const size_t numCases = sizeof(validDeprecatedCases) / sizeof(validDeprecatedCases[0]);

        for (size_t i = 0; i != numCases; ++i) {
            const DeprecatedURITestCase testCase = validDeprecatedCases[i];
            std::cout << "Testing URI: " << testCase.URI << '\n';
            std::string errMsg;
            const ConnectionString result = ConnectionString::parseDeprecated(testCase.URI, errMsg);
            ASSERT_TRUE(result.isValid());
            ASSERT_TRUE(errMsg.empty());
            ASSERT_EQ(testCase.numservers, result.getServers().size());
            ASSERT_EQ(testCase.type, result.type());
            ASSERT_EQ(testCase.setname, result.getSetName());
        }
    }

    TEST(ConnectionString, InvalidDeprecatedURIs) {
        const size_t numCases = sizeof(invalidDeprecatedCases) / sizeof(invalidDeprecatedCases[0]);

        for (size_t i = 0; i != numCases; ++i) {
            const InvalidURITestCase testCase = invalidDeprecatedCases[i];
            std::cout << "Testing URI: " << testCase.URI << '\n';
            std::string errMsg;
            const ConnectionString result = ConnectionString::parseDeprecated(testCase.URI, errMsg);
            ASSERT_FALSE(result.isValid());
        }
    }

} // namespace
