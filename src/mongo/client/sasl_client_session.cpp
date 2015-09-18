/*    Copyright 2012 10gen Inc.
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

#include "mongo/client/sasl_client_session.h"

#include "mongo/util/assert_util.h"

namespace mongo {
SaslClientSession::SaslClientSessionFactoryFn SaslClientSession::create = NULL;

SaslClientSession::SaslClientSession() {}

SaslClientSession::~SaslClientSession() {}

void SaslClientSession::setParameter(Parameter id, const StringData& value) {
    fassert(16807, id >= 0 && id < numParameters);
    fassert(28583, value.size() < std::numeric_limits<std::size_t>::max());

    DataBuffer& buffer = _parameters[id];
    buffer.size = value.size();
    buffer.data.reset(new char[buffer.size + 1]);

    // Note that we append a terminal NUL to buffer.data, so it may be treated as a C-style
    // string.  This is required for parameterServiceName, parameterServiceHostname,
    // parameterMechanism and parameterUser.
    value.copyTo(buffer.data.get(), true);
}

bool SaslClientSession::hasParameter(Parameter id) {
    if (id < 0 || id >= numParameters)
        return false;
    return static_cast<bool>(_parameters[id].data);
}

StringData SaslClientSession::getParameter(Parameter id) {
    if (!hasParameter(id))
        return StringData();

    DataBuffer& buffer = _parameters[id];
    return StringData(buffer.data.get(), buffer.size);
}

}  // namespace mongo
