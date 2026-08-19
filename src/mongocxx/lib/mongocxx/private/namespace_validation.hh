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

#include <bsoncxx/v1/stdx/string_view.hpp>

//

namespace mongocxx {

///
/// Return true when `name` is a valid database name.
///
/// A database name must not contain a "." (interpreted by the server as a namespace separator) or
/// a NUL character (which would truncate the name when it is converted to a C string).
///
inline bool is_valid_database_name(bsoncxx::v1::stdx::string_view name) {
    return name.find('.') == name.npos && name.find('\0') == name.npos;
}

///
/// Return true when `name` is a valid collection name.
///
/// A collection name must not contain a NUL character (which would truncate the name when it is
/// converted to a C string). Unlike a database name, a "." is permitted.
///
inline bool is_valid_collection_name(bsoncxx::v1::stdx::string_view name) {
    return name.find('\0') == name.npos;
}

} // namespace mongocxx
