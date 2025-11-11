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

#include <mongocxx/v1/replace_one_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

namespace mongocxx {
namespace v1 {

///
/// Options for a "replaceOne" operation.
///
/// Supported fields include:
/// - `bypass_document_validation` ("bypassDocumentValidation")
/// - `collation`
/// - `comment`
/// - `hint`
/// - `let`
/// - `sort`
/// - `upsert`
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class replace_one_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~replace_one_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one_options(replace_one_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) operator=(replace_one_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one_options(replace_one_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) operator=(replace_one_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one_options();

    ///
    /// Set the "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) bypass_document_validation(bool bypass_document_validation);

    ///
    /// Return the current "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) bypass_document_validation() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) upsert(bool upsert);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) write_concern(v1::write_concern wc);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) hint(v1::hint index_hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) let(bsoncxx::v1::document::value let);

    ///
    /// Return the current "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) let() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) sort(bsoncxx::v1::document::value sort);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const) comment() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::replace_one_options.
///
