// first.cpp

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

/**
 * this is a good first example of how to use mongo from c++
 */

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include "mongo/client/dbclient.h"

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace mongo;

void insert( DBClientBase* conn , const char * name , int num ) {
    BSONObjBuilder obj;
    obj.append( "name" , name );
    obj.append( "num" , num );
    conn->insert( "test.people" , obj.obj() );
    std::string e = conn->getLastError();
    if( !e.empty() ) {
        cout << "insert failed: " << e << endl;
        exit(EXIT_FAILURE);
    }
}

int main( int argc, const char **argv ) {

    if ( argc > 2 ) {
        std::cout << "usage: " << argv[0] << " [MONGODB_URI]"  << std::endl;
        return EXIT_FAILURE;
    }

    client::GlobalInstance instance;
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

    {
        // clean up old data from any previous tests
        BSONObjBuilder query;
        conn->remove( "test.people" , query.obj() );
    }

    insert( conn.get() , "eliot" , 15 );
    insert( conn.get() , "sara" , 23 );

    {
        mongo::BSONObjBuilder query;
        std::auto_ptr<mongo::DBClientCursor> cursor = conn->query( "test.people" , query.obj() );
        if (!cursor.get()) {
            cout << "query failure" << endl;
            return EXIT_FAILURE;
        }

        cout << "using cursor" << endl;
        while ( cursor->more() ) {
            mongo::BSONObj obj = cursor->next();
            cout << "\t" << obj.jsonString() << endl;
        }

    }

    {
        mongo::BSONObjBuilder query;
        query.append( "name" , "eliot" );
        mongo::BSONObj res = conn->findOne( "test.people" , query.obj() );
        cout << res.isEmpty() << "\t" << res.jsonString() << endl;
    }

    {
        mongo::BSONObjBuilder query;
        query.append( "name" , "asd" );
        mongo::BSONObj res = conn->findOne( "test.people" , query.obj() );
        cout << res.isEmpty() << "\t" << res.jsonString() << endl;
    }

    return EXIT_SUCCESS;
}
