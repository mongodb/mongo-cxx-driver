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

#include <bsoncxx/v1/oid.hpp> // IWYU pragma: export

//

#include <string>

#include <catch2/catch_tostring.hpp>

CATCH_REGISTER_ENUM(
    bsoncxx::v1::oid::errc,
    bsoncxx::v1::oid::errc::zero,
    bsoncxx::v1::oid::errc::null_bytes_ptr,
    bsoncxx::v1::oid::errc::invalid_length,
    bsoncxx::v1::oid::errc::empty_string,
    bsoncxx::v1::oid::errc::invalid_string)

namespace Catch {

template <>
struct StringMaker<bsoncxx::v1::oid> {
    static std::string convert(bsoncxx::v1::oid const& value) {
        return value.to_string();
    }
};

} // namespace Catch
