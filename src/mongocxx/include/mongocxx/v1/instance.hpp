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

#include <mongocxx/v1/instance-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

namespace mongocxx {
namespace v1 {

///
/// An instance of the MongoDB C++ Driver.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// All mongocxx API **MUST** be used within the lifetime of the instance object, with special exemptions listed  below.
/// Only **ONE** instance object may exist for the lifetime of a given process. It is undefined behavior to use  the
/// mongocxx API _before_ the instance object is initialized or _after_ the instance object is destroyed. It is
/// undefined behavior to create more than one instance object.
///
/// ```cpp
/// int main() {
///     // ...
///     // MUST NOT call mongocxx API before this point.
///     {
///         mongocxx::v1::instance instance;
///         // MAY call mongocxx API at this point.
///         // ...
///     }
///     // MUST NOT call mongocxx API after this point.
///     // ...
/// }
/// ```
///
/// @par Special exemptions
/// Only the following API are permitted to be used outside the lifetime of an instance object:
/// - @ref mongocxx::v1::logger
///
/// @see
/// - [Initialization and Cleanup (mongoc)](https://mongoc.org/libmongoc/current/init-cleanup.html)
///
class instance {};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::instance.
///
