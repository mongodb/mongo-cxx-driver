// authTest.cpp

/*    Copyright 2009 10gen Inc.
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

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include "mongo/client/dbclient.h"

#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <utility>

using namespace mongo;

bool serverLTE(DBClientBase* c, int major, int minor) {
    BSONObj result;
    c->runCommand("admin", BSON("buildinfo" << true), result);

    std::vector<BSONElement> version = result.getField("versionArray").Array();
    int serverMajor = version[0].Int();
    int serverMinor = version[1].Int();

    // std::pair uses lexicographic ordering
    return std::make_pair(serverMajor, serverMinor) <=
           std::make_pair(major, minor);
}


int main( int argc, const char **argv ) {

    using std::cout;
    using std::endl;
    using std::string;

    if ( argc > 2 ) {
        std::cout << "usage: " << argv[0] << " [MONGODB_URI]"  << std::endl;
        return EXIT_FAILURE;
    }

    client::Options autoTimeoutOpts = client::Options();
    autoTimeoutOpts.setAutoShutdownGracePeriodMillis(250);

    mongo::client::GlobalInstance instance(autoTimeoutOpts);
    if (!instance.initialized()) {
        std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
        return EXIT_FAILURE;
    }

    std::string uri = argc == 2 ? argv[1] : "mongodb://localhost:27017";
    std::string errmsg;

    ConnectionString cs = ConnectionString::parse(uri, errmsg);

    if (!cs.isValid()) {
        std::cout << "Error parsing connection string " << uri << ": " << errmsg << std::endl;
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<DBClientBase> conn(cs.connect(errmsg));
    if (!conn) {
        cout << "couldn't connect: " << errmsg << endl;
        return EXIT_FAILURE;
    }

    bool worked;
    BSONObj ret;

    // clean up old data from any previous tests
    worked = conn->runCommand( "test", BSON("dropAllUsersFromDatabase" << 1), ret );
    if (!worked) {
        cout << "Running MongoDB < 2.5.3 so falling back to old remove" << endl;
        conn->remove( "test.system.users" , BSONObj() );
    }

    // create a new user
    worked = conn->runCommand( "test",
        BSON( "createUser" << "eliot" <<
                "pwd" << "bar" <<
                "roles" << BSON_ARRAY("readWrite")),
        ret);
    if (!worked) {
        cout << "Running MongoDB < 2.5.3 so falling back to old user creation" << endl;
        conn->insert( "test.system.users" , BSON( "user" <<
            "eliot" << "pwd" << conn->createPasswordDigest( "eliot" , "bar" ) ) );
    }

    errmsg.clear();
    if (!conn->auth("test", "eliot", "bar", errmsg)) {
        cout << "Authentication failed, when it should have succeeded. Got error: " << errmsg << endl;
        return EXIT_FAILURE;
    }

    try {
        if (conn->auth("test", "eliot", "bars", errmsg)) { // incorrect password
            cout << "Authentication with invalid password should have failed but didn't" << endl;
            return EXIT_FAILURE;
        }
    } catch (const DBException&) {
        //Expected on v2.2 and below
        assert(serverLTE(conn.get(), 2, 2));
    }

    return EXIT_SUCCESS;
}
