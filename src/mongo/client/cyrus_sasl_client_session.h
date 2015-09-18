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

#include "mongo/client/sasl_client_session.h"

#include <sasl/sasl.h>

namespace mongo {

/**
 * Implementation of the client side of a SASL authentication conversation.
 * using the Cyrus SASL library.
 */
class CyrusSaslClientSession : public SaslClientSession {
    MONGO_DISALLOW_COPYING(CyrusSaslClientSession);

public:
    CyrusSaslClientSession();
    ~CyrusSaslClientSession();

    /**
     * Overriding to store the password data in sasl_secret_t format
     */
    virtual void setParameter(Parameter id, const StringData& value);

    /**
     * Returns the value of the parameterPassword parameter in the form of a sasl_secret_t, used
     * by the Cyrus SASL library's SASL_CB_PASS callback.  The session object owns the storage
     * referenced by the returned sasl_secret_t*, which will remain in scope according to the
     * same rules as given for SaslClientSession::getParameter().
     */
    sasl_secret_t* getPasswordAsSecret();

    virtual Status initialize();

    virtual Status step(const StringData& inputData, std::string* outputData);

    virtual bool isDone() const {
        return _done;
    }

private:
    /// Maximum number of Cyrus SASL callbacks stored in _callbacks.
    static const int maxCallbacks = 4;

    /// Underlying Cyrus SASL library connection object.
    sasl_conn_t* _saslConnection;

    // Number of successfully completed conversation steps.
    int _step;

    /// See isDone().
    bool _done;

    /// Stored of password in sasl_secret_t format
    boost::scoped_array<char> _secret;

    /// Callbacks registered on _saslConnection for providing the Cyrus SASL library with
    /// parameter values, etc.
    sasl_callback_t _callbacks[maxCallbacks];
};

}  // namespace mongo
