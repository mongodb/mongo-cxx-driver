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
#include <vector>

#include "mongo/base/disallow_copying.h"
#include "mongo/base/status.h"
#include "mongo/base/string_data.h"
#include "mongo/client/sasl_client_conversation.h"
#include "mongo/crypto/mechanism_scram.h"

namespace mongo {
/**
 *  Client side authentication session for SASL PLAIN.
 */
class SaslSCRAMSHA1ClientConversation : public SaslClientConversation {
    MONGO_DISALLOW_COPYING(SaslSCRAMSHA1ClientConversation);

public:
    /**
     * Implements the client side of a SASL PLAIN mechanism session.
     **/
    explicit SaslSCRAMSHA1ClientConversation(SaslClientSession* saslClientSession);

    virtual ~SaslSCRAMSHA1ClientConversation();

    /**
     * Takes one step in a SCRAM-SHA-1 conversation.
     *
     * @return !Status::OK() for failure. The boolean part indicates if the
     * authentication conversation is finished or not.
     *
     **/
    virtual StatusWith<bool> step(const StringData& inputData, std::string* outputData);

private:
    /**
     * Generates client-first-message.
     **/
    StatusWith<bool> _firstStep(std::string* outputData);

    /**
     * Parses server-first-message and generate client-final-message.
     **/
    StatusWith<bool> _secondStep(const std::vector<std::string>& input, std::string* outputData);

    /**
     * Generates client-first-message.
     **/
    StatusWith<bool> _thirdStep(const std::vector<std::string>& input, std::string* outputData);

    int _step;
    std::string _authMessage;
    unsigned char _saltedPassword[scram::hashSize];

    // client and server nonce concatenated
    std::string _clientNonce;
};

}  // namespace mongo
