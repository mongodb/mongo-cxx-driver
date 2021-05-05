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
    /// Constructs a new server_api object.
    ///
    /// The specified API version will be sent to the server. This will cause
    /// the server to behave in a manner compatible with that API version.
    /// It also causes the driver to behave in a manner compatible with the
    /// driver’s behavior as of the release when the driver first started
    /// to support the specified server API version.
    ///
    /// @param server_api_version
    ///   The server api version to send to the server.
    ///
    server_api(const char* server_api_version);

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
    /// Gets a boolean specifying whether the server will error for features
    /// that are not a part of the declared API version.
    ///
    /// @return
    ///   A boolean specifying whether the server will error for features that
    ///   are not part of the declared API version.
    ///
    bool strict() const;

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
    /// Gets a boolean specifying whether the server will error on deprecated features.
    ///
    /// @return
    ///   A boolean specifying whether the server will error on deprecated features.
    ///
    bool deprecation_errors() const;

    ///
    /// Gets the server api version.
    ///
    /// @return
    ///   A string specifying the declared server api version.
    ///
    const char* server_api_version() const;

   private:
    friend class mongocxx::client;
    friend class mongocxx::pool;

    MONGOCXX_PRIVATE void* convert() const;

    bool _strict;
    bool _deprecation_errors;
    const char* _server_api_version;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
