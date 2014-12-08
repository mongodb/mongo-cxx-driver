// @file whereExample.cpp
// @see http://dochub.mongodb.org/core/serversidecodeexecution

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

#include <iostream>

#ifndef verify
#  define verify(x) MONGO_verify(x)
#endif

using namespace std;
using namespace mongo;

int main( int argc, const char **argv ) {

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

    const char * ns = "test.where";

    conn->remove( ns , BSONObj() );

    conn->insert( ns , BSON( "name" << "eliot" << "num" << 17 ) );
    conn->insert( ns , BSON( "name" << "sara" << "num" << 24 ) );

    std::auto_ptr<DBClientCursor> cursor = conn->query( ns , BSONObj() );
    if (!cursor.get()) {
        cout << "query failure" << endl;
        return EXIT_FAILURE;
    }

    while ( cursor->more() ) {
        BSONObj obj = cursor->next();
        cout << "\t" << obj.jsonString() << endl;
    }

    cout << "now using $where" << endl;

    Query q = Query("{}").where("this.name == name" , BSON( "name" << "sara" ));

    cursor = conn->query( ns , q );
    if (!cursor.get()) {
        cout << "query failure" << endl;
        return EXIT_FAILURE;
    }

    int num = 0;
    while ( cursor->more() ) {
        BSONObj obj = cursor->next();
        cout << "\t" << obj.jsonString() << endl;
        num++;
    }
    verify( num == 1 );

    return EXIT_SUCCESS;
}
