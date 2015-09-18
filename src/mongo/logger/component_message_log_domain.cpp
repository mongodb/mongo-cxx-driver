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

#include "mongo/logger/component_message_log_domain.h"

namespace mongo {
namespace logger {

ComponentMessageLogDomain::ComponentMessageLogDomain() {}

ComponentMessageLogDomain::~ComponentMessageLogDomain() {}

bool ComponentMessageLogDomain::hasMinimumLogSeverity(LogComponent component) const {
    return _settings.hasMinimumLogSeverity(component);
}

bool ComponentMessageLogDomain::shouldLog(LogComponent component, LogSeverity severity) const {
    return _settings.shouldLog(component, severity);
}

bool ComponentMessageLogDomain::shouldLog(LogComponent component1,
                                          LogComponent component2,
                                          LogSeverity severity) const {
    return _settings.shouldLog(component1, severity) || _settings.shouldLog(component2, severity);
}

bool ComponentMessageLogDomain::shouldLog(LogComponent component1,
                                          LogComponent component2,
                                          LogComponent component3,
                                          LogSeverity severity) const {
    return _settings.shouldLog(component1, severity) || _settings.shouldLog(component2, severity) ||
        _settings.shouldLog(component3, severity);
}

LogSeverity ComponentMessageLogDomain::getMinimumLogSeverity() const {
    return _settings.getMinimumLogSeverity(LogComponent::kDefault);
}

LogSeverity ComponentMessageLogDomain::getMinimumLogSeverity(LogComponent component) const {
    return _settings.getMinimumLogSeverity(component);
}

void ComponentMessageLogDomain::setMinimumLoggedSeverity(LogSeverity severity) {
    _settings.setMinimumLoggedSeverity(LogComponent::kDefault, severity);
}

void ComponentMessageLogDomain::setMinimumLoggedSeverity(LogComponent component,
                                                         LogSeverity severity) {
    _settings.setMinimumLoggedSeverity(component, severity);
}

void ComponentMessageLogDomain::clearMinimumLoggedSeverity(LogComponent component) {
    _settings.clearMinimumLoggedSeverity(component);
}

}  // namespace logger
}  // namespace mongo
