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

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/pipeline-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/exception.hpp>

#include <cstdint>
#include <memory>

namespace mongocxx {
namespace v1 {

///
/// A list of bulk write operations. Unlike @ref mongocxx::v1::bulk_write, this supports writes across more than one
/// collection.
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write {
   private:
    class impl;
    void* _impl;

   public:
    class options;
    class result;
    class MONGOCXX_ABI_EXPORT exception;

    class insert_one_options;
    class update_one_options;
    class update_many_options;
    class replace_one_options;
    class delete_one_options;
    class delete_many_options;

    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~client_bulk_write();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client_bulk_write(client_bulk_write&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) operator=(client_bulk_write&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    client_bulk_write(client_bulk_write const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    client_bulk_write& operator=(client_bulk_write const& other) = delete;

    ///
    /// Append an "Insert One" operation targeting the given namespace.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&)
    append(bsoncxx::v1::stdx::string_view ns, bsoncxx::v1::document::value document, insert_one_options const& opts);

    ///
    /// Append an "Update One" operation targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) append(
        bsoncxx::v1::stdx::string_view ns,
        bsoncxx::v1::document::value filter,
        bsoncxx::v1::document::value update,
        update_one_options const& opts);

    ///
    /// Append an "Update One" operation with an aggregation pipeline targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) append(
        bsoncxx::v1::stdx::string_view ns,
        bsoncxx::v1::document::value filter,
        v1::pipeline const& update,
        update_one_options const& opts);

    ///
    /// Append an "Update Many" operation targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) append(
        bsoncxx::v1::stdx::string_view ns,
        bsoncxx::v1::document::value filter,
        bsoncxx::v1::document::value update,
        update_many_options const& opts);

    ///
    /// Append an "Update Many" operation with an aggregation pipeline targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) append(
        bsoncxx::v1::stdx::string_view ns,
        bsoncxx::v1::document::value filter,
        v1::pipeline const& update,
        update_many_options const& opts);

    ///
    /// Append a "Replace One" operation targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&) append(
        bsoncxx::v1::stdx::string_view ns,
        bsoncxx::v1::document::value filter,
        bsoncxx::v1::document::value replacement,
        replace_one_options const& opts);

    ///
    /// Append a "Delete One" operation targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&)
    append(bsoncxx::v1::stdx::string_view ns, bsoncxx::v1::document::value filter, delete_one_options const& opts);

    ///
    /// Append a "Delete Many" operation targeting the given namespace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_bulk_write&)
    append(bsoncxx::v1::stdx::string_view ns, bsoncxx::v1::document::value filter, delete_many_options const& opts);
    /// @}
    ///

    ///
    /// Execute the appended operations.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::client_bulk_write::exception when a bulk write error is encountered.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<result>) execute(options const& opts);

    class internal;

   private:
    /* explicit(false) */ client_bulk_write(void* bulk);
};

///
/// Options for a client bulk write operation.
///
/// Supported fields include:
/// - `bypass_document_validation` ("bypassDocumentValidation")
/// - `comment`
/// - `let`
/// - `ordered`
/// - `verbose_results` ("verboseResults")
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
class client_bulk_write::options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Default initialization.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) bypass_document_validation(bool bypass_document_validation);

    ///
    /// Return the current "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) bypass_document_validation() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

    ///
    /// Set the "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) let(bsoncxx::v1::document::value let);

    ///
    /// Return the current "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) let() const;

    ///
    /// Set the "ordered" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) ordered(bool ordered);

    ///
    /// Return the current "ordered" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) ordered() const;

    ///
    /// Set the "verboseResults" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) verbose_results(bool verbose_results);

    ///
    /// Return the current "verboseResults" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) verbose_results() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) write_concern(v1::write_concern wc);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;

    class internal;
};

