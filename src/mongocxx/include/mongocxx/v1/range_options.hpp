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

#include <mongocxx/v1/range_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// Options related to range_options queries for Queryable Encryption.
///
/// Supported fields include:
/// - `max`
/// - `min`
/// - `precision`
/// - `sparsity`
/// - `trim_factor` ("trimFactor")
///
/// @see
/// - [Client Side Encryption (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/client-side-encryption/client-side-encryption/)
/// - [Encrypted Fields and Enabled Queries (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/fundamentals/encrypt-and-query/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class range_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~range_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() range_options(range_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) operator=(range_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() range_options(range_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) operator=(range_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() range_options();

    ///
    /// Set the "min" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) min(bsoncxx::v1::types::value value);

    ///
    /// Return the current "min" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>)
    min() const;

    ///
    /// Set the "max" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) max(bsoncxx::v1::types::value value);

    ///
    /// Return the current "max" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>)
    max() const;

    ///
    /// Set the "sparsity" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) sparsity(std::int64_t value);

    ///
    /// Return the current "sparsity" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>)
    sparsity() const;

    ///
    /// Set the "trimFactor" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) trim_factor(std::int32_t value);

    ///
    /// Return the current "trimFactor" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>)
    trim_factor() const;

    ///
    /// Set the "precision" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(range_options&) precision(std::int32_t value);

    ///
    /// Return the current "precision" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>)
    precision() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::range_options.
///
