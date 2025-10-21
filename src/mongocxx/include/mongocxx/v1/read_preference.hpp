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

#include <mongocxx/v1/read_preference-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>

namespace mongocxx {
namespace v1 {

///
/// Options related to a MongoDB Read Preference.
///
/// Supported fields include:
/// - `hedge`
/// - `max_staleness` ("maxStalenessSeconds")
/// - `mode`
/// - `tags` ("tag_sets")
///
/// @see
/// - [Read Preference (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/read-preference/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class read_preference {
   private:
    void* _impl; // mongoc_read_prefs_t

   public:
    ///
    /// The read preference mode.
    ///
    /// @see
    /// - [Read Preference (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/read-preference/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class read_mode {
        ///
        /// Primary member only (default).
        ///
        k_primary,

        ///
        /// Primary member when available, secondary otherwise.
        ///
        k_primary_preferred,

        ///
        /// Secondary members only.
        ///
        k_secondary,

        ///
        /// Secondary members when available, primary otherwise.
        ///
        k_secondary_preferred,

        ///
        /// A random eligible member based on a specified latency threshold.
        ///
        k_nearest,
    };

    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~read_preference();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_preference&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) operator=(read_preference&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference(read_preference const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) operator=(read_preference const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() read_preference();

    ///
    /// Set the "hedge" field.
    ///
    /// @deprecated Deprecated in MongoDB Server version 8.0.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(read_preference&) hedge(bsoncxx::v1::document::view v);

    ///
    /// Return the current "hedge" field.
    ///
    /// @deprecated Deprecated in MongoDB Server version 8.0.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) hedge()
        const;

    ///
    /// Set the "mode" field.
    ///
    /// @param v One of:
    /// - @ref k_primary
    /// - @ref k_primary_preferred
    /// - @ref k_secondary
    /// - @ref k_secondary_preferred
    /// - @ref k_nearest
    /// Any unsupported value is interpreted as @ref k_primary.
    ///
    /// @see
    /// - [Read Preference Use Cases](https://www.mongodb.com/docs/manual/core/read-preference-use-cases/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) mode(read_mode v);

    ///
    /// Return the current "mode" field
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_mode) mode() const;

    ///
    /// Set the "tag_sets" field.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) tags(bsoncxx::v1::array::view v);
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) tags(bsoncxx::v1::document::view v);
    /// @}
    ///

    ///
    /// Return the current "tag_sets" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::array::view) tags() const;

    ///
    /// Set the "maxStalenessSeconds" field.
    ///
    /// @param v `-1` is equivalent to "unset".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(read_preference&) max_staleness(std::chrono::seconds v);

    ///
    /// Return the current "maxStalenessSeconds" field.
    ///
    /// @returns An empty optional if the option is unset.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::seconds>) max_staleness() const;

    ///
    /// Compare equal when all supported fields compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(read_preference const& lhs, read_preference const& rhs);

    friend bool operator!=(read_preference const& lhs, read_preference const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::read_preference.
///
