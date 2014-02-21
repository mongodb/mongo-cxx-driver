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

#include <boost/scoped_ptr.hpp>

#include "mongo/util/net/hostandport.h"
#include "mongo/util/net/message_port.h"

namespace mongo {

    class AuthenticationInfo;
    class AuthenticationSession;
    class AuthorizationSession;

    /**
     * this is the base class for Client and ClientInfo
     * Client is for mongod
     * ClientInfo is for mongos
     * They should converge slowly
     * The idea is this has the basic api so that not all code has to be duplicated
     */
    class ClientBasic : boost::noncopyable {
    public:
        virtual ~ClientBasic();
        AuthenticationSession* getAuthenticationSession();
        void resetAuthenticationSession(AuthenticationSession* newSession);
        void swapAuthenticationSession(boost::scoped_ptr<AuthenticationSession>& other);

        bool hasAuthorizationSession() const;
        AuthorizationSession* getAuthorizationSession() const;
        void setAuthorizationSession(AuthorizationSession* authorizationSession);

        bool getIsLocalHostConnection() {
            if (!hasRemote()) {
                return false;
            }
            return getRemote().isLocalHost();
        }

        virtual bool hasRemote() const { return _messagingPort; }
        virtual HostAndPort getRemote() const {
            verify( _messagingPort );
            return _messagingPort->remote();
        }
        AbstractMessagingPort * port() const { return _messagingPort; }

        static ClientBasic* getCurrent();
        static bool hasCurrent();

    protected:
        ClientBasic(AbstractMessagingPort* messagingPort);

    private:
        boost::scoped_ptr<AuthenticationSession> _authenticationSession;
        boost::scoped_ptr<AuthorizationSession> _authorizationSession;
        AbstractMessagingPort* const _messagingPort;
    };
}
