// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/events/command_failed-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

namespace mongocxx {
namespace v1 {
namespace events {

///
/// `CommandFailedEvent` from the Command Logging and Monitoring specification.
///
/// @see
/// - [Command Logging and Monitoring (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/command-logging-and-monitoring/command-logging-and-monitoring/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class command_failed {};

} // namespace events
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::events::command_failed.
///
