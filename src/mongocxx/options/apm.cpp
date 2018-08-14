// Copyright 2018-present MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mongocxx/options/apm.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

apm& apm::on_command_started(
    std::function<void(const mongocxx::events::command_started_event&)> command_started) {
    _command_started = std::move(command_started);
    return *this;
}

const std::function<void(const mongocxx::events::command_started_event&)>& apm::command_started()
    const {
    return _command_started;
}

apm& apm::on_command_failed(
    std::function<void(const mongocxx::events::command_failed_event&)> command_failed) {
    _command_failed = std::move(command_failed);
    return *this;
}

const std::function<void(const mongocxx::events::command_failed_event&)>& apm::command_failed()
    const {
    return _command_failed;
}

apm& apm::on_command_succeeded(
    std::function<void(const mongocxx::events::command_succeeded_event&)> command_succeeded) {
    _command_succeeded = std::move(command_succeeded);
    return *this;
}

const std::function<void(const mongocxx::events::command_succeeded_event&)>&
apm::command_succeeded() const {
    return _command_succeeded;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
