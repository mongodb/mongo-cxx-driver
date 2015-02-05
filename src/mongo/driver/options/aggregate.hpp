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

#include <mongo/bson/document.hpp>
#include <mongo/driver/read_preference.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB aggregation operation.
///
class LIBMONGOCXX_API aggregate {

   public:

    ///
    /// Enables writing to temporary files. When set to @c true, aggregation stages can write data
    /// to the _tmp subdirectory in the dbPath directory. The server-side default is @c false.
    ///
    /// @param allow_disk_use
    ///   Whether or not to allow disk use.
    ///
    void allow_disk_use(bool allow_disk_use);

    ///
    /// Retrieves the current allow_disk_use setting.
    ///
    /// @return Whether disk use is allowed.
    ///
    const stdx::optional<bool>& allow_disk_use() const;

    ///
    /// Sets the number of documents to return per batch.
    ///
    /// @param batch_size
    ///   The size of the batches to request.
    ///
    void batch_size(std::int32_t batch_size);

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    const stdx::optional<std::int32_t>& batch_size() const;

    ///
    /// Sets the maximum amount of time for this operation to run server-side in milliseconds.
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
    /// @return
    ///   The current max time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const stdx::optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets whether the results of this aggregation should be returned via a cursor.
    ///
    /// @note The default for this value depends on the version of the server:
    ///   - Servers >= 2.6 will use a server-side default of true.
    ///   - Servers < 2.6 will use a server-side default of false.
    ///
    /// If this optional setting is not engaged client-side, the server default will be used.
    ///
    /// @param use_cursor whether or not to use a cursor
    ///
    void use_cursor(bool use_cursor);

    ///
    /// The current use_cursor setting.
    ///
    /// @return the current use_cursor setting
    ///
    const stdx::optional<bool>& use_cursor() const;

    ///
    /// Sets the read_preference for this operation.
    ///
    /// @param rp the new read_preference
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
    stdx::optional<bool> _allow_disk_use;
    stdx::optional<std::int32_t> _batch_size;
    stdx::optional<std::int64_t> _max_time_ms;
    stdx::optional<bool> _use_cursor;
    stdx::optional<class read_preference> _read_preference;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
