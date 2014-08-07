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

#include "mongo/client/dbclient.h" // the mongo c++ driver

#include <iostream>

using namespace std;
using namespace mongo;

int main(int argc, char* argv[]) {

    Status status = client::initialize();
    if ( !status.isOK() ) {
        std::cout << "failed to initialize the client driver: " << status.toString() << endl;
        return EXIT_FAILURE;
    }

    const char *port = "27017";
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << endl;
            return EXIT_FAILURE;
        }
        port = argv[ 2 ];
    }

    try {
        cout << "connecting to localhost..." << endl;
        DBClientConnection c;
        c.connect(string("localhost:") + port);
        cout << "connected ok" << endl;

        bo o = BSON( "hello" << "world" );

        cout << "dropping collection..." << endl;
        c.dropCollection("test.foo");

        cout << "inserting..." << endl;

        time_t start = time(0);
        for( unsigned i = 0; i < 100000; i++ ) {
            c.insert("test.foo", o);
        }

        // wait until all operations applied
        cout << "getlasterror returns: \"" << c.getLastError() << '"' << endl;

        time_t done = time(0);
        time_t dt = done-start;
        cout << dt << " seconds " << 100000/dt << " per second" << endl;
    }
    catch(DBException& e) {
        cout << "caught DBException " << e.toString() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
