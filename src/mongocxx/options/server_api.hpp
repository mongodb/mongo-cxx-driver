// Copyright 2020 MongoDB Inc.
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

#include <string>

#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;
class pool;

namespace options {

///
/// Class representing options for server API.
///
class MONGOCXX_API server_api {
   public:
    ///
    /// Enum representing the possible values for server API version.
    ///
    enum class version { version_1 };

    ///
    /// Converts a version enum value to its string value.
    ///
    /// @param version
    ///   The enum value to convert to a string.
    ///
    /// @return
    ///   The string value of the given enum value.
    ///
    static std::string version_to_string(version version);

    ///
    /// Converts a version string to its enum value.
    ///
    /// @param version
    ///   The string to convert to an enum value.
    ///
    /// @return
    ///   The enum value of the given string.
    ///
    static version version_from_string(std::string version);

    ///
    /// Constructs a new server_api object.
    ///
    /// The specified API version will be sent to the server. This will cause
    /// the server to behave in a manner compatible with that API version.
    /// It also causes the driver to behave in a manner compatible with the
    /// driver’s behavior as of the release when the driver first started
    /// to support the specified server API version.
    ///
    /// @param version
    ///   The server api version to send to the server.
    ///
    server_api(version version);

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
    server_api& strict(bool strict);

    ///
    /// Gets the current value of the strict option.
    ///
    /// @return
    ///   The optional value of the strict option.
    ///
    const stdx::optional<bool>& strict() const;

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
    server_api& deprecation_errors(bool deprecation_errors);

    ///
    /// Gets the current value of the deprecation errors option.
    ///
    /// @return
    ///   The optional value of the deprecation errors option.
    ///
    const stdx::optional<bool>& deprecation_errors() const;

    ///
    /// Gets the declared server api version.
    ///
    /// @return
    ///   The version enum value specifying the declared server api version.
    ///
    version api_version() const;

   private:
    friend class mongocxx::client;
    friend class mongocxx::pool;

    MONGOCXX_PRIVATE void* convert() const;

    version _version;
    stdx::optional<bool> _strict;
    stdx::optional<bool> _deprecation_errors;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