///
/// The result of a client bulk write operation.
///
/// Summary counts (`inserted_count()`, `upserted_count()`, `matched_count()`, `modified_count()`,
/// `deleted_count()`) are always available.
///
/// The per-operation result documents (`insert_results()`, `update_results()`, `delete_results()`)
/// are only available when @ref mongocxx::v1::client_bulk_write::options::verbose_results was set to `true`;
/// otherwise they are empty.
///
/// @see
/// - [Bulk Write (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/bulk-write/)
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::result {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~result();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() result(result&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(result&) operator=(result&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() result(result const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(result&) operator=(result const& other);

    ///
    /// Return the number of documents inserted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) inserted_count() const;

    ///
    /// Return the number of documents upserted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) upserted_count() const;

    ///
    /// Return the number of documents matched by update or replace operations.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) matched_count() const;

    ///
    /// Return the number of documents modified by update or replace operations.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) modified_count() const;

    ///
    /// Return the number of documents deleted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) deleted_count() const;

    ///
    /// Return the per-operation insert results as a document mapping model index to the insert result.
    ///
    /// @returns Empty when verbose results were not requested.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) insert_results() const;

    ///
    /// Return the per-operation update results as a document mapping model index to the update result.
    ///
    /// @returns Empty when verbose results were not requested.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) update_results() const;

    ///
    /// Return the per-operation delete results as a document mapping model index to the delete result.
    ///
    /// @returns Empty when verbose results were not requested.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) delete_results() const;

    class internal;

   private:
    /* explicit(false) */ result(void* impl);
};

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

///
/// A client bulk write error.
///
/// @par Inherits:
/// - @ref mongocxx::v1::exception
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::exception : public v1::exception {
   private:
    class impl;
    std::shared_ptr<impl> _impl;

   public:
    ///
    /// A document mapping model index to the corresponding write error.
    ///
    /// @returns An empty document if there are no write errors.
    ///
    bsoncxx::v1::document::view MONGOCXX_ABI_CDECL write_errors() const;

    ///
    /// An array of write concern errors.
    ///
    /// @returns An empty array if there are no write concern errors.
    ///
    bsoncxx::v1::array::view MONGOCXX_ABI_CDECL write_concern_errors() const;

    ///
    /// The server reply associated with the error, if any.
    ///
    /// @returns An empty document if no server reply is available.
    ///
    bsoncxx::v1::document::view MONGOCXX_ABI_CDECL error_reply() const;

    ///
    /// The partial result obtained before the error occurred, if any.
    ///
    /// @returns An empty optional when no operations succeeded before the error.
    ///
    bsoncxx::v1::stdx::optional<result> MONGOCXX_ABI_CDECL partial_result() const;

    class internal;

   private:
    MONGOCXX_ABI_NO_EXPORT /* explicit(false) */
    exception(int code, char const* message, std::unique_ptr<impl> impl);

    MONGOCXX_ABI_NO_EXPORT void key_function() const override;
};

BSONCXX_PRIVATE_WARNINGS_POP();

///
/// Options for an "Insert One" operation in a client bulk write.
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::insert_one_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
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
    MONGOCXX_ABI_EXPORT_CDECL() insert_one_options();

    class internal;
};

///
/// Options for an "Update One" operation in a client bulk write.
///
/// Supported fields include:
/// - `array_filters` ("arrayFilters")
/// - `collation`
/// - `hint`
/// - `sort`
/// - `upsert`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::update_one_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~update_one_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one_options(update_one_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) operator=(update_one_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one_options(update_one_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) operator=(update_one_options const& other);

    ///
    /// Default initialization.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one_options();

    ///
    /// Set the "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) array_filters(bsoncxx::v1::array::value array_filters);

    ///
    /// Return the current "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view>) array_filters() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) hint(v1::hint hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) sort(bsoncxx::v1::document::value sort);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one_options&) upsert(bool upsert);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    class internal;
};

///
/// Options for an "Update Many" operation in a client bulk write.
///
/// Supported fields include:
/// - `array_filters` ("arrayFilters")
/// - `collation`
/// - `hint`
/// - `upsert`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::update_many_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~update_many_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many_options(update_many_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) operator=(update_many_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many_options(update_many_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) operator=(update_many_options const& other);

    ///
    /// Default initialization.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many_options();

    ///
    /// Set the "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) array_filters(bsoncxx::v1::array::value array_filters);

    ///
    /// Return the current "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view>) array_filters() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) hint(v1::hint hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many_options&) upsert(bool upsert);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    class internal;
};

///
/// Options for a "Replace One" operation in a client bulk write.
///
/// Supported fields include:
/// - `collation`
/// - `hint`
/// - `sort`
/// - `upsert`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::replace_one_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
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
    MONGOCXX_ABI_EXPORT_CDECL() replace_one_options();

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) hint(v1::hint hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) sort(bsoncxx::v1::document::value sort);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one_options&) upsert(bool upsert);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    class internal;
};

///
/// Options for a "Delete One" operation in a client bulk write.
///
/// Supported fields include:
/// - `collation`
/// - `hint`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::delete_one_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~delete_one_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_one_options(delete_one_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one_options&) operator=(delete_one_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_one_options(delete_one_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one_options&) operator=(delete_one_options const& other);

    ///
    /// Default initialization.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_one_options();

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one_options&) hint(v1::hint hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    class internal;
};

///
/// Options for a "Delete Many" operation in a client bulk write.
///
/// Supported fields include:
/// - `collation`
/// - `hint`
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_bulk_write::delete_many_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~delete_many_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many_options(delete_many_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_options&) operator=(delete_many_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many_options(delete_many_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_options&) operator=(delete_many_options const& other);

    ///
    /// Default initialization.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many_options();

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_options&) collation(bsoncxx::v1::document::value collation);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_options&) hint(v1::hint hint);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides entities related to client bulk write operations.
///
