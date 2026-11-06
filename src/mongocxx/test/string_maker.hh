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

#include <bsoncxx/test/string_maker.hh> // IWYU pragma: export

//

#include <mongocxx/v1/instance-fwd.hpp>

#include <bsoncxx/v1/config/export.hpp>

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/logger.hpp>
#include <mongocxx/v1/read_concern.hpp>
#include <mongocxx/v1/read_preference.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <string>

#include <mongocxx/private/scoped_bson-fwd.hh>

CATCH_REGISTER_ENUM(
    mongocxx::v1::source_errc,
    mongocxx::v1::source_errc::zero,
    mongocxx::v1::source_errc::mongocxx,
    mongocxx::v1::source_errc::mongoc,
    mongocxx::v1::source_errc::mongocrypt,
    mongocxx::v1::source_errc::server)

CATCH_REGISTER_ENUM(
    mongocxx::v1::type_errc,
    mongocxx::v1::type_errc::zero,
    mongocxx::v1::type_errc::invalid_argument,
    mongocxx::v1::type_errc::runtime_error)

CATCH_REGISTER_ENUM(
    mongocxx::v1::log_level,
    mongocxx::v1::log_level::k_error,
    mongocxx::v1::log_level::k_critical,
    mongocxx::v1::log_level::k_warning,
    mongocxx::v1::log_level::k_message,
    mongocxx::v1::log_level::k_info,
    mongocxx::v1::log_level::k_debug,
    mongocxx::v1::log_level::k_trace)

CATCH_REGISTER_ENUM(
    mongocxx::v1::read_concern::level,
    mongocxx::v1::read_concern::level::k_local,
    mongocxx::v1::read_concern::level::k_majority,
    mongocxx::v1::read_concern::level::k_linearizable,
    mongocxx::v1::read_concern::level::k_server_default,
    mongocxx::v1::read_concern::level::k_unknown,
    mongocxx::v1::read_concern::level::k_available,
    mongocxx::v1::read_concern::level::k_snapshot)

CATCH_REGISTER_ENUM(
    mongocxx::v1::read_preference::read_mode,
    mongocxx::v1::read_preference::read_mode::k_primary,
    mongocxx::v1::read_preference::read_mode::k_primary_preferred,
    mongocxx::v1::read_preference::read_mode::k_secondary,
    mongocxx::v1::read_preference::read_mode::k_secondary_preferred,
    mongocxx::v1::read_preference::read_mode::k_nearest)

CATCH_REGISTER_ENUM(
    mongocxx::v1::write_concern::level,
    mongocxx::v1::write_concern::level::k_default,
    mongocxx::v1::write_concern::level::k_majority,
    mongocxx::v1::write_concern::level::k_tag,
    mongocxx::v1::write_concern::level::k_unacknowledged,
    mongocxx::v1::write_concern::level::k_acknowledged,
    mongocxx::v1::write_concern::level::k_unknown)

namespace Catch {

// No point specializing StringMaker for the abstract class mongocxx::v1::logger.

template <>
struct StringMaker<mongocxx::scoped_bson> {
    static std::string BSONCXX_ABI_CDECL convert(mongocxx::scoped_bson const& value);
};

template <>
struct StringMaker<mongocxx::v1::instance> {
    static std::string convert(mongocxx::v1::instance const&) {
        return "mongocxx::v1::instance";
    }
};

} // namespace Catch
