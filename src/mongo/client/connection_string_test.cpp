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

    struct URLTestCase {
        std::string URL;
        std::string uname;
        std::string password;
        mongo::ConnectionString::ConnectionType type;
        std::string setname;
        size_t numservers;
        size_t numOptions;
        std::string database;
    };

    const mongo::ConnectionString::ConnectionType kMaster = mongo::ConnectionString::MASTER;
    const mongo::ConnectionString::ConnectionType kSet = mongo::ConnectionString::SET;

    const URLTestCase cases[] = {

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
    };


    TEST(ConnectionString, GoodTrickyURLs) {

        const size_t numCases = sizeof(cases) / sizeof(cases[0]);

        for (size_t i = 0; i != numCases; ++i) {
            const URLTestCase testCase = cases[i];
            std::cout << "Testing URL: " << testCase.URL << '\n';
            std::string errMsg;
            const ConnectionString result = ConnectionString::parse(testCase.URL, errMsg);
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

} // namespace
