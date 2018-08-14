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

#pragma once

#include <functional>

#include <mongocxx/events/command_failed_event.hpp>
#include <mongocxx/events/command_started_event.hpp>
#include <mongocxx/events/command_succeeded_event.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

class MONGOCXX_API apm {
   public:
    ///
    /// Set the command started monitoring callback. The callback takes a reference to a
    /// command_started_event which will only contain valid data for the duration of the callback.
    ///
    /// @param command_started
    ///   The command started monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    apm& on_command_started(
        std::function<void(const mongocxx::events::command_started_event&)> command_started);

    ///
    /// Retrieves the command started monitoring callback.
    ///
    /// @return The command started monitoring callback.
    ///
    const std::function<void(const mongocxx::events::command_started_event&)>& command_started()
        const;

    ///
    /// Set the command failed monitoring callback. The callback takes a reference to a
    /// command_failed_event which will only contain valid data for the duration of the callback.
    ///
    /// @param command_failed
    ///   The command failed monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    apm& on_command_failed(
        std::function<void(const mongocxx::events::command_failed_event&)> command_failed);

    ///
    /// Retrieves the command failed monitoring callback.
    ///
    /// @return The command failed monitoring callback.
    ///
    const std::function<void(const mongocxx::events::command_failed_event&)>& command_failed()
        const;

    ///
    /// Set the command succeeded monitoring callback. The callback takes a reference to a
    /// command_succeeded_event which will only contain valid data for the duration of the callback.
    ///
    /// @param command_failed
    ///   The command succeeded monitoring callback.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    apm& on_command_succeeded(
        std::function<void(const mongocxx::events::command_succeeded_event&)> command_succeeded);

    ///
    /// Retrieves the command succeeded monitoring callback.
    ///
    /// @return The command succeeded monitoring callback.
    ///
    const std::function<void(const mongocxx::events::command_succeeded_event&)>& command_succeeded()
        const;

   private:
    std::function<void(const mongocxx::events::command_started_event&)> _command_started;
    std::function<void(const mongocxx::events::command_failed_event&)> _command_failed;
    std::function<void(const mongocxx::events::command_succeeded_event&)> _command_succeeded;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
