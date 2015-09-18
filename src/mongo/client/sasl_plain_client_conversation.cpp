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

#include "mongo/platform/basic.h"

#include "mongo/client/sasl_plain_client_conversation.h"

#include "mongo/base/status_with.h"
#include "mongo/bson/util/builder.h"
#include "mongo/client/sasl_client_session.h"
#include "mongo/util/password_digest.h"

namespace mongo {

SaslPLAINClientConversation::SaslPLAINClientConversation(SaslClientSession* saslClientSession)
    : SaslClientConversation(saslClientSession) {}

SaslPLAINClientConversation::~SaslPLAINClientConversation(){};

StatusWith<bool> SaslPLAINClientConversation::step(const StringData& inputData,
                                                   std::string* outputData) {
    // Create PLAIN message on the form: user\0user\0pwd

    StringBuilder sb;
    sb << _saslClientSession->getParameter(SaslClientSession::parameterUser).toString() << '\0'
       << _saslClientSession->getParameter(SaslClientSession::parameterUser).toString() << '\0'
       << _saslClientSession->getParameter(SaslClientSession::parameterPassword).toString();

    *outputData = sb.str();

    return StatusWith<bool>(true);
}

}  // namespace mongo
