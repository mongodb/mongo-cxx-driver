#include <iostream>
#include "mongo/client/dbclient.h"
#include "mongo/bson/bson.h"

using namespace mongo;

bool create_user(DBClientConnection& conn) {
    BSONObj result;
    bool worked;
    worked = conn.runCommand("$external", BSON(
        "createUser" << "tyler" <<
        "roles" << BSON_ARRAY(
            BSON("role" << "readWrite" << "db" << "sasltest")
        )
    ), result);
    return worked;
}

bool supports_sasl(DBClientConnection& conn) {
    BSONObj result;
    conn.runCommand("admin", BSON( "buildinfo" << true ), result);
    if (result["version"].toString() >= "2.5.3") {
        return true;
    } else {
        return false;
    }
}

int main() {
    client::initialize();
    DBClientConnection conn;
    conn.connect("ldaptest.10gen.cc");

    if (supports_sasl(conn)) {
        // mechanism: "PLAIN",
        // user: <username>,
        // pwd:  <cleartext password>,
        // digestPassword: false
        conn.auth(BSON(
            "mechanism" << "PLAIN" <<
            "user" << "drivers-team" <<
            "pwd" << "mongor0x$xgen" <<
            "digestPassword" << false
        ));
    } else {
        std::cout << "MongoDB version too old to support SASL" << std::endl;
    }

    BSONObj single = conn.findOne("ldap.test", Query("{}"));
    //assert(single["ldap"] && single["authenticated"].toString("yeah"));
    std::cout << single << std::endl;

    return EXIT_SUCCESS;
}
