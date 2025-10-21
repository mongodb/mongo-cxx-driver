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

#include <mongocxx/v1/bulk_write-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/pipeline.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <cstdint>
#include <map>
#include <utility>

namespace mongocxx {
namespace v1 {

///
/// A list of bulk write operations.
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write {
   public:
    ///
    /// Enumeration identifying the type of a write operation.
    ///
    enum class type {
        ///
        /// An "Insert One" operation.
        ///
        k_insert_one,

        ///
        /// A "Delete One" operation.
        ///
        k_delete_one,

        ///
        /// A "Delete Many" operation.
        ///
        k_delete_many,

        ///
        /// An "Update One" operation.
        ///
        k_update_one,

        ///
        /// An "Update Many" operation.
        ///
        k_update_many,

        ///
        /// A "Replace One" operation.
        ///
        k_replace_one,
    };

    class insert_one;
    class update_one;
    class update_many;
    class replace_one;
    class delete_one;
    class delete_many;
    class single;
    class options;
    class result;

    ///
    /// Return true when there are no appended operations.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) empty() const;

    ///
    /// Append a single write operation.
    ///
    /// @throws mongocxx::v1::exception when a client-side error is encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bulk_write&) append(single op);

    ///
    /// Execute the appended operations.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<result>) execute();
};

///
/// A single "Insert One" write operation.
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::insert_one {
   public:
    ///
    /// The document to be inserted.
    ///
    bsoncxx::v1::document::value value;

    ///
    /// Initialize with `doc`.
    ///
    /* explicit(false) */ insert_one(bsoncxx::v1::document::value doc) : value{std::move(doc)} {}

    ///
    /// Equivalent to `this->value`.
    ///
    bsoncxx::v1::document::value const& document() const {
        return value;
    }
};

///
/// A single "Update One" write operation.
///
/// Supported fields include:
/// - `array_filters` ("arrayFilters")
/// - `collation`
/// - `filter`
/// - `hint`
/// - `sort`
/// - `update`
/// - `upsert`
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::update_one {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~update_one();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one(update_one&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) operator=(update_one&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one(update_one const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) operator=(update_one const& other);

    ///
    /// Initialize with the given "filter" and "update" documents.
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one(bsoncxx::v1::document::value filter, bsoncxx::v1::document::value update);

    ///
    /// Initialize with the given "filter" document and "update" aggregation pipeline.
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_one(bsoncxx::v1::document::value filter, v1::pipeline update);

    ///
    /// Return the current "filter" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) filter() const;

    ///
    /// Return the current "update" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) update() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) hint(v1::hint v);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) sort(bsoncxx::v1::document::value v);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) upsert(bool v);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    ///
    /// Set the "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one&) array_filters(bsoncxx::v1::array::value v);

    ///
    /// Return the current "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view>) array_filters() const;
};

///
/// A single "Update Many" write operation.
///
/// Supported fields include:
/// - `array_filters` ("arrayFilters")
/// - `collation`
/// - `hint`
/// - `upsert`
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::update_many {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~update_many();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many(update_many&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) operator=(update_many&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many(update_many const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) operator=(update_many const& other);

    ///
    /// Initialize with the given "filter" and "update" documents.
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many(bsoncxx::v1::document::value filter, bsoncxx::v1::document::value update);

    ///
    /// Initialize with the given "filter" document and "update" aggregation pipeline.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() update_many(bsoncxx::v1::document::value filter, pipeline const& update);

    ///
    /// Return the current "filter" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) filter() const;

    ///
    /// Return the current "update" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) update() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) hint(v1::hint v);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) upsert(bool v);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;

    ///
    /// Set the "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many&) array_filters(bsoncxx::v1::array::value v);

    ///
    /// Return the current "arrayFilters" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view>) array_filters() const;
};

///
/// A single "Replace One" write operation.
///
/// Supported fields include:
/// - `collation`
/// - `filter`
/// - `hint`
/// - `replacement`
/// - `sort`
/// - `upsert`
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::replace_one {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~replace_one();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one(replace_one&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) operator=(replace_one&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one(replace_one const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) operator=(replace_one const& other);

    ///
    /// Initialize with the given "filter" and "replacement" documents.
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() replace_one(
        bsoncxx::v1::document::value filter,
        bsoncxx::v1::document::value replacement);

    ///
    /// Return the current "filter" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) filter() const;

    ///
    /// Return the current "replacement" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) replacement() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) hint(v1::hint v);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;

    ///
    /// Set the "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) sort(bsoncxx::v1::document::value v);

    ///
    /// Return the current "sort" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) sort() const;

    ///
    /// Set the "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&) upsert(bool v);

    ///
    /// Return the current "upsert" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) upsert() const;
};

///
/// A single "Delete One" write operation.
///
/// Supported fields include:
/// - `collation`
/// - `filter`
/// - `hint`
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::delete_one {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~delete_one();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_one(delete_one&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one&) operator=(delete_one&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_one(delete_one const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one&) operator=(delete_one const& other);

    ///
    /// Initialize with the given "filter".
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() delete_one(bsoncxx::v1::document::value filter);

    ///
    /// Return the current "filter" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) filter() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one&) hint(v1::hint v);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;
};

///
/// A single "Delete Many" write operation.
///
/// Supported fields include:
/// - `collation`
/// - `filter`
/// - `hint`
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::delete_many {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~delete_many();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many(delete_many&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many&) operator=(delete_many&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many(delete_many const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many&) operator=(delete_many const& other);

    ///
    /// Initialize with the given "filter".
    ///
    /// @par Postconditions:
    /// - All other supported fields are "unset" or zero-initialized.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() delete_many(bsoncxx::v1::document::value filter);

    ///
    /// Return the current "filter" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) filter() const;

    ///
    /// Set the "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many&) hint(v1::hint v);

    ///
    /// Return the current "hint" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::hint>) hint() const;
};

///
/// A single write operation.
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::single {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated references.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~single();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() single(single&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(single&) operator=(single&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() single(single const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(single&) operator=(single const& other);

    ///
    /// Initialize with `op`.
    ///
    /// @{
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(insert_one op);
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(update_one op);
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(update_many op);
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(delete_one op);
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(delete_many op);
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() single(replace_one op);
    /// @}
    ///

    ///
    /// Return the type of this write operation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write::type) type() const;

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::insert_one.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_insert_one`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(insert_one const&) get_insert_one() const&;
    MONGOCXX_ABI_EXPORT_CDECL(insert_one&&) get_insert_one() &&;
    /// @}
    ///

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::update_one.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_update_one`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(update_one const&) get_update_one() const&;
    MONGOCXX_ABI_EXPORT_CDECL(update_one&&) get_update_one() &&;
    /// @}
    ///

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::update_many.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_update_many`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(update_many const&) get_update_many() const&;
    MONGOCXX_ABI_EXPORT_CDECL(update_many&&) get_update_many() &&;
    /// @}
    ///

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::delete_one.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_delete_one`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(delete_one const&) get_delete_one() const&;
    MONGOCXX_ABI_EXPORT_CDECL(delete_one&&) get_delete_one() &&;
    /// @}
    ///

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::delete_many.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_delete_many`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(delete_many const&) get_delete_many() const&;
    MONGOCXX_ABI_EXPORT_CDECL(delete_many&&) get_delete_many() &&;
    /// @}
    ///

    ///
    /// Return this write operation as an @ref mongocxx::v1::bulk_write::replace_one.
    ///
    /// @par Preconditions:
    /// - `this->type() == mongocxx::v1::bulk_write::type::k_replace_one`
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(replace_one const&) get_replace_one() const&;
    MONGOCXX_ABI_EXPORT_CDECL(replace_one&&) get_replace_one() &&;
    /// @}
    ///
};

///
/// Options for a bulk write operation.
///
/// Supported fields include:
/// - `bypass_document_validation` ("bypassDocumentValidation")
/// - `comment`
/// - `let`
/// - `ordered`
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
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
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) bypass_document_validation(bool bypass_document_validation);

    ///
    /// Return the current "bypassDocumentValidation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool> const) bypass_document_validation() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const) comment() const;

    ///
    /// Set the "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) let(bsoncxx::v1::document::value let);

    ///
    /// Return the current "let" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) let() const;

    ///
    /// Set the "ordered" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) ordered(bool ordered);

    ///
    /// Return the current "ordered" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) ordered() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) write_concern(v1::write_concern wc);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
};

///
/// The result of a write operation.
///
/// Supported fields include:
/// - `inserted_count` ("nInserted" or "insertedCount")
/// - `matched_count` ("nMatched" or "matchedCount")
/// - `modified_count` ("nModified" or "modifiedCount")
/// - `deleted_count` ("nDeleted" or "deletedCount")
/// - `upserted_count` ("nUpserted" or "upsertedCount")
/// - `upserted_ids` ("upserted" or "upsertedIds")
///
/// @important The raw server response is translated by mongoc into the Bulk Write API specification format even when
/// the CRUD API specification is used to implement the requested operations (see: `MONGOC_WRITE_RESULT_COMPLETE`).
///
/// @see
/// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
/// - [Bulk Write API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/bulk-write/)
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write::result {
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
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() result();

    ///
    /// Return the value of the mongoc "nInserted" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) inserted_count() const;

    ///
    /// Return the value of the mongoc "nMatched" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) matched_count() const;

    ///
    /// Return the value of the mongoc "nModified" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) modified_count() const;

    ///
    /// Return the value of the mongoc "nDeleted" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) deleted_count() const;

    ///
    /// Return the value of the mongoc "nUpserted" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) upserted_count() const;

    ///
    /// A map from the operation index to the upserted document ID.
    ///
    using id_map = std::map<std::int64_t, bsoncxx::v1::types::view>;

    ///
    /// Return a map from the operation index to the upserted document ID.
    ///
    /// @returns Empty when the "upserted" field is not present or is empty.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(id_map) upserted_ids() const;

    ///
    /// Compare equal when `lhs.raw()` and `rhs.raw()` compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(result const& lhs, result const& rhs);

    friend bool operator!=(result const& lhs, result const& rhs) {
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
/// Provides entities related to write operations.
///
