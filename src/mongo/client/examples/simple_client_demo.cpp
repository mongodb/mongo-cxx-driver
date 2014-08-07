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

// See also : http://dochub.mongodb.org/core/cppdrivertutorial

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
        unsigned long long count = c.count("test.foo");
        cout << "count of exiting documents in collection test.foo : " << count << endl;

        c.remove("test.foo", BSONObj());

        bo o = BSON( "hello" << "world" );
        c.insert("test.foo", o);

        string e = c.getLastError();
        if( !e.empty() ) {
            cout << "insert #1 failed: " << e << endl;
        }

        // make an index with a unique key constraint
        c.createIndex("test.foo", IndexSpec().addKeys(BSON("hello"<<1)).unique());

        try {
            c.insert("test.foo", o); // will cause a dup key error on "hello" field
        } catch (const OperationException &) {
            // duplicate key error
        }
        cout << "we expect a dup key error here:" << endl;
        cout << "  " << c.getLastErrorDetailed().toString() << endl;
    } 
    catch(DBException& e) { 
        cout << "caught DBException " << e.toString() << endl;
        return 1;
    }

    return 0;
}

