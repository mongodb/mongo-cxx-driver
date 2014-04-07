#include "mongo/unittest/integration_test.h"
#include "mongo/client/dbclient.h"

using namespace mongo;

namespace {
    bool supports_sasl(DBClientConnection& conn) {
        BSONObj result;
        conn.runCommand("admin", BSON( "buildinfo" << true ), result);
        return result["version"].toString() >= "2.5.3";
    }
} // namespace

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
