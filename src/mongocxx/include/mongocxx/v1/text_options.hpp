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

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v1 {

///
/// Options related to text_options queries for Queryable Encryption.
///
/// Supported fields include:
/// - `case_sensitive` ("caseSensitive")
/// - `diacritic_sensitive` ("diacriticSensitive")
/// - `prefix`
/// - `suffix`
/// - `substring`
///
/// @see
/// - [Client Side Encryption (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/client-side-encryption/client-side-encryption/)
/// - [Encrypted Fields and Enabled Queries (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/fundamentals/encrypt-and-query/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class text_options {
   public:
    class prefix;
    class suffix;
    class substring;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~text_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() text_options(text_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) operator=(text_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() text_options(text_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) operator=(text_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() text_options();

    ///
    /// Set the "caseSensitive" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) case_sensitive(bool value);

    ///
    /// Gets the current "caseSensitive" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) case_sensitive() const;

    ///
    /// Set the "diacriticSensitive" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) diacritic_sensitive(bool value);

    ///
    /// Gets the current "diacriticSensitive" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) diacritic_sensitive() const;

    ///
    /// Sets the "prefix" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) prefix_opts(prefix opts);

    ///
    /// Gets the current "prefix" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<prefix>) prefix_opts() const;

    ///
    /// Sets the "suffix" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) suffix_opts(suffix opts);

    ///
    /// Gets the current "suffix" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<suffix>) suffix_opts() const;

    ///
    /// Sets the "substring" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(text_options&) substring_opts(substring opts);

    ///
    /// Gets the current "substring" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<substring>) substring_opts() const;

   private:
    class impl;
    void* _impl;
};

class text_options::prefix {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~prefix();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() prefix(prefix&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(prefix&) operator=(prefix&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() prefix(prefix const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(prefix&) operator=(prefix const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() prefix();

    ///
    /// Set the "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(prefix&) str_max_query_length(int32_t value);

    ///
    /// Return the current "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_max_query_length() const;

    ///
    /// Set the "strMinQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(prefix&) str_min_query_length(int32_t value);

    ///
    /// Return the current "strMinQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_min_query_length() const;
};
class text_options::suffix {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~suffix();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() suffix(suffix&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(suffix&) operator=(suffix&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() suffix(suffix const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(suffix&) operator=(suffix const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() suffix();

    ///
    /// Set the "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(suffix&) str_max_query_length(int32_t value);

    ///
    /// Return the current "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_max_query_length() const;

    ///
    /// Set the "strminQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(suffix&) str_min_query_length(int32_t value);

    ///
    /// Return the current "strminQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_min_query_length() const;
};
class text_options::substring {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~substring();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() substring(substring&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(substring&) operator=(substring&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() substring(substring const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(substring&) operator=(substring const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() substring();

    ///
    /// Set the "strMaxLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(substring&) str_max_length(int32_t value);

    ///
    /// Return the current "strMaxLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_max_length() const;
    ///
    /// Set the "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(substring&) str_max_query_length(int32_t value);

    ///
    /// Return the current "strMaxQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_max_query_length() const;

    ///
    /// Set the "strminQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(substring&) str_min_query_length(int32_t value);

    ///
    /// Return the current "strminQueryLength" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<int32_t>) str_min_query_length() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::text_options.
///
