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

#include "mongo/config.h"

INSTALL_FUNCTION(base_init) // "default"
INSTALL_FUNCTION(bson_oid) // "OIDGeneration"
INSTALL_FUNCTION(logger_logstream_builder) // "LogstreamBuilder"
INSTALL_FUNCTION(logger_logger) // "GlobalLogManager"
#ifdef MONGO_SSL
INSTALL_FUNCTION(client_sasl_client_authenticate_impl) // "SaslClientAuthenticateFunction"
INSTALL_FUNCTION(client_native_sasl_client_session) // "NativeSaslClientContext"
#endif
#ifdef MONGO_SASL
INSTALL_FUNCTION(client_cyrus_sasl_client_session) // "CyrusSaslAllocatorsAndMutexes,CyrusSaslClientContext"
INSTALL_FUNCTION(client_sasl_sspi) // "SaslSspiClientPlugin,SaslCramClientPlugin,SaslPlainClientPlugin"
#endif
INSTALL_FUNCTION(util_fail_point_service) // "FailPointRegistry,AllFailPointsRegistered"
INSTALL_FUNCTION(util_net_socket_poll) // "DynamicLinkWin32Poll"
INSTALL_FUNCTION(util_net_ssl_manager) // "SSLManager"
INSTALL_FUNCTION(util_net_sock) // "SockWSAStartup"
INSTALL_FUNCTION(util_time_support) // "Init32TimeSupport"
