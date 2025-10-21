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

#include <mongocxx/v1/write_concern-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <cstdint>
#include <memory>

namespace mongocxx {
namespace v1 {

///
/// Options related to a MongoDB Write Concern.
///
/// Supported fields include:
/// - `journal` ("j")
/// - `level` ("w")
/// - `timeout` ("wtimeout")
///
/// @see
/// - [Write Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/write-concern/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class write_concern {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// The write concern level.
    ///
    /// @see
    /// - [Write Concern (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/write-concern/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class level {
        ///
        /// [Default MongoDB Read Concerns/Write Concerns (MongoDB
        /// Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
        ///
        k_default,

        ///
        /// [`"w": "majority"`](https://www.mongodb.com/docs/manual/reference/write-concern/)
        ///
        k_majority,

        ///
        /// [`"w": "<custom write concern name>"`](https://www.mongodb.com/docs/manual/reference/write-concern/)
        ///
        k_tag,

        ///
        /// [`"w": 0`](https://www.mongodb.com/docs/manual/reference/write-concern/)
        ///
        k_unacknowledged,

        ///
        /// [`"w": 1`](https://www.mongodb.com/docs/manual/reference/write-concern/)
        ///
        k_acknowledged,

        ///
        /// An unknown (unsupported) write concern level.
        ///
        /// @note Not to be confused with @ref k_default.
        ///
        k_unknown,
    };

    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~write_concern();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern(write_concern&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) operator=(write_concern&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern(write_concern const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) operator=(write_concern const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write_concern();

    ///
    /// Set the "w" field as to acknowledgement level.
    ///
    /// @param v One of:
    /// - k_default
    /// - k_majority
    /// - k_unacknowledged
    /// - k_acknowledged
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) acknowledge_level(level v);

    ///
    /// Return the current "w" field as an acknowledgement level.
    ///
    /// @returns
    /// - @ref k_default when "w" is unset.
    /// - @ref k_majority when `"w": "majority"`.
    /// - @ref k_tag when `"w": <custom write concern name>`.
    /// - @ref k_unacknowledged when `"w": 0`.
    /// - @ref k_acknowledged when `"w": n` and `n >= 1`.
    /// - @ref k_unknown for all other (unsupported) values.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(level) acknowledge_level() const;

    ///
    /// Equivalent to `this->acknowledge_level(level::k_majority)`.
    ///
    write_concern& majority() {
        return this->acknowledge_level(level::k_majority);
    }

    ///
    /// Equivalent to `this->acknowledge_level(k_majority).timeout(timeout)`.
    ///
    write_concern& majority(std::chrono::milliseconds v) {
        return this->acknowledge_level(level::k_majority).timeout(v);
    }

    ///
    /// Set the "wtimeout" field.
    ///
    /// @note `0` is equivalent to "unset".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) timeout(std::chrono::milliseconds v);

    ///
    /// Return the current "wtimeout" field.
    ///
    /// @note `0` is equivalent to "unset".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::chrono::milliseconds) timeout() const;

    ///
    /// Set the "w" field to an integer.
    ///
    /// @param v
    /// - `0` is equivalent to `this->acknowledge_level(k_unacknowledged)`.
    /// - `1` is equivalent to `this->acknowledge_level(k_acknowledged)`.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) nodes(std::int32_t v);

    ///
    /// Return the current "w" field as an integer.
    ///
    /// @returns Empty when `this->acknowledge_level()` is not @ref k_unacknowledged or @ref k_acknowledged.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) nodes() const;

    ///
    /// Set the "w" field to a custom write concern name.
    ///
    /// @param v Equivalent to `this->acknowledge_level(level::k_default)` when `v` is empty.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) tag(bsoncxx::v1::stdx::string_view v);

    ///
    /// Return the current "w" value as a custom write concern name.
    ///
    /// @returns Empty when `this->acknowledge_level()` is not @ref k_tag.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) tag() const;

    ///
    /// Set the "j" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_concern&) journal(bool j);

    ///
    /// Return the current "j" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) journal() const;

    ///
    /// Return true when this write concern requires acknowledgement.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) is_acknowledged() const;

    ///
    /// Return this write concern option as a document.
    ///
    /// @par Preconditions:
    /// - `this->acknowledge_level() != level::k_unknown`.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) to_document() const;

    ///
    /// Compare equal when all supported fields compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(write_concern const& lhs, write_concern const& rhs);

    friend bool operator!=(write_concern const& lhs, write_concern const& rhs) {
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
/// Provides @ref mongocxx::v1::write_concern.
///
