// Copyright 2015 MongoDB Inc.
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

#include <mongocxx/config/prelude.hpp>

#include <system_error>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Get the error_category for exceptions originating from the server.
///
/// @return The server error_category
///
const std::error_category& server_error_category();

///
/// Get the error_category for exceptions originating from the libmongoc library.
///
/// @return The implementation error_category
///
const std::error_category& implementation_error_category();

///
/// Get the error_category for inherent mongocxx exceptions.
///
/// @return The inherent error_category
///
const std::error_category& inherent_error_category();

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
