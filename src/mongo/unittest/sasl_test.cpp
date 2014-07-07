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

#include "mongo/unittest/integration_test.h"
#include "mongo/client/dbclient.h"

using namespace mongo;

namespace {
    bool supports_sasl(DBClientConnection& conn) {
        BSONObj result;
        conn.runCommand("admin", BSON( "buildinfo" << true ), result);
        return result["version"].toString() >= "2.5.3";
    }

    TEST(SASLAuthentication, LDAP) {
        DBClientConnection conn;
        conn.connect("ldaptest.10gen.cc"); // only available internally or on jenkins

        if (supports_sasl(conn)) {
            conn.auth(BSON(
                "mechanism" << "PLAIN" <<
                "user" << "drivers-team" <<
                "pwd" << "mongor0x$xgen" <<
                "digestPassword" << false
            ));
        } else {
            // MongoDB version too old to support SASL
            SUCCEED();
        }

        BSONObj result = conn.findOne("ldap.test", Query("{}"));
        ASSERT_TRUE(result["ldap"].trueValue());
        ASSERT_EQUALS(result["authenticated"].str(), "yeah");
    }

    TEST(SASLAuthentication, DISABLED_Kerberos) {
        // You must run kinit -p drivers@LDAPTEST.10GEN.CC before this test
        DBClientConnection conn;
        conn.connect("ldaptest.10gen.cc"); // only available internally or on jenkins

        if (supports_sasl(conn)) {
            conn.auth(BSON(
                "mechanism" << "GSSAPI" <<
                "user" << "drivers@LDAPTEST.10GEN.CC"
            ));
        } else {
            // MongoDB version too old to support SASL
            SUCCEED();
        }

        BSONObj result = conn.findOne("kerberos.test", Query("{}"));
        ASSERT_TRUE(result["kerberos"].trueValue());
        ASSERT_EQUALS(result["authenticated"].str(), "yeah");
    }
} // namespace
