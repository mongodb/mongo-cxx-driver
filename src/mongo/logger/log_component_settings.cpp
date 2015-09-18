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

#include "mongo/logger/log_component_settings.h"

#include "mongo/util/assert_util.h"
#include "mongo/util/debug_util.h"

namespace mongo {
namespace logger {

LogComponentSettings::LogComponentSettings() {
    _minimumLoggedSeverity[LogComponent::kDefault] = char(LogSeverity::Log().toInt());

    for (int i = 0; i < int(LogComponent::kNumLogComponents); ++i) {
        _minimumLoggedSeverity[i] = _minimumLoggedSeverity[LogComponent::kDefault];
        _hasMinimumLoggedSeverity[i] = false;
    }

    _hasMinimumLoggedSeverity[LogComponent::kDefault] = true;
}

LogComponentSettings::~LogComponentSettings() {}

bool LogComponentSettings::hasMinimumLogSeverity(LogComponent component) const {
    dassert(int(component) >= 0 && int(component) < LogComponent::kNumLogComponents);
    return _hasMinimumLoggedSeverity[component];
}

LogSeverity LogComponentSettings::getMinimumLogSeverity(LogComponent component) const {
    dassert(int(component) >= 0 && int(component) < LogComponent::kNumLogComponents);
    return LogSeverity::cast(_minimumLoggedSeverity[component]);
}

void LogComponentSettings::setMinimumLoggedSeverity(LogComponent component, LogSeverity severity) {
    dassert(int(component) >= 0 && int(component) < LogComponent::kNumLogComponents);
    _minimumLoggedSeverity[component] = char(severity.toInt());
    _hasMinimumLoggedSeverity[component] = true;

    // Every unconfigured component will inherit log severity from parent.
    // Traversing the severity array once works because child components always
    // come after the parent in the LogComponent::Value enumeration.
    for (int i = 0; i < int(LogComponent::kNumLogComponents); ++i) {
        if (!_hasMinimumLoggedSeverity[i]) {
            LogComponent::Value v = LogComponent::Value(i);
            LogComponent parentComponent = LogComponent(v).parent();
            LogSeverity parentSeverity = getMinimumLogSeverity(parentComponent);
            _minimumLoggedSeverity[i] = char(parentSeverity.toInt());
        }
    }
}

void LogComponentSettings::clearMinimumLoggedSeverity(LogComponent component) {
    dassert(int(component) >= 0 && int(component) < LogComponent::kNumLogComponents);

    // LogComponent::kDefault must always be configured.
    if (component == LogComponent::kDefault) {
        setMinimumLoggedSeverity(component, LogSeverity::Log());
        return;
    }

    // Set unconfigured severity level to match LogComponent::kDefault.
    setMinimumLoggedSeverity(component, getMinimumLogSeverity(component.parent()));
    _hasMinimumLoggedSeverity[component] = false;
}

bool LogComponentSettings::shouldLog(LogComponent component, LogSeverity severity) const {
    dassert(int(component) >= 0 && int(component) < LogComponent::kNumLogComponents);

    // Should match parent component if minimum severity level is not configured for
    // component.
    dassert(_hasMinimumLoggedSeverity[component] ||
            _minimumLoggedSeverity[component] == _minimumLoggedSeverity[component.parent()]);

    return severity >= LogSeverity::cast(_minimumLoggedSeverity[component]);
}

}  // logger
}  // mongo
