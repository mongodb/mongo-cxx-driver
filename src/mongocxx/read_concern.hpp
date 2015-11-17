// Copyright 2015 MongoDB Inc.
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

#include <memory>

#include <mongocxx/config/prelude.hpp>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client;
class collection;
class database;
class uri;

///
/// A class to represent the read concern. Read concern can be set at the client, database, or
/// collection level. The read concern can also be provided via connection string, and will be
/// parsed and set on the client constructed for the URI.
///
/// For the WiredTiger storage engine, MongoDB 3.2 introduced the readConcern option for replica
/// sets and replica set shards. The readConcern option allows clients to choose a level of
/// isolation for their reads. You can specify a readConcern of "majority" to read data that has
/// been written to a majority of nodes and thus cannot be rolled back. By default, MongoDB uses a
/// readConcern of "local" which does not guarantee that the read data would not be rolled back.
///
/// @TODO link to the docs when they exist.
///
class MONGOCXX_API read_concern {

   public:
    ///
    /// A class to represent the read concern level.
    /// @TODO link to the docs when they exist.
    ///
    enum class level {
        k_local,
        k_majority,
        k_unknown,
    };

    ///
    /// Constructs a new read_concern with default acknowledge_level of k_empty and default
    /// acknowledge_string of "" (empty string).
    ///
    read_concern();

    ///
    /// Copy constructs a read_concern.
    ///
    read_concern(const read_concern&);

    ///
    /// Copy assigns a read_concern.
    ///
    read_concern& operator=(const read_concern&);

    ///
    /// Move constructs a read_concern.
    ///
    read_concern(read_concern&&) noexcept;

    ///
    /// Move assigns a read_concern.
    ///
    read_concern& operator=(read_concern&&) noexcept;

    ///
    /// Destroys a read_concern.
    ///
    ~read_concern();

    ///
    /// Sets the read concern level.
    ///
    /// @param rc_level
    ///   Either k_local or k_majority.
    ///
    /// @throws std::invalid_argument if rc_level is not k_local or k_majority.
    ///
    void acknowledge_level(level rc_level);

    ///
    /// Gets the current read concern level. If this was set with acknowledge_string to anything
    /// other than "local" or "majority", this will return k_unknown.
    ///
    /// @return The read concern level.
    ///
    stdx::optional<level> acknowledge_level() const;

    ///
    /// Sets the read concern string.
    ///
    /// @param rc_string
    ///   The read concern string.
    ///
    void acknowledge_string(stdx::string_view rc_string);

    ///
    /// Gets the current read concern string. If the read concern level was set with
    /// acknowledge_level, this will return either "local" or "majority".
    ///
    /// @return The read concern string.
    ///
    stdx::optional<stdx::string_view> acknowledge_string() const;

   private:
    friend client;
    friend collection;
    friend database;
    friend uri;

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE read_concern(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;

};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
