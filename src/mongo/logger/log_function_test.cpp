/*    Copyright 2013 10gen Inc.
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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kReplication

#include "mongo/platform/basic.h"

#include "mongo/logger/log_test.h"

#include <sstream>
#include <string>
#include <vector>

#include "mongo/logger/appender.h"
#include "mongo/logger/encoder.h"
#include "mongo/logger/log_component.h"
#include "mongo/logger/message_event_utf8_encoder.h"
#include "mongo/util/log.h"
#include "mongo/util/mongoutils/str.h"

using namespace mongo::logger;

namespace mongo {
namespace {

typedef LogTest<MessageEventDetailsEncoder> LogTestDetailsEncoder;

// Constants for log component test cases.
const LogComponent componentA = LogComponent::kCommand;
const LogComponent componentB = MONGO_LOG_DEFAULT_COMPONENT;

// Tests pass through of log component:
//     unconditional log functions -> LogStreamBuilder -> MessageEventEphemeral
//                                 -> MessageEventDetailsEncoder
// MONGO_DEFAULT_LOG_COMPONENT is set to kReplication before including util/log.h
// so non-debug logging without explicit component will log with kReplication instead
// of kDefault.
TEST_F(LogTestDetailsEncoder, LogFunctionsOverrideGlobalComponent) {
    // severe() - no component specified.
    severe() << "This is logged";
    ASSERT_TRUE(shouldLog(LogSeverity::Severe()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " F " << componentB.getNameForLog()),
                      std::string::npos);

    // severe() - with component.
    _logLines.clear();
    severe(componentA) << "This is logged";
    ASSERT_TRUE(logger::globalLogDomain()->shouldLog(componentA, LogSeverity::Severe()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " F " << componentA.getNameForLog()),
                      std::string::npos);

    // error() - no component specified.
    _logLines.clear();
    error() << "This is logged";
    ASSERT_TRUE(shouldLog(LogSeverity::Error()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " E " << componentB.getNameForLog()),
                      std::string::npos);

    // error() - with component.
    _logLines.clear();
    error(componentA) << "This is logged";
    ASSERT_TRUE(logger::globalLogDomain()->shouldLog(componentA, LogSeverity::Error()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " E " << componentA.getNameForLog()),
                      std::string::npos);

    // warning() - no component specified.
    _logLines.clear();
    warning() << "This is logged";
    ASSERT_TRUE(shouldLog(LogSeverity::Warning()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " W " << componentB.getNameForLog()),
                      std::string::npos);

    // warning() - with component.
    _logLines.clear();
    warning(componentA) << "This is logged";
    ASSERT_TRUE(logger::globalLogDomain()->shouldLog(componentA, LogSeverity::Warning()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " W " << componentA.getNameForLog()),
                      std::string::npos);

    // log() - no component specified.
    _logLines.clear();
    log() << "This is logged";
    ASSERT_TRUE(shouldLog(LogSeverity::Log()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " I " << componentB.getNameForLog()),
                      std::string::npos);

    // log() - with component.
    _logLines.clear();
    log(componentA) << "This is logged";
    ASSERT_TRUE(logger::globalLogDomain()->shouldLog(componentA, LogSeverity::Log()));
    ASSERT_EQUALS(1U, _logLines.size());
    ASSERT_NOT_EQUALS(_logLines[0].find(str::stream() << " I " << componentA.getNameForLog()),
                      std::string::npos);
}

}  // namespace
}  // namespace mongo
