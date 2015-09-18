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

#include "mongo/base/disallow_copying.h"
#include "mongo/logger/log_component.h"
#include "mongo/logger/log_severity.h"

namespace mongo {
namespace logger {

/**
 * Contains log severities for a list of log components.
 * kDefault always has a log severity defined but it is not necessary to
 * provide log severities for the other components (up to but not including kNumLogComponents).
 */
class LogComponentSettings {
    MONGO_DISALLOW_COPYING(LogComponentSettings);

public:
    LogComponentSettings();
    ~LogComponentSettings();

    /**
     * Returns true if a minimum log severity has been set for this component.
     * Used by log level commands to query component severity configuration.
     */
    bool hasMinimumLogSeverity(LogComponent component) const;

    /**
     * Gets the minimum log severity for component.
     * Result is defined only if hasMinimumLogSeverity() returns true for component.
     */
    LogSeverity getMinimumLogSeverity(LogComponent component) const;

    /**
     * Sets the minimum log severity for component.
     */
    void setMinimumLoggedSeverity(LogComponent component, LogSeverity severity);

    /**
     * Clears the minimum log severity for component.
     * For kDefault, severity level is initialized to default value.
     */
    void clearMinimumLoggedSeverity(LogComponent component);

    /**
     * Predicate that answers the question, "Should I, the caller, append to you, the log
     * domain, componented messages of the given severity?"  True means yes.
     *
     * If minimum severity levels are not configured, compare 'severity' against the configured
     * level for kDefault.
     */
    bool shouldLog(LogComponent component, LogSeverity severity) const;

private:
    // True if a log severity is explicitly set for a component.
    // This differentiates between unconfigured components and components that happen to have
    // the same severity as kDefault.
    // This is also used to update the severities of unconfigured components when the severity
    // for kDefault is modified.
    bool _hasMinimumLoggedSeverity[LogComponent::kNumLogComponents];

    // Log severities for components.
    // Store numerical values of severities to be cache-line friendly.
    // Set to kDefault minimum logged severity if _hasMinimumLoggedSeverity[i] is false.
    char _minimumLoggedSeverity[LogComponent::kNumLogComponents];
};

}  // namespace logger
}  // namespace mongo
