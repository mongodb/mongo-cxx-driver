// Copyright 2016 MongoDB Inc.
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

#include <cstdint>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;

namespace test_util {

//
// Determines the max wire version associated with the given client, by running the "isMaster"
// command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
std::int32_t get_max_wire_version(const client& client);

//
// Determines whether or not the given client supports the collation feature, by running the
// "isMaster" command.
//
// Throws mongocxx::operation_exception if the operation fails, or the server reply is malformed.
//
bool supports_collation(const client& client);

}  // namespace test_util
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
