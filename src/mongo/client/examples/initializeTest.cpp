// initializeTest.cpp

/*    Copyright 2014 MongoDB Inc.
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
#include <windosck2.h>
#include <windows.h>
#endif

#include "mongo/client/dbclient.h"

#include <iostream>

using namespace std;
using namespace mongo;

void successfulInit(client::Options opts) {
    Status status = client::initialize(opts);
    if ( !status.isOK() ) {
        cout << "failed to initialize the client driver: " << status.toString() << endl;
        throw 1;
    }
}

void failedInit(string errMsg) {
    Status status = client::initialize();
    if ( status.isOK() ) {
        cout << errMsg << endl;
        throw 1;
    }
}

void successfulShutdown() {
    Status status = client::shutdown();
    if ( !status.isOK() ) {
        cout << "failed to shutdown properly: " << status.toString() << endl;
        throw 1;
    }
}

void failedShutdown(string errMsg) {
    Status status = client::shutdown();
    if ( status.isOK() ) {
        cout << errMsg << endl;
        throw 1;
    }
}

int main() {

    client::Options manualShutdownOpts = client::Options();
    manualShutdownOpts.setCallShutdownAtExit(false);

    try {
        // shutdown before initializing, should fail
        failedShutdown("shouldn't shutdown before initializing");

        // first initialize, should succeed
        successfulInit(manualShutdownOpts);

        // shutdown, should succeed
        successfulShutdown();

        // shutdown again, should fail
        failedShutdown("shouldn't shutdown before initializing");

        // first initialize, should succeed
        successfulInit(manualShutdownOpts);

        // second initialize, should fail
        failedInit("shouldn't initialize twice");

        // a final shutdown, should succeed
        successfulShutdown();
    }
    catch (int e) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
