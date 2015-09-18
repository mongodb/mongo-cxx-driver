/*
 *    Copyright (C) 2014 MongoDB Inc.
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

#include "mongo/base/disallow_copying.h"
#include "mongo/base/string_data.h"

namespace mongo {

class SaslClientSession;
template <typename T>
class StatusWith;

/**
 * Abstract class for implementing the clent-side
 * of a SASL mechanism conversation.
 */
class SaslClientConversation {
    MONGO_DISALLOW_COPYING(SaslClientConversation);

public:
    /**
     * Implements the client side of a SASL authentication mechanism.
     *
     * "saslClientSession" is the corresponding SASLClientSession.
     * "saslClientSession" must stay in scope until the SaslClientConversation's
     *  destructor completes.
     *
     **/
    explicit SaslClientConversation(SaslClientSession* saslClientSession)
        : _saslClientSession(saslClientSession) {}

    virtual ~SaslClientConversation();

    /**
     * Performs one step of the client side of the authentication session,
     * consuming "inputData" and producing "*outputData".
     *
     * A return of Status::OK() indicates successful progress towards authentication.
     * A return of !Status::OK() indicates failed authentication
     *
     * A return of true means that the authentication process has finished.
     * A return of false means that the authentication process has more steps.
     *
     */
    virtual StatusWith<bool> step(const StringData& inputData, std::string* outputData) = 0;

protected:
    SaslClientSession* _saslClientSession;
};

}  // namespace mongo
