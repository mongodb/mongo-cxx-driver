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

#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "mongo/base/status.h"
#include "mongo/logger/appender.h"
#include "mongo/logger/log_severity.h"
#include "mongo/logger/logger.h"
#include "mongo/logger/message_log_domain.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
namespace logger {

    // Used for testing logging framework only.
    // TODO(schwerin): Have logger write to a different log from the global log, so that tests can
    // redirect their global log output for examination.
    template <typename MessageEventEncoder>
    class LogTest : public unittest::Test {
        friend class LogTestAppender;
    public:
        LogTest() : _severityOld(globalLogDomain()->getMinimumLogSeverity()) {
            globalLogDomain()->clearAppenders();
            _appenderHandle = globalLogDomain()->attachAppender(
                    MessageLogDomain::AppenderAutoPtr(new LogTestAppender(this)));
        }

        virtual ~LogTest() {
            globalLogDomain()->detachAppender(_appenderHandle);
            globalLogDomain()->setMinimumLoggedSeverity(_severityOld);
        }

    protected:
        std::vector<std::string> _logLines;
        LogSeverity _severityOld;

    private:
        class LogTestAppender : public MessageLogDomain::EventAppender {
        public:
            explicit LogTestAppender(LogTest* ltest) : _ltest(ltest) {}
            virtual ~LogTestAppender() {}
            virtual Status append(const MessageLogDomain::Event& event) {
                std::ostringstream _os;
                if (!_encoder.encode(event, _os))
                    return Status(ErrorCodes::LogWriteFailed, "Failed to append to LogTestAppender.");
                _ltest->_logLines.push_back(_os.str());
                return Status::OK();
            }

        private:
            LogTest *_ltest;
            MessageEventEncoder _encoder;
        };

        MessageLogDomain::AppenderHandle _appenderHandle;
    };

}  // namespace logger
}  // namespace mongo
