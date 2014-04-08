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

#include <iostream>

#include "mongo/unittest/integration_test.h"
#include "mongo/client/init.h"

namespace mongo {
    namespace unittest {
        IntegrationTestParams integrationTestParams;
    } // namespace unittest
} // namespace mongo

int main(int argc, char **argv) {
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << std::endl;
            return EXIT_FAILURE;
        }
        mongo::unittest::integrationTestParams.port = argv[2];
    } else {
        mongo::unittest::integrationTestParams.port = "27107";
    }

    mongo::Status status = mongo::client::initialize();
    if (!status.isOK())
        ::abort();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
