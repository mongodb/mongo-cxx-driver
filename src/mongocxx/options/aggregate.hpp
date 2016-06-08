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

#include <chrono>
#include <cstdint>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB aggregation operation.
///
class MONGOCXX_API aggregate {
   public:
    ///
    /// Enables writing to temporary files. When set to @c true, aggregation stages can write data
    /// to the _tmp subdirectory in the dbPath directory. The server-side default is @c false.
    ///
    /// @param allow_disk_use
    ///   Whether or not to allow disk use.
    ///
    aggregate& allow_disk_use(bool allow_disk_use);

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
    aggregate& batch_size(std::int32_t batch_size);

    ///
    /// The current batch size setting.
    ///
    /// @return The current batch size.
    ///
    const stdx::optional<std::int32_t>& batch_size() const;

    ///
    /// Sets the maximum amount of time for this operation to run server-side in milliseconds.
    ///
    /// @param max_time
    ///   The max amount of time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    aggregate& max_time(std::chrono::milliseconds max_time);

    ///
    /// The current max_time setting.
    ///
    /// @return
    ///   The current max time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const stdx::optional<std::chrono::milliseconds>& max_time() const;

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
    aggregate& use_cursor(bool use_cursor);

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
    aggregate& read_preference(class read_preference rp);

    ///
    /// The current read_preference for this operation.
    ///
    /// @return the current read_preference
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    const stdx::optional<class read_preference>& read_preference() const;

    ///
    /// Sets whether the $out stage should bypass document validation.
    ///
    /// @param bypass_document_validation whether or not to bypass validation.
    ///
    aggregate& bypass_document_validation(bool bypass_document_validation);

    ///
    /// The current bypass_document_validation setting.
    ///
    /// @return the current bypass_document_validation setting
    ///
    const stdx::optional<bool>& bypass_document_validation() const;

   private:
    stdx::optional<bool> _allow_disk_use;
    stdx::optional<std::int32_t> _batch_size;
    stdx::optional<std::chrono::milliseconds> _max_time;
    stdx::optional<bool> _use_cursor;
    stdx::optional<class read_preference> _read_preference;
    stdx::optional<bool> _bypass_document_validation;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
