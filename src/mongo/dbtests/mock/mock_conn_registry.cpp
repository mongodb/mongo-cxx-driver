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

#include "mongo/platform/basic.h"

#include "mongo/dbtests/mock/mock_conn_registry.h"

#include "mongo/base/init.h"
#include "mongo/dbtests/mock/mock_dbclient_connection.h"

namespace mongo {

    using std::string;

    boost::scoped_ptr<MockConnRegistry> MockConnRegistry::_instance;

    MONGO_INITIALIZER(MockConnRegistry)(InitializerContext* context) {
        return MockConnRegistry::init();
    }

    Status MockConnRegistry::init() {
        MockConnRegistry::_instance.reset(new MockConnRegistry());
        return Status::OK();
    }

    MockConnRegistry::MockConnRegistry():
            _mockConnStrHook(this),
            _registryMutex() {
    }

    MockConnRegistry* MockConnRegistry::get() {
        return _instance.get();
    }

    ConnectionString::ConnectionHook* MockConnRegistry::getConnStrHook() {
        return &_mockConnStrHook;
    }

    void MockConnRegistry::addServer(MockRemoteDBServer* server) {
        boost::lock_guard<boost::mutex> sl(_registryMutex);

        const std::string hostName(server->getServerAddress());
        fassert(16533, _registry.count(hostName) == 0);

        _registry[hostName] = server;
    }

    bool MockConnRegistry::removeServer(const std::string& hostName) {
        boost::lock_guard<boost::mutex> sl(_registryMutex);
        return _registry.erase(hostName) == 1;
    }

    void MockConnRegistry::clear() {
        boost::lock_guard<boost::mutex> sl(_registryMutex);
        _registry.clear();
    }

    MockDBClientConnection* MockConnRegistry::connect(const std::string& connStr) {
        boost::lock_guard<boost::mutex> sl(_registryMutex);
        fassert(16534, _registry.count(connStr) == 1);
        return new MockDBClientConnection(_registry[connStr], true);
    }

    MockConnRegistry::MockConnHook::MockConnHook(MockConnRegistry* registry):
            _registry(registry) {
    }

    MockConnRegistry::MockConnHook::~MockConnHook() {
    }

    mongo::DBClientBase* MockConnRegistry::MockConnHook::connect(
                const ConnectionString& connString,
                std::string& errmsg,
                double socketTimeout) {
        const string hostName(connString.toString());
        MockDBClientConnection* conn = _registry->connect(hostName);

        if (!conn->connect(hostName.c_str(), errmsg)) {
            // Assumption: connect never throws, so no leak.
            delete conn;

            // mimic ConnectionString::connect for MASTER type connection to return NULL
            // if the destination is unreachable.
            return NULL;
        }

        return conn;
    }
}
