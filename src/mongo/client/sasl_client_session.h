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

#pragma once

#include <boost/scoped_array.hpp>
#include <string>

#include "mongo/base/disallow_copying.h"
#include "mongo/base/status.h"
#include "mongo/base/string_data.h"
#include "mongo/client/export_macros.h"
#include "mongo/stdx/functional.h"

namespace mongo {

    /**
     * Base class for the client side of a SASL authentication conversation.
     *
     * To use, create an instance, then use setParameter() to configure the authentication
     * parameters.  Once all parameters are set, call initialize() to initialize the client state
     * machine.  Finally, use repeated calls to step() to generate messages to send to the server
     * and process server responses.
     *
     * The required parameters vary by mechanism, but all mechanisms require parameterServiceName,
     * parameterServiceHostname, parameterMechanism and parameterUser.  All of the required
     * parameters must be UTF-8 encoded strings with no embedded NUL characters.  The
     * parameterPassword parameter is not constrained.
     */
    class SaslClientSession {
        MONGO_DISALLOW_COPYING(SaslClientSession);
    public:
        typedef stdx::function<SaslClientSession* (const std::string&)> SaslClientSessionFactoryFn;
        static SaslClientSessionFactoryFn create;

        /**
         * Identifiers of parameters used to configure a SaslClientSession.
         */
        enum Parameter {
            parameterServiceName = 0,
            parameterServiceHostname,
            parameterMechanism,
            parameterUser,
            parameterPassword,
            numParameters  // Must be last
        };

        SaslClientSession();
        virtual ~SaslClientSession();

        /**
         * Sets the parameter identified by "id" to "value".
         *
         * The value of "id" must be one of the legal values of Parameter less than numParameters.
         * May be called repeatedly for the same value of "id", with the last "value" replacing
         * previous values.
         *
         * The session object makes and owns a copy of the data in "value".
         */
        virtual void setParameter(Parameter id, const StringData& value);

        /**
         * Returns true if "id" identifies a parameter previously set by a call to setParameter().
         */
        virtual bool hasParameter(Parameter id);

        /**
         * Returns the value of a previously set parameter.
         *
         * If parameter "id" was never set, returns an empty StringData.  Note that a parameter may
         * be explicitly set to StringData(), so use hasParameter() to distinguish those cases.
         *
         * The session object owns the storage behind the returned StringData, which will remain
         * valid until setParameter() is called with the same value of "id", or the session object
         * goes out of scope.
         */
        virtual StringData getParameter(Parameter id);

        /**
         * Initializes a session for use.
         *
         * Call exactly once, after setting any parameters you intend to set via setParameter().
         */
        virtual Status initialize() = 0;

        /**
         * Takes one step of the SASL protocol on behalf of the client.
         *
         * Caller should provide data from the server side of the conversation in "inputData", or an
         * empty StringData() if none is available.  If the client should make a response to the
         * server, stores the response into "*outputData".
         *
         * Returns Status::OK() on success.  Any other return value indicates a failed
         * authentication, though the specific return value may provide insight into the cause of
         * the failure (e.g., ProtocolError, AuthenticationFailed).
         *
         * In the event that this method returns Status::OK(), consult the value of isDone() to
         * determine if the conversation has completed.  When step() returns Status::OK() and
         * isDone() returns true, authentication has completed successfully.
         */
        virtual Status step(const StringData& inputData, std::string* outputData) = 0;

        /**
         * Returns true if the authentication completed successfully.
         */
        virtual bool isDone() const = 0;

    private:
        /**
         * Buffer object that owns data for a single parameter.
         */
        struct DataBuffer {
            boost::scoped_array<char> data;
            size_t size;
        };

        /// Buffers for each of the settable parameters.
        DataBuffer _parameters[numParameters];
    };

}  // namespace mongo
