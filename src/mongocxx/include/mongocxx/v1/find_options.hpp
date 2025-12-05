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

#include <mongocxx/v1/find_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/cursor.hpp>

#include <chrono>
#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// Options for a "find" command.
///
/// Supported fields include:
/// - `allow_disk_use` ("allowDiskUse")
/// - `allow_partial_results` ("allowPartialResults")
/// - `batch_size` ("batchSize")
/// - `collation`
/// - `comment`
/// - `cursor_type` ("cursorType")
/// - `hint`
/// - `let`
/// - `limit`
/// - `max_await_time` ("maxAwaitTimeMS")
/// - `max_time` ("maxTimeMS")
/// - `max`
/// - `min`
/// - `no_cursor_timeout` ("noCursorTimeout")
/// - `projection`
/// - `read_preference` ("readPreference")
/// - `return_key` ("returnKey")
/// - `show_record_id` ("showRecordId")
/// - `skip`
/// - `sort`
///
/// @see
/// - [`find` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/find/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class find_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~find_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_options(find_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) operator=(find_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_options(find_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) operator=(find_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_options();

    ///
    /// Set the "allowDiskUse" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) allow_disk_use(bool allow_disk_use);

    ///
    /// Return the current "allowDiskUse" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) allow_disk_use() const;

    ///
    /// Set the "allowPartialResults" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) allow_partial_results(bool allow_partial);

    ///
    /// Return the current "allowPartialResults" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) allow_partial_results() const;

    ///
    /// Set the "batchSize" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) batch_size(std::int32_t batch_size);

    ///
    /// Return the current "batchSize" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) batch_size() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

    ///
    /// Set the "cursorType" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) cursor_type(v1::cursor::type cursor_type);

    ///
    /// Return the current "cursorType" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<cursor::type>) cursor_type() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) hint(v1::hint index_hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) let(bsoncxx::v1::document::value let);

    ///
    /// Return the current "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) let() const;

    ///
    /// Set the "limit" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) limit(std::int64_t limit);

    ///
    /// Return the current "limit" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) limit() const;

    ///
    /// Set the "max" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) max(bsoncxx::v1::document::value max);

    ///
    /// Return the current "max" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) max() const;

    ///
    /// Set the "maxAwaitTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) max_await_time(std::chrono::milliseconds max_await_time);

    ///
    /// Return the current "maxAwaitTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_await_time() const;

    ///
    /// Set the "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) max_time(std::chrono::milliseconds max_time);

    ///
    /// Return the current "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

    ///
    /// Set the "min" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) min(bsoncxx::v1::document::value min);

    ///
    /// Return the current "min" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) min() const;

    ///
    /// Set the "noCursorTimeout" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) no_cursor_timeout(bool no_cursor_timeout);

    ///
    /// Return the current "noCursorTimeout" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) no_cursor_timeout() const;

    ///
    /// Set the "projection" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) projection(bsoncxx::v1::document::value projection);

    ///
    /// Return the current "projection" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) projection() const;

    ///
    /// Set the "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) read_preference(v1::read_preference rp);

    ///
    /// Return the current "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_preference>) read_preference() const;

    ///
    /// Set the "returnKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) return_key(bool return_key);

    ///
    /// Return the current "returnKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) return_key() const;

    ///
    /// Set the "showRecordId" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) show_record_id(bool show_record_id);

    ///
    /// Return the current "showRecordId" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) show_record_id() const;

    ///
    /// Set the "skip" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) skip(std::int64_t skip);

    ///
    /// Return the current "skip" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) skip() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_options&) sort(bsoncxx::v1::document::value ordering);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::find_options.
///
