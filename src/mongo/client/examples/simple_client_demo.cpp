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

    if ( argc > 2 ) {
        std::cout << "usage: " << argv[0] << " [MONGODB_URI]"  << std::endl;
        return EXIT_FAILURE;
    }

    mongo::client::GlobalInstance instance;
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
    if ( !conn ) {
        cout << "couldn't connect : " << errmsg << endl;
        return EXIT_FAILURE;
    }

    try {
        unsigned long long count = conn->count("test.foo");
        cout << "count of exiting documents in collection test.foo : " << count << endl;

        conn->remove("test.foo", BSONObj());

        BSONObj o = BSON( "hello" << "world" );
        conn->insert("test.foo", o);

        string e = conn->getLastError();
        if( !e.empty() ) {
            cout << "insert #1 failed: " << e << endl;
        }

        // make an index with a unique key constraint
        conn->createIndex("test.foo", IndexSpec().addKeys(BSON("hello"<<1)).unique());

        try {
            conn->insert("test.foo", o); // will cause a dup key error on "hello" field
        } catch (const OperationException &) {
            // duplicate key error
        }
        cout << "we expect a dup key error here:" << endl;
        cout << "  " << conn->getLastErrorDetailed().toString() << endl;
    } 
    catch(DBException& e) { 
        cout << "caught DBException " << e.toString() << endl;
        return 1;
    }

    return 0;
}

