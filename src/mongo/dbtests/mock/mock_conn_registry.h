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

#include <boost/thread/mutex.hpp>

#include "mongo/base/status.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/dbtests/mock/mock_dbclient_connection.h"
#include "mongo/dbtests/mock/mock_remote_db_server.h"
#include "mongo/platform/unordered_map.h"

namespace mongo {
/**
 * Registry for storing mock servers and can create mock connections to these
 * servers.
 */
class MockConnRegistry {
public:
    /**
     * Initializes the static instance.
     */
    static Status init();

    /**
     * @return the singleton registry. If this needs to be called before main(),
     *     then the initializer method should depend on "MockConnRegistry".
     */
    static MockConnRegistry* get();

    /**
     * Adds a server to this registry.
     *
     * @param server the server to add. Caller is responsible for disposing
     *     the server.
     */
    void addServer(MockRemoteDBServer* server);

    /**
     * Removes the server from this registry.
     *
     * @param hostName the host name of the server to remove.
     *
     * @return true if the server is in the registry and was removed.
     */
    bool removeServer(const std::string& hostName);

    /**
     * Clears the registry.
     */
    void clear();

    /**
     * @return a new mocked connection to a server with the given hostName.
     */
    MockDBClientConnection* connect(const std::string& hostName);

    /**
     * @return the hook that can be used with ConnectionString.
     */
    ConnectionString::ConnectionHook* getConnStrHook();

private:
    class MockConnHook : public ConnectionString::ConnectionHook {
    public:
        /**
         * Creates a new connection hook for the ConnectionString class that
         * can create mock connections to mock replica set members using their
         * pseudo host names.
         *
         * @param replSet the mock replica set. Caller is responsible for managing
         *     replSet and making sure that it lives longer than this object.
         */
        MockConnHook(MockConnRegistry* registry);
        ~MockConnHook();

        mongo::DBClientBase* connect(const mongo::ConnectionString& connString,
                                     std::string& errmsg,
                                     double socketTimeout);

    private:
        MockConnRegistry* _registry;
    };

    MockConnRegistry();

    static boost::scoped_ptr<MockConnRegistry> _instance;

    MockConnHook _mockConnStrHook;

    // protects _registry
    boost::mutex _registryMutex;
    unordered_map<std::string, MockRemoteDBServer*> _registry;
};
}
