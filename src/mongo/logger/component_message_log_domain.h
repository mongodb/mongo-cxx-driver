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

#pragma once

#include "mongo/logger/log_component_settings.h"
#include "mongo/logger/message_log_domain.h"

namespace mongo {
namespace logger {

/**
 * Logging domain for ephemeral messages with minimum severity.
 */
class MONGO_CLIENT_API ComponentMessageLogDomain : public MessageLogDomain {
    MONGO_DISALLOW_COPYING(ComponentMessageLogDomain);

public:
    ComponentMessageLogDomain();

    ~ComponentMessageLogDomain();

    /**
     * Predicate that answers the questwion, "Should I, the caller, append to you, the log
     * domain, messages of the given severity?"  True means yes.
     */
    bool shouldLog(LogComponent component, LogSeverity severity) const;
    bool shouldLog(LogComponent component1, LogComponent component2, LogSeverity severity) const;
    bool shouldLog(LogComponent component1,
                   LogComponent component2,
                   LogComponent component3,
                   LogSeverity severity) const;

    /**
     * Returns true if a minimum log severity has been set for this component.
     * Called by log level commands to query component severity configuration.
     */
    bool hasMinimumLogSeverity(LogComponent component) const;

    /**
     * Gets the minimum severity of messages that should be sent to this LogDomain.
     */
    LogSeverity getMinimumLogSeverity() const;
    LogSeverity getMinimumLogSeverity(LogComponent component) const;

    /**
     * Sets the minimum severity of messages that should be sent to this LogDomain.
     */
    void setMinimumLoggedSeverity(LogSeverity severity);
    void setMinimumLoggedSeverity(LogComponent, LogSeverity severity);

    /**
     * Clears the minimum log severity for component.
     * For kDefault, severity level is initialized to default value.
     */
    void clearMinimumLoggedSeverity(LogComponent component);

private:
    LogComponentSettings _settings;
};

}  // namespace logger
}  // namespace mongo
