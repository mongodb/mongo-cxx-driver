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

#include <mongo/driver/config/prelude.hpp>

#include <cstdint>
#include <string>

#include <mongo/bson/document.hpp>
#include <mongo/driver/read_preference.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

// TODO: take hint as a std::string parameter in addition to bson::document::view
// TODO: figure out hint (make a new type or use bson::element?)

///
/// Class representing the optional arguments to a MongoDB count command
///
class LIBMONGOCXX_API count {

   public:

    ///
    /// Sets the index to use for this operation.
    ///
    /// @param hint
    ///   Document representing the index to use.
    ///
    void hint(bson::document::view hint);

    ///
    /// Gets the current index being hinted.
    ///
    /// @return The current hint.
    ///
    const stdx::optional<bson::document::view>& hint() const;

    ///
    /// Sets the maximum number of documents to count.
    ///
    /// @param limit
    ///  The max number of documents to count.
    ///
    void limit(std::int64_t limit);

    ///
    /// Gets the current limit.
    ///
    /// @return The current limit.
    ///
    const stdx::optional<std::int64_t>& limit() const;

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time_ms
    ///   The max amount of time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    void max_time_ms(std::int64_t max_time_ms);

    ///
    /// The current max_time_ms setting.
    ///
    /// @return The current max time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const stdx::optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets the number of documents to skip before counting documents.
    ///
    /// @param skip
    ///   The number of documents to skip.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.skip/
    ///
    void skip(std::int64_t skip);

    ///
    /// Gets the current number of documents to skip.
    ///
    /// @return The number of documents to skip.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/cursor.skip/
    ///
    const stdx::optional<std::int64_t>& skip() const;

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp
    ///   The new read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    void read_preference(class read_preference rp);

    ///
    /// The current read_preference for this operation.
    ///
    /// @return the current read_preference
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    const stdx::optional<class read_preference>& read_preference() const;

   private:
    stdx::optional<bson::document::view> _hint;
    stdx::optional<std::int64_t> _limit;
    stdx::optional<std::int64_t> _max_time_ms;
    stdx::optional<std::int64_t> _skip;
    stdx::optional<class read_preference> _read_preference;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
