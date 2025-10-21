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

#include <mongocxx/v1/hint-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options related to a MongoDB query hint.
///
/// A document or string that specifies the index to use to support the query predicate.
///
/// @see
/// - [Query Optimization (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/query-optimization/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class hint {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~hint();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() hint(hint&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(hint&) operator=(hint&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() hint(hint const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(hint&) operator=(hint const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `this->str().has_value() == false`
    /// - `this->doc().has_value() == false`
    ///
    MONGOCXX_ABI_EXPORT_CDECL() hint();

    ///
    /// Initialize this "hint" option as a document.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() hint(std::string str);

    ///
    /// Return the current "hint" option as a document.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() hint(bsoncxx::v1::document::value doc);

    ///
    /// Return the current "hint" value as a string.
    ///
    /// @returns Empty when not a string.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) str() const;

    ///
    /// Return the current "hint" value as a document.
    ///
    /// @returns Empty when not a document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) doc() const;

    ///
    /// Return the current "hint" value as a BSON type value.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::view) to_value() const;

    ///
    /// Equivalent to to_value() const.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() operator bsoncxx::v1::types::view() const;

    ///
    /// Compare equal when the underlying "hint" values compare equal.
    ///
    /// @{
    friend bool operator==(hint const& lhs, hint const& rhs) {
        return lhs.str() == rhs.str() || lhs.doc() == rhs.doc();
    }

    friend bool operator!=(hint const& lhs, hint const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

    ///
    /// Equivalent to comparing `h.str()` with `str`.
    ///
    /// @{
    friend bool operator==(hint const& h, bsoncxx::v1::stdx::string_view str) {
        return h.str() == str;
    }

    friend bool operator!=(hint const& h, bsoncxx::v1::stdx::string_view str) {
        return h.str() != str;
    }

    friend bool operator==(bsoncxx::v1::stdx::string_view str, hint const& h) {
        return str == h.str();
    }

    friend bool operator!=(bsoncxx::v1::stdx::string_view str, hint const& h) {
        return str != h.str();
    }
    /// @}
    ///

    ///
    /// Equivalent to comparing `h.doc()` and `doc`.
    ///
    /// @{
    friend bool operator==(hint const& h, bsoncxx::v1::document::view doc) {
        return h.doc() == doc;
    }

    friend bool operator!=(hint const& h, bsoncxx::v1::document::view doc) {
        return h.doc() != doc;
    }

    friend bool operator==(bsoncxx::v1::document::view doc, hint const& h) {
        return doc == h.doc();
    }

    friend bool operator!=(bsoncxx::v1::document::view doc, hint const& h) {
        return doc != h.doc();
    }
    /// @}
    ///
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::hint.
///
