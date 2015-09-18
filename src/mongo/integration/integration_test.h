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

#pragma once

#include <string>
#include <map>
#include <memory>

#include "mongo/orchestration/replica_set.h"
#include "mongo/orchestration/server.h"
#include "mongo/orchestration/service.h"
#include "mongo/unittest/unittest.h"

// Act like we are using the driver externally
#ifdef MONGO_EXPOSE_MACROS
#undef MONGO_EXPOSE_MACROS
#endif

namespace mongo {
namespace integration {

//
// MongoDB Integration Test Environment
//
// Augments the testing environment such that the orchestration service is available.
//
// This Environment is available globally in tests and test fixtures that use the provided
// integration_test_main.cpp.
//
// Google test takes ownership and is responsible for destructing new heap allocated
// instances of this object via AddGlobalTestEnvironment.
//
class Environment : public ::testing::Environment {
public:
    Environment(const std::string& uri, const std::string& preset) {
        _orchestration.reset(new mongo::orchestration::Service(uri));
        _preset = preset;
    }

    static const std::auto_ptr<mongo::orchestration::Service>& orchestration() {
        return _orchestration;
    }

    static const std::string& getPreset() {
        return _preset;
    }

private:
    static std::string _preset;
    static std::auto_ptr<mongo::orchestration::Service> _orchestration;
};

//
// MongoDB Integration Test Fixture for single server ("standalone") tests
//
// Creates a Mongod instance which is unique per test-case and destroys it upon test
// termination.
//
class StandaloneTest : public ::testing::Test {
public:
    static mongo::orchestration::Server server() {
        return Environment::orchestration()->server(_id);
    }

    static void SetUpTestCase() {
        mongo::orchestration::Document params;
        params["preset"] = Environment::getPreset();
        _id = Environment::orchestration()->createMongod(params);
    }

    static void TearDownTestCase() {
        Environment::orchestration()->server(_id).destroy();
    }

private:
    static std::string _id;
};

//
// MongoDB Integration Test Fixture for replica set tests
//
// Creates a Replica Set which is unique per test-case and destroys it upon test
// termination. Uses the arbiter preset which creates a 3 node set with 2 data bearing
// members and an arbiter.
//
class ReplicaSetTest : public ::testing::Test {
public:
    static mongo::orchestration::ReplicaSet rs() {
        return Environment::orchestration()->replicaSet(_id);
    }

    static void SetUpTestCase() {
        mongo::orchestration::Document params;
        params["preset"] = Environment::getPreset();
        _id = Environment::orchestration()->createReplicaSet(params);
    }

    static void TearDownTestCase() {
        Environment::orchestration()->replicaSet(_id).destroy();
    }

private:
    static std::string _id;
};

}  // namespace integration
}  // namespace mongo
