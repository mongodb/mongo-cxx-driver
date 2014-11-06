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

#include "mongo/integration/integration_test.h"
#include "mongo/client/init.h"

namespace mongo {
    namespace integration {
        std::auto_ptr<mongo::orchestration::Service> Environment::_orchestration;
        std::string Environment::_preset;
        std::string mongo::integration::StandaloneTest::_id;
        std::string mongo::integration::ReplicaSetTest::_id;
    } // namespace integration
} // namespace mongo

int main(int argc, char **argv) {

    if (!(argc == 2 || argc == 3)) {
        std::cout << "usage: " << argv[0] <<
            " MONGO_ORCHESTRATION_HOST:MONGO_ORCHESTRATION_PORT" <<
            " [MONGO_ORCHESTRATION_PRESET]" << std::endl;
    }

    mongo::client::GlobalInstance instance;
    if (!instance.initialized()) {
        std::cerr << "failed to initialize the client driver: " << instance.status() << std::endl;
        ::abort();
    }

    std::string preset("basic.json");
    if (argc == 3) {
        preset = argv[2];
    }

    // Google test takes ownership of Environment and destroys it when finished.
    ::testing::AddGlobalTestEnvironment(new mongo::integration::Environment(argv[1], preset));
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
