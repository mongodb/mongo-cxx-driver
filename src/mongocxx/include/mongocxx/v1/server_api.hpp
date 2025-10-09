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

#include <mongocxx/v1/server_api-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>
#include <string>
#include <system_error>
#include <type_traits>

namespace mongocxx {
namespace v1 {

///
/// Options related to MongoDB Stable API configuration.
///
/// Supported fields include:
/// - `version`
/// - `strict`
/// - `deprecation_errors` ("deprecationErrors")
///
/// @see
/// - [Stable API (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/stable-api/)
///
class server_api {
    // This class implements `ServerApi`:
    //  - https://specifications.readthedocs.io/en/latest/versioned-api/versioned-api/

   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// The server API version.
    ///
    /// @note Not to be confused with the MongoDB C++ Driver's API or ABI version.
    ///
    enum class version {
        ///
        /// Stable API Version 1.
        ///
        k_version_1,
    };

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~server_api();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() server_api(server_api&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) operator=(server_api&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() server_api(server_api const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) operator=(server_api const& other);

    ///
    /// Initialize with the given server API version.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() server_api(version v);

    ///
    /// Return the given server API version as a string.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::server_api::errc::invalid_version when `v` is not a
    /// valid server API version.
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::string) version_to_string(version v);

    ///
    /// Return the given server API version as an enumerator.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::server_api::errc::invalid_version when `v` is not a
    /// valid server API version.
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(version) version_from_string(bsoncxx::v1::stdx::string_view v);

    ///
    /// Set the "strict" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) strict(bool strict);

    ///
    /// Return the current "strict" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) strict() const;

    ///
    /// Set the "deprecationErrors" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) deprecation_errors(bool v);

    ///
    /// Return the current "deprecationErrors" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) deprecation_errors() const;

    ///
    /// Return the current "version" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(version) get_version() const;

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::server_api.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,            ///< Zero.
        invalid_version, ///< The server API version is invalid.
    };

    ///
    /// The error category for @ref mongocxx::v1::server_api::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::server_api::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::server_api.
///
