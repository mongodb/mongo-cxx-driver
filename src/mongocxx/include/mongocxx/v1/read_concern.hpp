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

#include <mongocxx/v1/read_concern-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

namespace mongocxx {
namespace v1 {

///
/// Options related to a MongoDB Read Concern.
///
/// Supported fields include:
/// - `level`
///
/// @see
/// - [Read Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/read-concern/)
/// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class read_concern {
   private:
    void* _impl; // mongoc_read_concern_t

   public:
    ///
    /// The read concern level.
    ///
    /// @see
    /// - [Read Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/read-concern/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class level {
        ///
        /// [Read Concern "local" (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/read-concern-local/)
        ///
        k_local,

        ///
        /// [Read Concern "majority" (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/read-concern-majority/)
        ///
        k_majority,

        ///
        /// [Read Concern "linearizable" (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/read-concern-linearizable/)
        ///
        k_linearizable,

        ///
        /// [Default MongoDB Read Concerns/Write Concerns (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
        ///
        k_server_default,

        ///
        /// An unknown (unsupported) read concern level.
        ///
        /// @note Not to be confused with @ref level::k_server_default.
        ///
        k_unknown,

        ///
        /// [Read Concern "available" (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/read-concern-available/)
        ///
        k_available,

        ///
        /// [Read Concern "snapshot" (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/read-concern-snapshot/)
        ///
        k_snapshot,
    };

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~read_concern();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_concern(read_concern&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_concern&) operator=(read_concern&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_concern(read_concern const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_concern&) operator=(read_concern const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `this->level() == k_server_default`
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_concern();

    ///
    /// Set the read concern level.
    ///
    /// @param v One of:
    /// - @ref level::k_local
    /// - @ref level::k_majority
    /// - @ref level::k_linearizable
    /// - @ref level::k_server_default
    /// - @ref level::k_available
    /// - @ref level::k_snapshot
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_concern&) acknowledge_level(level v);

    ///
    /// Return the current read concern level.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(level) acknowledge_level() const;

    ///
    /// Set the read concern level to an arbitrary string.
    ///
    /// @param v Equivalent to `this->acknowledge_level(k_server_default)` when `v` is empty.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_concern&) acknowledge_string(bsoncxx::v1::stdx::string_view v);

    ///
    /// Return the current read concern level as a string.
    ///
    /// @returns Empty when `this->acknowledge_level()` is @ref level::k_server_default or @ref level::k_unknown.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) acknowledge_string() const;

    ///
    /// Return this read concern option as a document.
    ///
    /// @par Preconditions:
    /// - `this->acknowledge_level() != level::k_unknown`.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) to_document() const;

    ///
    /// Compare equal when all supported fields compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(read_concern const& lhs, read_concern const& rhs);

    friend bool operator!=(read_concern const& lhs, read_concern const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

    class internal;

   private:
    /* explicit(false) */ read_concern(void* impl);
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::read_concern.
///
