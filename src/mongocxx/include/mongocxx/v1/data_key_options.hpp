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

#include <mongocxx/v1/data_key_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// A Data Encryption Key (DEK) for In-Use Encryption.
///
/// Supported fields include:
/// - `key_alt_names` ("keyAltNames")
/// - `key_material` ("keyMaterial")
/// - `master_key` ("masterKey")
///
/// @see
/// - [Encryption Keys and Key Vaults (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/fundamentals/keys-key-vaults/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class data_key_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~data_key_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() data_key_options(data_key_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(data_key_options&) operator=(data_key_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() data_key_options(data_key_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(data_key_options&) operator=(data_key_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() data_key_options();

    ///
    /// Set the "masterKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(data_key_options&) master_key(bsoncxx::v1::document::value master_key);

    ///
    /// Return the current "masterKey" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) master_key() const;

    ///
    /// Set the "keyAltNames" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(data_key_options&) key_alt_names(std::vector<std::string> key_alt_names);

    ///
    /// Return the current "keyAltNames" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::vector<std::string>) key_alt_names() const;

    ///
    /// Binary data with a length of exactly 96 bytes.
    ///
    using key_material_type = std::vector<std::uint8_t>;

    ///
    /// Set the "keyMaterial" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(data_key_options&) key_material(key_material_type key_material);

    ///
    /// Return the current "keyMaterial" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<key_material_type>) key_material() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::data_key_options.
///
