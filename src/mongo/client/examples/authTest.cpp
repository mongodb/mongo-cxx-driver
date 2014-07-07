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

using namespace mongo;

int main( int argc, const char **argv ) {

    using std::cout;
    using std::endl;
    using std::string;

    const char *port = "27017";
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            cout << "need to pass port as second param" << endl;
            return EXIT_FAILURE;
        }
        port = argv[ 2 ];
    }

    Status status = client::initialize();
    if (!status.isOK()) {
        cout << "failed to initialize the client driver: " << status.toString() << endl;
        return EXIT_FAILURE;
    }

    string errmsg;
    ConnectionString cs = ConnectionString::parse(string("127.0.0.1:") + port, errmsg);
    if (!cs.isValid()) {
        cout << "error parsing url: " << errmsg << endl;
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
    conn->auth(BSON("user" << "eliot" <<
                    "db" << "test" <<
                    "pwd" << "bar" <<
                    "mechanism" << "MONGODB-CR"));

    try {
        conn->auth(BSON("user" << "eliot" <<
                        "db" << "test" <<
                        "pwd" << "bars" << // incorrect password
                        "mechanism" << "MONGODB-CR"));
        // Shouldn't get here.
        cout << "Authentication with invalid password should have failed but didn't" << endl;
        return EXIT_FAILURE;
    } catch (const DBException&) {
        // expected
    }
    return EXIT_SUCCESS;
}
