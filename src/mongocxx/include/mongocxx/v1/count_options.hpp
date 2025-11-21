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

#include <mongocxx/v1/count_options-fwd.hpp> // IWYU pragma: export

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

#include <chrono>
#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// Options for a "countDocuments" operation.
///
/// Supported fields include:
/// - `collation`
/// - `comment`
/// - `hint`
/// - `limit`
/// - `max_time` ("maxTimeMS")
/// - `read_preference` ("readPreference")
/// - `skip`
///
/// @see
/// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class count_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~count_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() count_options(count_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) operator=(count_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() count_options(count_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) operator=(count_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() count_options();

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) hint(v1::hint index_hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

    ///
    /// Set the "limit" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) limit(std::int64_t limit);

    ///
    /// Return the current "limit" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) limit() const;

    ///
    /// Set the "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) max_time(std::chrono::milliseconds max_time);

    ///
    /// Return the current "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

    ///
    /// Set the "skip" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) skip(std::int64_t skip);

    ///
    /// Return the current "skip" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) skip() const;

    ///
    /// Set the "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(count_options&) read_preference(v1::read_preference rp);

    ///
    /// Return the current "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_preference>) read_preference() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::count_options.
///
