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

#include <mongocxx/options/server_api-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/server_api.hpp> // IWYU pragma: export

#include <string>

#include <mongocxx/client-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/pool-fwd.hpp>   // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::client::server_api_opts.
///
/// @see
/// - [Stable API (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/stable-api/)
///
class server_api {
   public:
    ///
    /// Enum representing the possible values for server API version.
    ///
    using version = v1::server_api::version;

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
    /* explicit(false) */ server_api(version version) : _version{version} {}

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ server_api(v1::server_api const& opts) : server_api{opts.get_version()} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::server_api() const {
        return v1::server_api{_version};
    }

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
    static MONGOCXX_ABI_EXPORT_CDECL(version) version_from_string(bsoncxx::v_noabi::stdx::string_view version);

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
    server_api& strict(bool strict) {
        _strict = strict;
        return *this;
    }

    ///
    /// Gets the current value of the strict option.
    ///
    /// @return
    ///   The optional value of the strict option.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& strict() const {
        return _strict;
    }

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
    server_api& deprecation_errors(bool deprecation_errors) {
        _deprecation_errors = deprecation_errors;
        return *this;
    }

    ///
    /// Gets the current value of the deprecation errors option.
    ///
    /// @return
    ///   The optional value of the deprecation errors option.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& deprecation_errors() const {
        return _deprecation_errors;
    }

    ///
    /// Gets the declared server api version.
    ///
    /// @return
    ///   The version enum value specifying the declared server api version.
    ///
    version get_version() const {
        return _version;
    }

    class internal;

   private:
    version _version;
    bsoncxx::v_noabi::stdx::optional<bool> _strict;
    bsoncxx::v_noabi::stdx::optional<bool> _deprecation_errors;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::server_api from_v1(v1::server_api const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::server_api to_v1(v_noabi::options::server_api const& v) {
    return v1::server_api{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::server_api.
///
/// @par Includes
/// - @ref mongocxx/v1/server_api.hpp
///
