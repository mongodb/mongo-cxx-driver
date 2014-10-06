/*    Copyright 2014 10gen Inc.
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

#include <cstdlib>
#include <iostream>
#include <memory>

#include "mongo/client/dbclient.h"

using namespace mongo;

int main(int argc, char* argv[]) {

    const char *port = "27017";
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << std::endl;
            return EXIT_FAILURE;
        }
        port = argv[ 2 ];
    }

    mongo::client::GlobalInstance instance;
    if (!instance.initialized()) {
        std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
        return EXIT_FAILURE;
    }

    DBClientConnection conn;
    conn.connect(std::string("localhost:").append(port));
    conn.dropCollection("test.test");

    // Don't run on MongoDB < 2.2
    BSONObj cmdResult;
    conn.runCommand("admin", BSON("buildinfo" << true), cmdResult);
    std::vector<BSONElement> versionArray = cmdResult["versionArray"].Array();
    if (versionArray[0].Int() < 2 || versionArray[1].Int() < 2)
        return EXIT_SUCCESS;

    conn.insert("test.test", BSON("x" << 0));
    conn.insert("test.test", BSON("x" << 1));
    conn.insert("test.test", BSON("x" << 1));
    conn.insert("test.test", BSON("x" << 2));
    conn.insert("test.test", BSON("x" << 2));
    conn.insert("test.test", BSON("x" << 2));

    std::auto_ptr<DBClientCursor> cursor = conn.aggregate("test.test",
        BSON_ARRAY(
            BSON("$match" << BSON("x" << GT << 0)) <<
            BSON("$group" << BSON("_id" << "$x" << "count" << BSON("$sum" << 1)))
        )
    );

    std::cout << "------- AGGREGATION -------" << std::endl;
    while (cursor->more()) {
        std::cout << cursor->next() << std::endl;
    }

    return EXIT_SUCCESS;
}
