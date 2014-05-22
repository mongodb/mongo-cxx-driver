/*    Copyright 2014 10gen Inc.
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

#include "mongo/client/export_macros.h"

namespace mongo {
namespace client {

    /** The Options structure is passed to mongo::client::initialize to configure various
     *  properties and configurations of the driver.
     */

    class MONGO_CLIENT_API Options {
    public:

        // Defaults for non-boolean or std::string parameters that are not defaulted to the
        // empty string. These are useful in case you which to set a parameter to a scale
        // factor or mutation of the default.
        static const int kDefaultAutoShutdownGracePeriodMillis = 250;
        static const int kDefaultDefaultLocalThresholdMillis = 15;

        /** Obtains the currently configured options for the driver. This method
         *  must not be called before mongo::client::initialize has completed.
         */
        static const Options& current();

        /** Constructs a default options object with default values for all options. The
         *  default option values are documented with each mutator.
         */
        Options();

        /** The default ports where different mongodb servers tend to run */
        enum DefaultPorts {
            kDbServer = 27017,
            kShardServer = 27018,
            kConfigServer = 27019
        };

        /** The possible modes for SSL support in an SSL enabled build of the driver. */
        enum SSLModes {

            /** Don't attempt to make SSL connections, or require SSL support of the server. */
            kSSLDisabled,

            /** Attempt SSL connections, but fall back to no SSL if server does not support.
             *
             *  NOTE: The driver does not currently offer this mode.
             */

            // kSSLPreferred,

            /** Require SLL. */
            kSSLRequired
        };


        //
        // Startup and shutdown
        //

        /** If true, the driver will automatically schedule a client::shutdown via
         *  'std::atexit'. If false, the user of the library is responsible for calling
         *  'client::shutdown'.
         *
         *  Default: true
         */
        Options& setCallShutdownAtExit(bool value = true);
        bool callShutdownAtExit() const;

        /** The grace period used when calling client::shutdown from atexit. If
         * 'callShutdownAtExit' is false, this parameter has no effect.
         *
         *  Default: 250 ms
         */
        Options& setAutoShutdownGracePeriodMillis(int millis);
        int autoShutdownGracePeriodMillis() const;


        //
        // Replication
        //

        /** Set the default threshold to consider a node local.
         *
         *  Default: 15 ms
         */
        Options& setDefaultLocalThresholdMillis(int millis);
        int defaultLocalThresholdMillis() const;


        //
        // SSL
        //
        // NOTE: None of the below settings have any effect unless the driver
        // was compiled with SSL support.
        //
        // Glossary:
        //  - CA File: certificate authority certificate file
        //  - PEM Key File: SSL certificate file in PEM format
        //  - CRL: certificate revocation list
        //  - FIPS Mode: OpenSSL crypto library FIPS 140-2 mode processing
        //

        /** If set to kSSLRequired, the driver will use SSL when connecting.
         *
         *  Default: kSSLDisabled
         */
        Options& setSSLMode(SSLModes sslMode = kSSLRequired);
        SSLModes SSLMode() const;

        /** A convenience: returns true if SSL is not disabled (preferred or required). */
        inline bool SSLEnabled() const {
            return SSLMode() != kSSLDisabled;
        }

        /** Sets whether to operate in FIPS mode.
         *
         *  Default: false
         */
        Options& setFIPSMode(bool value = true);
        const bool FIPSMode() const;

        /** Configure the SSL CA file to use. Has no effect if 'useSSL' is false.
         *
         *  Default: false
         */
        Options& setSSLCAFile(const std::string& fileName);
        const std::string& SSLCAFile() const;

        /** Configure the SSL PEM key file to use. Has no effect if 'useSSL' is false.
         *
         *  Default: <empty>
         */
        Options& setSSLPEMKeyFile(const std::string& fileName);
        const std::string& SSLPEMKeyFile() const;

        /** Configure the SSL PEM key password. Has no effect if 'useSSL' is false.
         *
         *  Default: <empty>
         */
        Options& setSSLPEMKeyPassword(const std::string& password);
        const std::string& SSLPEMKeyPassword() const;

        /** Configure the SSL CRL file to use. Has no effect if 'useSSL' is false.
         *
         *  Default: <empty>
         */
        Options& setSSLCRLFile(const std::string& fileName);
        const std::string& SSLCRLFile() const;

        /** When set true, SSL certificate validation is disabled.
         *
         *  Default: false
         */
        Options& setSSLAllowInvalidCertificates(bool value = true);
        const bool SSLAllowInvalidCertificates() const;


        //
        // Misc
        //

        /** Configure whether BSON objects returned from the server should be validated.
         *
         *  Default: false
         */
        Options& setValidateObjects(bool value = true);
        bool validateObjects() const;

    private:
        bool _callShutdownAtExit;
        int _autoShutdownGracePeriodMillis;
        SSLModes _sslMode;
        bool _useFIPSMode;
        std::string _sslCAFile;
        std::string _sslPEMKeyFile;
        std::string _sslPEMKeyPassword;
        std::string _sslCRLFile;
        bool _sslAllowInvalidCertificates;
        int _defaultLocalThresholdMillis;
        bool _validateObjects;
    };

} // namespace client
} // namespace mongo
