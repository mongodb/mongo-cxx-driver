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

#include <mongocxx/v1/insert_one_options-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>

namespace mongocxx {
namespace v1 {

///
/// Options for an "insertOne" operation.
///
/// Supported fields include:
/// - `bypass_document_validation` ("bypassDocumentValidation")
/// - `comment`
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Insert Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/insert-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class insert_one_options {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~insert_one_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() insert_one_options(insert_one_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one_options&) operator=(insert_one_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() insert_one_options(insert_one_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one_options&) operator=(insert_one_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() insert_one_options();

    ///
    /// Set the "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one_options&) bypass_document_validation(bool bypass_document_validation);

    ///
    /// Return the current "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) bypass_document_validation() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one_options&) write_concern(v1::write_concern wc);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::insert_one_options.
///
