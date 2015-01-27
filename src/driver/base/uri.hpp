// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include <memory>
#include <string>
#include <vector>

#include "bson/document.hpp"
#include "driver/base/read_preference.hpp"
#include "driver/base/write_concern.hpp"

namespace mongo {
namespace driver {
namespace base {

///
/// Class representing a MongoDB connection string URI.
///
/// @todo: return const char* instead or stringview
/// @todo: harmonize with C library (options, credentials, etc...)
///
/// @see http://docs.mongodb.org/manual/reference/connection-string/
///
class uri {

   public:

    struct host {
         std::string host;
         std::uint16_t port;
         std::int32_t family;
    };

    static const std::string k_default_uri;

    ///
    /// Constructs a uri from an optional MongoDB uri string. If no uri string is specified,
    /// uses the default uri string, 'mongodb://localhost:27017'.
    ///
    /// @param uri_string
    ///   String representing a MongoDB connection string uri, defaults to k_default_uri.
    ///
    /// @todo this should really take a stringview (polyfilled)?
    ///
    uri(const std::string& uri_string = k_default_uri);

    ///
    /// Move constructs a uri.
    ///
    uri(uri&&) noexcept;

    ///
    /// Move assigns a uri.
    ///
    uri& operator=(uri&&) noexcept;

    ///
    /// Destroys a uri.
    ///
    ~uri();

    ///
    /// Returns the authentication mechanism from the uri.
    ///
    /// @return A string representing the authentication mechanism.
    ///
    std::string auth_mechanism() const;

    ///
    /// Returns the authentication source from the uri.
    ///
    /// @return A string representing the authentication source.
    ///
    std::string auth_source() const;

    ///
    /// Returns the hosts from the uri.
    ///
    /// @return A vector of hosts.
    ///
    std::vector<host> hosts() const;

    ///
    /// Returns the database from the uri.
    ///
    /// @return A string with the name of the database.
    ///
    std::string database() const;

    ///
    /// Returns other uri options.
    ///
    /// @return A document view containing other options.
    ///
    bson::document::view options() const;

    ///
    /// Returns the password from the uri.
    ///
    /// @return A string containing the supplied password.
    ///
    std::string password() const;

    ///
    /// Returns the read preference from the uri.
    ///
    /// @return A read_preference that represents what was specified in the uri.
    ///
    class read_preference read_preference() const;

    ///
    /// Returns the replica set specified in the uri.
    ///
    /// @return A string representing the supplied replica set name.
    ///
    std::string replica_set() const;

    ///
    /// Returns the ssl parameter from the uri.
    ///
    /// @return Boolean that is @c true if ssl is enabled and @c false if not.
    ///
    bool ssl() const;

    ///
    /// Returns the uri in a string format.
    ///
    /// @return A string with the uri.
    ///
    std::string to_string() const;

    ///
    /// Returns the supplied username from the uri.
    ///
    /// @return A string with the username specified in the uri.
    ///
    std::string username() const;

    ///
    /// Returns the write concern specified in the uri.
    ///
    /// @return A write_concern that represents what was specified in the uri.
    ///
    class write_concern write_concern() const;

   private:
    friend class client;

    class impl;
    std::unique_ptr<impl> _impl;

};

}  // namespace base
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
