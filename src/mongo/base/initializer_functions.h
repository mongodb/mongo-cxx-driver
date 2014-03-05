INSTALL_FUNCTION(base_init) // "default"
INSTALL_FUNCTION(platform_backtrace) // "SolarisBacktrace"
INSTALL_FUNCTION(logger_logstream_builder) // "LogstreamBuilder"
INSTALL_FUNCTION(logger_ramlog) // "RamLogCatalog"
INSTALL_FUNCTION(logger_logger) // "GlobalLogManager"
#ifdef MONGO_SASL
INSTALL_FUNCTION(client_sasl_client_authenticate_impl) // "SaslClientAuthenticateFunction"
INSTALL_FUNCTION(client_sasl_client_session) // "CyrusSaslAllocatorsAndMutexes,SaslClientContext"
INSTALL_FUNCTION(client_sasl_sspi) // "SaslSspiClientPlugin,SaslCramClientPlugin,SaslPlainClientPlugin"
#endif
INSTALL_FUNCTION(util_fail_point_service) // "FailPointRegistry,AllFailPointsRegistered"
INSTALL_FUNCTION(util_net_socket_poll) // "DynamicLinkWin32Poll"
INSTALL_FUNCTION(util_net_ssl_manager) // "SSLManager"
