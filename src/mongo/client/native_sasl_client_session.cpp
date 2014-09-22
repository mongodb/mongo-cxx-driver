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

#include "mongo/client/native_sasl_client_session.h"

#include "mongo/base/init.h"
#include "mongo/util/mongoutils/str.h"

namespace mongo {
namespace {

    SaslClientSession* createNativeSaslClientSession() {
        return new NativeSaslClientSession();
    }
     
    MONGO_INITIALIZER(NativeSaslClientContext)(InitializerContext* context) {
        SaslClientSession::create = createNativeSaslClientSession;
        return Status::OK();
    }

} // namespace

    NativeSaslClientSession::NativeSaslClientSession() :
            SaslClientSession(),
            _step(0),
            _done(false) {
    }

    NativeSaslClientSession::~NativeSaslClientSession() {}

    Status NativeSaslClientSession::initialize() {
        return Status(ErrorCodes::BadValue,
                      mongoutils::str::stream() << "SASL authentication not supported in client");
    }

    Status NativeSaslClientSession::step(const StringData& inputData, std::string* outputData) {
        return Status(ErrorCodes::BadValue,
                      mongoutils::str::stream() << "SASL authentication not supported in client");
    }
}  // namespace

MONGO_INITIALIZER_FUNCTION_ASSURE_FILE(client_native_sasl_client_session)
