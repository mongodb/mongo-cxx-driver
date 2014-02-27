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

#include <iostream>
#include "mongo/client/dbclient.h"

using namespace mongo;

int printIfAge(DBClientConnection& c, int age) {
    std::auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", QUERY( "age" << age ).sort("name") );
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

int run(int argc, char* argv[]) {

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

    DBClientConnection c;
    c.connect(string("localhost:") + port); //"192.168.58.1");
    cout << "connected ok" << endl;
    BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
    c.insert("tutorial.persons", p);
    p = BSON( "name" << "Jane" << "age" << 40 );
    c.insert("tutorial.persons", p);
    p = BSON( "name" << "Abe" << "age" << 33 );
    c.insert("tutorial.persons", p);
    p = BSON( "name" << "Methuselah" << "age" << BSONNULL);
    c.insert("tutorial.persons", p);
    p = BSON( "name" << "Samantha" << "age" << 21 << "city" << "Los Angeles" << "state" << "CA" );
    c.insert("tutorial.persons", p);

    c.ensureIndex("tutorial.persons", fromjson("{age:1}"));

    cout << "count:" << c.count("tutorial.persons") << endl;

    std::auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", BSONObj());
    if (!cursor.get()) {
        cout << "query failure" << endl;
        return EXIT_FAILURE;
    }

    while( cursor->more() ) {
        cout << cursor->next().toString() << endl;
    }

    cout << "\nprintifage:\n";
    return printIfAge(c, 33);
}

int main(int argc, char* argv[]) {
    int ret = EXIT_SUCCESS;
    try {
        ret = run(argc, argv);
    }
    catch( DBException &e ) {
        cout << "caught " << e.what() << endl;
        ret = EXIT_FAILURE;
    }
    return ret;
}
