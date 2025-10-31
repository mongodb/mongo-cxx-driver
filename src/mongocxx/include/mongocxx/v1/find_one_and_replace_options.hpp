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

#include <mongocxx/v1/find_one_and_replace_options-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/return_document-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>

namespace mongocxx {
namespace v1 {

///
/// Options for a "findOneAndReplace" operation.
///
/// Supported fields include:
/// - `bypass_document_validation` ("bypassDocumentValidation")
/// - `collation`
/// - `comment`
/// - `hint`
/// - `let`
/// - `max_time` ("maxTimeMS")
/// - `projection`
/// - `return_document` ("returnDocument")
/// - `sort`
/// - `upsert`
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Query Documents (MongoDB Manual)](https://www.mongodb.com/docs/manual/tutorial/query-documents/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class find_one_and_replace_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~find_one_and_replace_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_one_and_replace_options(find_one_and_replace_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) operator=(find_one_and_replace_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_one_and_replace_options(find_one_and_replace_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) operator=(find_one_and_replace_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() find_one_and_replace_options();

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) bypass_document_validation(
        bool bypass_document_validation);

    ///
    /// Return the current "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) bypass_document_validation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) hint(v1::hint index_hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) let(bsoncxx::v1::document::value let);

    ///
    /// Return the current "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) let() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const) comment() const;

    ///
    /// Set the "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) max_time(std::chrono::milliseconds max_time);

    ///
    /// Return the current "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

    ///
    /// Set the "projection" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) projection(bsoncxx::v1::document::value projection);

    ///
    /// Return the current "projection" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) projection() const;

    ///
    /// Set the "returnDocument" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) return_document(return_document return_document);

    ///
    /// Return the current "returnDocument" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::return_document>) return_document() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) sort(bsoncxx::v1::document::value ordering);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) upsert(bool upsert);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(find_one_and_replace_options&) write_concern(v1::write_concern write_concern);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::find_one_and_replace_options.
///
