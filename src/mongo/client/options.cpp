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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kDefault;

#include "mongo/platform/basic.h"

#include "mongo/client/private/options.h"
#include "mongo/client/options.h"
#include "mongo/util/log.h"

namespace mongo {
namespace client {

namespace {
// The unique global options instance.
Options options;
}  // namespace

#if !defined(_MSC_EXTENSIONS)
const int Options::kDefaultDefaultLocalThresholdMillis;
const unsigned int Options::kDefaultAutoShutdownGracePeriodMillis;
#endif

void setOptions(const Options& newOptions) {
    options = newOptions;
}

const Options& Options::current() {
    return options;
}

Options::Options()
    : _callShutdownAtExit(false),
      _autoShutdownGracePeriodMillis(kDefaultAutoShutdownGracePeriodMillis),
      _sslMode(kSSLDisabled),
      _useFIPSMode(false),
      _sslAllowInvalidCertificates(false),
      _sslAllowInvalidHostnames(false),
      _defaultLocalThresholdMillis(kDefaultDefaultLocalThresholdMillis),
      _minLoggedSeverity(logger::LogSeverity::Log()),
      _IPv6Enabled(false),
      _validateObjects(false) {}

Options& Options::setCallShutdownAtExit(bool value) {
    _callShutdownAtExit = value;
    return *this;
}

bool Options::callShutdownAtExit() const {
    return _callShutdownAtExit;
}

Options& Options::setAutoShutdownGracePeriodMillis(unsigned int millis) {
    _autoShutdownGracePeriodMillis = millis;
    return *this;
}

unsigned int Options::autoShutdownGracePeriodMillis() const {
    return _autoShutdownGracePeriodMillis;
}

Options& Options::setDefaultLocalThresholdMillis(int millis) {
    _defaultLocalThresholdMillis = millis;
    return *this;
}

int Options::defaultLocalThresholdMillis() const {
    return _defaultLocalThresholdMillis;
}

Options& Options::setSSLMode(SSLModes sslMode) {
    _sslMode = sslMode;
    return *this;
}

Options::SSLModes Options::SSLMode() const {
    return _sslMode;
}

Options& Options::setFIPSMode(bool value) {
    _useFIPSMode = value;
    return *this;
}

const bool Options::FIPSMode() const {
    return _useFIPSMode;
}

Options& Options::setSSLDisabledTLSProtocols(const std::vector<TLSProtocol>& protocols) {
    _sslDisabledTLSProtocols = protocols;
    return *this;
}

const std::vector<Options::TLSProtocol>& Options::SSLDisabledTLSProtocols() const {
    return _sslDisabledTLSProtocols;
}

Options& Options::setSSLCAFile(const std::string& fileName) {
    _sslCAFile = fileName;
    return *this;
}

const std::string& Options::SSLCAFile() const {
    return _sslCAFile;
}

Options& Options::setSSLPEMKeyFile(const std::string& fileName) {
    _sslPEMKeyFile = fileName;
    return *this;
}

const std::string& Options::SSLPEMKeyFile() const {
    return _sslPEMKeyFile;
}

Options& Options::setSSLPEMKeyPassword(const std::string& password) {
    _sslPEMKeyPassword = password;
    return *this;
}

const std::string& Options::SSLPEMKeyPassword() const {
    return _sslPEMKeyPassword;
}

Options& Options::setSSLCRLFile(const std::string& fileName) {
    _sslCRLFile = fileName;
    return *this;
}

const std::string& Options::SSLCRLFile() const {
    return _sslCRLFile;
}

Options& Options::setSSLAllowInvalidCertificates(bool value) {
    _sslAllowInvalidCertificates = value;
    return *this;
}

const bool Options::SSLAllowInvalidCertificates() const {
    return _sslAllowInvalidCertificates;
}

Options& Options::setSSLAllowInvalidHostnames(bool value) {
    _sslAllowInvalidHostnames = value;
    return *this;
}

const bool Options::SSLAllowInvalidHostnames() const {
    return _sslAllowInvalidHostnames;
}

Options& Options::setSSLCipherConfig(const std::string& config) {
    _sslCipherConfig = config;
    return *this;
}

const std::string& Options::SSLCipherConfig() const {
    return _sslCipherConfig;
}

Options& Options::setLogAppenderFactory(const Options::LogAppenderFactory& factory) {
    _appenderFactory = factory;
    return *this;
}

const Options::LogAppenderFactory& Options::logAppenderFactory() const {
    return _appenderFactory;
}

Options& Options::setMinLoggedSeverity(logger::LogSeverity level) {
    _minLoggedSeverity = level;
    return *this;
}

logger::LogSeverity Options::minLoggedSeverity() const {
    return _minLoggedSeverity;
}

Options& Options::setIPv6Enabled(bool state) {
    _IPv6Enabled = state;
    return *this;
}

bool Options::IPv6Enabled() const {
    return _IPv6Enabled;
}

Options& Options::setValidateObjects(bool value) {
    _validateObjects = value;
    return *this;
}

bool Options::validateObjects() const {
    return _validateObjects;
}

}  // namespace client
}  // namespace mongo
