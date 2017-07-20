// Copyright 2017 MongoDB Inc.
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

#include <bsoncxx/string/view_or_value.hpp>
#include <mongocxx/options/session.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class MONGOCXX_API session {
   public:
    ///
    /// Creates a new session.
    ///
    /// @param client
    ///   The client that created this session.
    /// @param options
    ///   Additional options for configuring the session.
    ///
    session(const client& client, const options::session& options = {});

    session(const session&) = delete;

    session& operator=(const session&) = delete;

    ///
    /// Move constructs a session.
    ///
    session(session&&) noexcept;

    ///
    /// Move assigns a session.
    ///
    session& operator=(session&&) noexcept;

    ///
    /// Ends and destroys the session.
    ///
    ~session();

    ///
    /// Gets the client that started this session.
    ///
    /// @return
    ///   The client that started this session.
    ///
    const mongocxx::client& client();

    ///
    /// Gets the current write concern for this session.
    ///
    /// @return
    ///   The current @c write_concern.
    ///
    class write_concern write_concern() const;

    ///
    /// Gets the current read concern for this session.
    ///
    /// @return
    ///   The current @c read_concern.
    ///
    class read_concern read_concern() const;

    ///
    /// Returns the current read preference for this session.
    ///
    /// @return
    ///   The current @c read_preference.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/core/read-preference/
    ///
    class read_preference read_preference() const;

    ///
    /// Returns whether the driver session has ended. A driver session has ended when endSession has
    /// been called.
    ///
    /// @return
    ///   whether the driver session as ended.
    ///
    bool has_ended() const;

    ///
    /// Ends this driver session.
    ///
    /// @note
    ///   This does not have to be explicitly called unless the user wants to manually end the
    ///   session before it gets destroyed.
    /// @note
    ///   If session has already ended, this method is a no-op.
    ///
    void end_session();

    ///
    /// Obtains a database that represents a logical grouping of collections on a MongoDB server.
    ///
    /// @note
    ///   A database cannot be obtained from a temporary session object.
    ///
    /// @param name
    ///   The name of the database to get.
    ///
    /// @return
    ///   The database.
    ///
    class database database(bsoncxx::string::view_or_value name) const&;
    class database database(bsoncxx::string::view_or_value name) const&& = delete;

    ///
    /// Allows the syntax @c session["db_name"] as a convenient shorthand for the
    /// session::database() method by implementing the array subscript operator.
    ///
    /// @note
    ///   A database cannot be obtained from a temporary session object.
    ///
    /// @param name
    ///   The name of the database.
    ///
    /// @return
    ///   Client side representation of a server side database.
    ///
    class database operator[](bsoncxx::string::view_or_value name) const&;
    class database operator[](bsoncxx::string::view_or_value name) const&& = delete;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
