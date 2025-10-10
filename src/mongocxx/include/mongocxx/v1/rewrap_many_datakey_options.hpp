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

#include <mongocxx/v1/rewrap_many_datakey_options-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>

namespace mongocxx {
namespace v1 {

///
/// Options for a "rewrapManyDataKey" operation.
///
/// Supported fields include:
/// - `master_key` ("masterKey")
/// - `provider`
///
/// @see
/// - [Rotate and Rewrap Encryption Keys (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/fundamentals/manage-keys/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class rewrap_many_datakey_options {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~rewrap_many_datakey_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey_options(rewrap_many_datakey_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_options&) operator=(rewrap_many_datakey_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey_options(rewrap_many_datakey_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_options&) operator=(rewrap_many_datakey_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey_options();

    ///
    /// Set the "provider" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_options&) provider(std::string provider);

    ///
    /// Return the current "provider" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) provider() const;

    ///
    /// Set the "masterKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_options&) master_key(bsoncxx::v1::document::value master_key);

    ///
    /// Return the current "masterKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) master_key() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

/// @file
/// Provides @ref mongocxx::v1::rewrap_many_datakey_options.
///
