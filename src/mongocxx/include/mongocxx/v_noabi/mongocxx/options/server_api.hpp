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

#include <string>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/options/server_api-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Class representing options for server API.
///
/// @see
/// - [Stable API (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/stable-api/)
///
class server_api {
   public:
    ///
    /// Enum representing the possible values for server API version.
    ///
    enum class version {
        k_version_1,  ///< Stable API Version 1.
    };

    ///
    /// Constructs a new server_api object.
    ///
    /// The specified API version will be sent to the server. This will cause
    /// the server to behave in a manner compatible with that API version.
    /// The driver will behave in a manner compatible with a server configured
    /// with that API version, regardless of the server's actual release version.
    ///
    /// @param version
    ///   The server api version to send to the server.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() server_api(version version);

    ///
    /// Converts a version enum value to its string value.
    ///
    /// @param version
    ///   The enum value to convert to a string.
    ///
    /// @throws mongocxx::v_noabi::logic_error on an invalid argument
    ///
    /// @return
    ///   The string value of the given enum value.
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::string) version_to_string(version version);

    ///
    /// Converts a version string to its enum value.
    ///
    /// @param version
    ///   The string to convert to an enum value.
    ///
    /// @throws mongocxx::v_noabi::logic_error on an invalid argument
    ///
    /// @return
    ///   The enum value of the given string.
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(version)
        version_from_string(bsoncxx::v_noabi::stdx::string_view version);

    ///
    /// Sets the strict option, specifying whether the server should return
    /// errors for features that are not part of the declared API version.
    ///
    /// @param strict
    ///   The value to set strict to.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) strict(bool strict);

    ///
    /// Gets the current value of the strict option.
    ///
    /// @return
    ///   The optional value of the strict option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) strict() const;

    ///
    /// Sets the deprecation errors option, specifying whether the server should
    /// return errors for features that are deprecated in the declared API version.
    ///
    /// @param deprecation_errors
    ///   The value to set deprecation errors to.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(server_api&) deprecation_errors(bool deprecation_errors);

    ///
    /// Gets the current value of the deprecation errors option.
    ///
    /// @return
    ///   The optional value of the deprecation errors option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&)
    deprecation_errors() const;

    ///
    /// Gets the declared server api version.
    ///
    /// @return
    ///   The version enum value specifying the declared server api version.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(version) get_version() const;

   private:
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::pool;

    version _version;
    bsoncxx::v_noabi::stdx::optional<bool> _strict;
    bsoncxx::v_noabi::stdx::optional<bool> _deprecation_errors;
};

}  // namespace options
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::server_api.
///
