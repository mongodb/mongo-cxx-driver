//tutorial.cpp

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

using namespace std;
using namespace mongo;

int printIfAge(DBClientBase* conn, int age) {
    std::auto_ptr<DBClientCursor> cursor = conn->query("tutorial.persons", MONGO_QUERY( "age" << age ).sort("name") );
    if (!cursor.get()) {
        cout << "query failure" << endl;
        return EXIT_FAILURE;
    }

    while( cursor->more() ) {
        BSONObj p = cursor->next();
        cout << p.getStringField("name") << endl;
    }
    return EXIT_SUCCESS;
}

int run(DBClientBase* conn) {

    cout << "connected ok" << endl;
    BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
    conn->insert("tutorial.persons", p);
    p = BSON( "name" << "Jane" << "age" << 40 );
    conn->insert("tutorial.persons", p);
    p = BSON( "name" << "Abe" << "age" << 33 );
    conn->insert("tutorial.persons", p);
    p = BSON( "name" << "Methuselah" << "age" << BSONNULL);
    conn->insert("tutorial.persons", p);
    p = BSON( "name" << "Samantha" << "age" << 21 << "city" << "Los Angeles" << "state" << "CA" );
    conn->insert("tutorial.persons", p);

    conn->createIndex("tutorial.persons", fromjson("{age:1}"));

    cout << "count:" << conn->count("tutorial.persons") << endl;

    std::auto_ptr<DBClientCursor> cursor = conn->query("tutorial.persons", BSONObj());
    if (!cursor.get()) {
        cout << "query failure" << endl;
        return EXIT_FAILURE;
    }

    while( cursor->more() ) {
        cout << cursor->next().toString() << endl;
    }

    cout << "\nprintifage:\n";
    return printIfAge(conn, 33);
}

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

    int ret = EXIT_SUCCESS;
    try {
        ret = run(conn.get());
    }
    catch( DBException &e ) {
        cout << "caught " << e.what() << endl;
        ret = EXIT_FAILURE;
    }
    return ret;
}
