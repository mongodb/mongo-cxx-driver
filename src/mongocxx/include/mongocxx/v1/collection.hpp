//
// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/collection-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/delete_many_result-fwd.hpp>
#include <mongocxx/v1/delete_one_result-fwd.hpp>
#include <mongocxx/v1/insert_one_result-fwd.hpp>
#include <mongocxx/v1/pipeline-fwd.hpp>
#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>
#include <mongocxx/v1/replace_one_result-fwd.hpp>
#include <mongocxx/v1/search_indexes-fwd.hpp>
#include <mongocxx/v1/update_many_result-fwd.hpp>
#include <mongocxx/v1/update_one_result-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/aggregate_options.hpp>
#include <mongocxx/v1/bulk_write.hpp>
#include <mongocxx/v1/change_stream.hpp>
#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/count_options.hpp>
#include <mongocxx/v1/delete_many_options.hpp>
#include <mongocxx/v1/delete_one_options.hpp>
#include <mongocxx/v1/distinct_options.hpp>
#include <mongocxx/v1/estimated_document_count_options.hpp>
#include <mongocxx/v1/find_one_and_delete_options.hpp>
#include <mongocxx/v1/find_one_and_replace_options.hpp>
#include <mongocxx/v1/find_one_and_update_options.hpp>
#include <mongocxx/v1/find_options.hpp>
#include <mongocxx/v1/indexes.hpp>
#include <mongocxx/v1/insert_many_options.hpp>
#include <mongocxx/v1/insert_many_result.hpp>
#include <mongocxx/v1/insert_one_options.hpp>
#include <mongocxx/v1/replace_one_options.hpp>
#include <mongocxx/v1/update_many_options.hpp>
#include <mongocxx/v1/update_one_options.hpp>

#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB collection.
///
/// @see
/// - [Databases and Collections in MongoDB (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/databases-and-collections/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class collection {
   private:
    class impl;
    void* _impl;

    template <typename Container>
    using has_begin_expr = decltype((std::declval<Container const&>().begin()));

    template <typename Container>
    struct has_begin : bsoncxx::detail::is_detected<has_begin_expr, Container> {};

    template <typename Container>
    using has_end_expr = decltype((std::declval<Container const&>().end()));

    template <typename Container>
    struct has_end : bsoncxx::detail::is_detected<has_end_expr, Container> {};

    static void is_write_iter_expr_impl(v1::bulk_write::single) {}

    template <typename InputIt>
    using is_write_iter_expr = decltype((is_write_iter_expr_impl)(*std::declval<InputIt&>()));

    template <typename InputIt>
    struct is_write_iter : bsoncxx::detail::is_detected<is_write_iter_expr, InputIt> {};

    template <typename Sentinel, typename InputIt>
    struct is_sentinel_for : bsoncxx::detail::is_equality_comparable<Sentinel, InputIt> {};

    template <typename Container>
    struct is_container : bsoncxx::detail::conjunction<has_begin<Container>, has_end<Container>> {};

    static void is_document_iter_expr_impl(bsoncxx::v1::document::view) {}

    template <typename InputIt>
    using is_document_iter_expr = decltype((is_document_iter_expr_impl)(*std::declval<InputIt&>()));

    template <typename InputIt>
    struct is_document_iter : bsoncxx::detail::is_detected<is_document_iter_expr, InputIt> {};

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views, cursors, bulk writes, indexes, and change streams.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~collection();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() collection(collection&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(collection&) operator=(collection&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() collection(collection const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(collection&) operator=(collection const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() collection();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Run an aggregation framework pipeline.
    ///
    /// @see
    /// - [`aggregate` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/aggregate/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) aggregate(
        v1::pipeline const& pipeline,
        v1::aggregate_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    aggregate(v1::client_session const& session, v1::pipeline const& pipeline, v1::aggregate_options const& opts = {});
    /// @}
    ///

    ///
    /// Create a bulk write operation.
    ///
    /// The bulk write operation must be executed with @ref mongocxx::v1::bulk_write::execute().
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write) create_bulk_write(v1::bulk_write::options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write) create_bulk_write(
        v1::client_session const& session,
        v1::bulk_write::options const& opts = {});
    /// @}
    ///

    ///
    /// Equivalent to `this->bulk_write(...)` with a single write operation.
    ///
    /// @{
    bsoncxx::v1::stdx::optional<v1::bulk_write::result> write(
        v1::bulk_write::single const& write,
        v1::bulk_write::options const& opts = {}) {
        return this->create_bulk_write(opts).append(write).execute();
    }

    bsoncxx::v1::stdx::optional<v1::bulk_write::result> write(
        v1::client_session const& session,
        v1::bulk_write::single const& write,
        v1::bulk_write::options const& opts = {}) {
        return this->create_bulk_write(session, opts).append(write).execute();
    }
    /// @}
    ///

    ///
    /// Equivalent to `this->bulk_write(writes.begin(), writes.begin(), opts)`.
    ///
    /// @par Constraints:
    /// - `writes.begin()` and `writes.end()` must be valid.
    ///
    template <typename Container, bsoncxx::detail::enable_if_t<is_container<Container>::value>* = nullptr>
    bsoncxx::v1::stdx::optional<v1::bulk_write::result> bulk_write(
        Container const& writes,
        v1::bulk_write::options const& opts = {}) {
        return this->bulk_write(writes.begin(), writes.end(), opts);
    }

    ///
    /// Equivalent to `this->bulk_write(session, writes.begin(), writes.begin(), opts)`.
    ///
    /// @par Constraints:
    /// - `writes.begin()` and `writes.end()` must be valid.
    ///
    template <typename Container, bsoncxx::detail::enable_if_t<is_container<Container>::value>* = nullptr>
    bsoncxx::v1::stdx::optional<v1::bulk_write::result>
    bulk_write(v1::client_session const& session, Container const& writes, v1::bulk_write::options const& opts = {}) {
        return this->bulk_write(session, writes.begin(), writes.end(), opts);
    }

    ///
    /// Execute multiple write operations.
    ///
    /// Equivalent to:
    /// ```cpp
    /// auto bulk = this->create_bulk_write(...);
    /// for (auto iter = begin; iter != end; ++iter) {
    ///     bulk.append(*iter);
    /// }
    /// return bulk.execute();
    /// ```
    ///
    /// @par Constraints:
    /// - `InputIt` satisfies Cpp17InputIterator.
    /// - The value type of `InputIt` is convertible to @ref mongocxx::v1::bulk_write::single.
    /// - `Sentinel` satisfies `std::sentinel_for<Sentinel, InputIt>`.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    template <
        typename InputIt,
        typename Sentinel,
        bsoncxx::detail::enable_if_t<is_write_iter<InputIt>::value && is_sentinel_for<Sentinel, InputIt>::value>* =
            nullptr>
    bsoncxx::v1::stdx::optional<v1::bulk_write::result>
    bulk_write(InputIt begin, Sentinel end, v1::bulk_write::options const& opts = {}) {
        v1::bulk_write bulk{this->create_bulk_write(opts)};
        for (auto iter = begin; iter != end; ++iter) {
            bulk.append(*iter);
        }
        return bulk.execute();
    }

    template <
        typename InputIt,
        typename Sentinel,
        bsoncxx::detail::enable_if_t<is_write_iter<InputIt>::value && is_sentinel_for<Sentinel, InputIt>::value>* =
            nullptr>
    bsoncxx::v1::stdx::optional<v1::bulk_write::result> bulk_write(
        v1::client_session const& session,
        InputIt begin,
        Sentinel end,
        v1::bulk_write::options const& opts = {}) {
        v1::bulk_write bulk{this->create_bulk_write(session, opts)};
        for (auto iter = begin; iter != end; ++iter) {
            bulk.append(*iter);
        }
        return bulk.execute();
    }
    /// @}
    ///

    ///
    /// Return the number of documents in this collection that match the given query filter.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
    /// - [`$group` (aggregation stage) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/operator/aggregation/group/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) count_documents(
        bsoncxx::v1::document::view filter,
        v1::count_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t)
    count_documents(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        v1::count_options const& opts = {});
    /// @}
    ///

    ///
    /// Return an estimate of the number of documents in this collection using collection metadata.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
    /// - [`count` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/count/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) estimated_document_count(
        v1::estimated_document_count_options const& opts = {});

    ///
    /// Equivalent to `this->indexes().create_one(...)`.
    ///
    /// @{
    bsoncxx::v1::stdx::optional<std::string> create_index(
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value create_opts = {},
        v1::indexes::options opts = {}) {
        return this->indexes().create_one(keys, create_opts, opts);
    }

    bsoncxx::v1::stdx::optional<std::string> create_index(
        v1::client_session const& session,
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value create_opts = {},
        v1::indexes::options opts = {}) {
        return this->indexes().create_one(session, keys, create_opts, opts);
    }
    /// @}
    ///

    ///
    /// Delete multiple documents in this collection using the given query filter.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`delete` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/delete/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::delete_many_result>) delete_many(
        bsoncxx::v1::document::view q,
        v1::delete_many_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::delete_many_result>) delete_many(
        v1::client_session const& session,
        bsoncxx::v1::document::view q,
        v1::delete_many_options const& opts = {});
    /// @}
    ///

    ///
    /// Delete a single document in this collection using the given query filter.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`delete` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/delete/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::delete_one_result>) delete_one(
        bsoncxx::v1::document::view q,
        v1::delete_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::delete_one_result>)
    delete_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view q,
        v1::delete_one_options const& opts = {});
    /// @}
    ///

    ///
    /// Return the distinct values for the specified field within this collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`distinct` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/distinct/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    distinct(
        bsoncxx::v1::stdx::string_view key,
        bsoncxx::v1::document::view query,
        v1::distinct_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) distinct(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view key,
        bsoncxx::v1::document::view query,
        v1::distinct_options const& opts = {});
    /// @}
    ///

    ///
    /// Drop this collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`drop` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/atlas/data-federation/admin/cli/collections/drop-collections-views/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) drop(
        bsoncxx::v1::stdx::optional<v1::write_concern> const& wc = {},
        bsoncxx::v1::document::view opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(void) drop(
        v1::client_session const& session,
        bsoncxx::v1::stdx::optional<v1::write_concern> const& wc = {},
        bsoncxx::v1::document::view opts = {});
    /// @}
    ///

    ///
    /// Find documents in this collection matching the given query filter.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::collection::errc::max_await_time_u32 if the
    /// "maxAwaitTimeMS" field is not representable as an `std::uint32_t`.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`find` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/find/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) find(bsoncxx::v1::document::view filter, v1::find_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    find(v1::client_session const& session, bsoncxx::v1::document::view filter, v1::find_options const& opts = {});
    /// @}
    ///

    ///
    /// Return the first matching document obtained by `this->find(...)`.
    ///
    /// @returns Empty when no matching document was found.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::collection::errc::max_await_time_u32 if the
    /// "maxAwaitTimeMS" field is not representable as an `std::uint32_t`.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`find` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/find/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one(
        bsoncxx::v1::document::view filter,
        v1::find_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>)
    find_one(v1::client_session const& session, bsoncxx::v1::document::view filter, v1::find_options const& opts = {});
    /// @}
    ///

    ///
    /// Find a single document in this collection and delete it, returning the original document.
    ///
    /// @returns Empty when no matching document was found.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`findAndModify` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/findAndModify/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_delete(
        bsoncxx::v1::document::view query,
        v1::find_one_and_delete_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_delete(
        v1::client_session const& session,
        bsoncxx::v1::document::view query,
        v1::find_one_and_delete_options const& opts = {});
    /// @}
    ///

    ///
    /// Find a single document in this collection and replace it.
    ///
    /// @returns The document before or after replacement, depending on the "returnDocument" field. Empty when no
    /// matching document was found.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`findAndModify` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/findAndModify/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_replace(
        bsoncxx::v1::document::view query,
        bsoncxx::v1::document::view replacement,
        v1::find_one_and_replace_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_replace(
        v1::client_session const& session,
        bsoncxx::v1::document::view query,
        bsoncxx::v1::document::view replacement,
        v1::find_one_and_replace_options const& opts = {});
    /// @}
    ///

    ///
    /// Find a single document in this collection and update it.
    ///
    /// @returns The document before or after the update, depending on the "returnDocument" field. Empty when no
    /// matching document was found.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [`findAndModify` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/findAndModify/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_update(
        bsoncxx::v1::document::view query,
        bsoncxx::v1::document::view update,
        v1::find_one_and_update_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_update(
        bsoncxx::v1::document::view query,
        v1::pipeline const& update,
        v1::find_one_and_update_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_update(
        v1::client_session const& session,
        bsoncxx::v1::document::view query,
        bsoncxx::v1::document::view update,
        v1::find_one_and_update_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>) find_one_and_update(
        v1::client_session const& session,
        bsoncxx::v1::document::view query,
        v1::pipeline const& update,
        v1::find_one_and_update_options const& opts = {});
    /// @}
    ///

    ///
    /// Insert a single document into this collection as a bulk write operation.
    ///
    /// If the document does not contain an "_id" field, an "_id" field is generated using @ref bsoncxx::v1::oid.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::insert_one_result>) insert_one(
        bsoncxx::v1::document::view document,
        v1::insert_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::insert_one_result>) insert_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view document,
        v1::insert_one_options const& opts = {});
    /// @}
    ///

    ///
    /// Equivalent to `this->insert_many(docs.begin(), docs.end(), opts)`.
    ///
    template <typename Container, bsoncxx::detail::enable_if_t<is_container<Container>::value>* = nullptr>
    bsoncxx::v1::stdx::optional<v1::insert_many_result> insert_many(
        Container const& docs,
        v1::insert_many_options const& opts = {}) {
        return this->insert_many(docs.begin(), docs.end(), opts);
    }

    ///
    /// Equivalent to `this->insert_many(session, docs.begin(), docs.end(), opts)`.
    ///
    template <typename Container, bsoncxx::detail::enable_if_t<is_container<Container>::value>* = nullptr>
    bsoncxx::v1::stdx::optional<v1::insert_many_result>
    insert_many(v1::client_session const& session, Container const& docs, v1::insert_many_options const& opts = {}) {
        return this->insert_many(session, docs.begin(), docs.end(), opts);
    }

    ///
    /// Insert the given documents into this collection as a bulk write operation.
    ///
    /// If the document(s) do not contain an "_id" field, an "_id" field is generated using @ref bsoncxx::v1::oid.
    ///
    /// @par Constraints:
    /// - `InputIt` satisfies Cpp17InputIterator.
    /// - The value type of `InputIt` is convertible to @ref bsoncxx::v1::document::view.
    /// - `Sentinel` satisfies `std::is_sentinel_for<Sentinel, InputIt>`.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    template <
        typename InputIt,
        typename Sentinel,
        bsoncxx::detail::enable_if_t<is_document_iter<InputIt>::value && is_sentinel_for<Sentinel, InputIt>::value>* =
            nullptr>
    bsoncxx::v1::stdx::optional<v1::insert_many_result>
    insert_many(InputIt begin, Sentinel end, v1::insert_many_options const& opts = {}) {
        v1::bulk_write bulk{this->_create_insert_many(nullptr, opts)};
        std::vector<bsoncxx::v1::types::value> inserted_ids;
        for (auto iter = begin; iter != end; ++iter) {
            this->_append_insert_many(bulk, inserted_ids, *iter);
        }
        return this->_execute_insert_many(bulk, inserted_ids);
    }

    template <
        typename InputIt,
        typename Sentinel,
        bsoncxx::detail::enable_if_t<is_document_iter<InputIt>::value && is_sentinel_for<Sentinel, InputIt>::value>* =
            nullptr>
    bsoncxx::v1::stdx::optional<v1::insert_many_result> insert_many(
        v1::client_session const& session,
        InputIt begin,
        Sentinel end,
        v1::insert_many_options const& opts = {}) {
        v1::bulk_write bulk{this->_create_insert_many(&session, opts)};
        std::vector<bsoncxx::v1::types::value> inserted_ids;
        for (auto iter = begin; iter != end; ++iter) {
            this->_append_insert_many(bulk, inserted_ids, *iter);
        }
        return this->_execute_insert_many(bulk, inserted_ids);
    }
    /// @}
    ///

    ///
    /// Equivalent to `this->indexes.list()`.
    ///
    v1::cursor list_indexes() {
        return this->indexes().list();
    }

    ///
    /// Equivalent to `this->indexes.list(session)`.
    ///
    v1::cursor list_indexes(v1::client_session const& session) {
        return this->indexes().list(session);
    }

    ///
    /// Return the name of this collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) name() const;

    ///
    /// Change the name of this collection.
    ///
    /// @throws mongocxx::v1::exception when a client-side or server-side error is encountered. (mongoc does not return
    /// the raw server response for this command.)
    ///
    /// @see
    /// - [`renameCollection` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/renamecollection/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) rename(
        bsoncxx::v1::stdx::string_view new_name,
        bool drop_target = false,
        bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern = {});

    MONGOCXX_ABI_EXPORT_CDECL(void) rename(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view new_name,
        bool drop_target = false,
        bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern = {});
    /// @}
    ///

    ///
    /// Set the default "readConcern" to use for operations on this collection.
    ///
    /// Overrides the default inherited from the associated database or client. May be overridden by individual
    /// operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_concern(v1::read_concern const& rc);

    ///
    /// Return the current default "readConcern" to use for operations on this collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_concern) read_concern() const;

    ///
    /// Set the default "readPreference" to use for operations on this collection.
    ///
    /// Overrides the default inherited from the associated database or client. May be overridden by individual
    /// operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_preference(v1::read_preference const& rp);

    ///
    /// Return the current default "readPreference" to use for operations on this collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::read_preference) read_preference() const;

    ///
    /// Replace a single document in this collection as a bulk write operation.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::replace_one_result>) replace_one(
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::value replacement,
        v1::replace_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::replace_one_result>) replace_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::value replacement,
        v1::replace_one_options const& opts = {});
    /// @}
    ///

    ///
    /// Update multiple documents in this collection as a bulk write operation.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_many_result>) update_many(
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::view update,
        v1::update_many_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_many_result>)
    update_many(bsoncxx::v1::document::view filter, v1::pipeline const& update, update_many_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_many_result>) update_many(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::view update,
        v1::update_many_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_many_result>) update_many(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        v1::pipeline const& update,
        v1::update_many_options const& opts = {});
    /// @}
    ///

    ///
    /// Update a single document in this collection as a bulk write operation.
    ///
    /// @returns Empty when the bulk write operation is unacknowledged.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_one_result>) update_one(
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::view update,
        v1::update_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_one_result>)
    update_one(bsoncxx::v1::document::view filter, v1::pipeline const& update, update_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_one_result>) update_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        bsoncxx::v1::document::view update,
        v1::update_one_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::update_one_result>) update_one(
        v1::client_session const& session,
        bsoncxx::v1::document::view filter,
        v1::pipeline const& update,
        v1::update_one_options const& opts = {});
    /// @}
    ///

    ///
    /// Set the default "writeConcern" to use for operations on this collection.
    ///
    /// Overrides the default inherited from the associated database or client. May be overridden by individual
    /// operations.
    ///
    /// @see
    /// - [Default MongoDB Read Concerns/Write Concerns (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/mongodb-defaults/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write_concern(v1::write_concern const& wc);

    ///
    /// Return the current default "writeConcern" to use for operations on this collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::write_concern) write_concern() const;

    ///
    /// Manage indexes associated with this collection.
    ///
    /// @see
    /// - [Indexes (MongoDB Manual)](https://www.mongodb.com/docs/manual/indexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::indexes) indexes();

    ///
    /// Return a change stream subscribed to this collection.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(v1::change_stream::options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::client_session const& session,
        v1::change_stream::options const& opts = {});
    /// @}
    ///

    ///
    /// Return a change stream subscribed to this collection with events filtered/modified by `pipeline`.
    ///
    /// @see
    /// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream) watch(
        v1::pipeline const& pipeline,
        v1::change_stream::options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::change_stream)
    watch(v1::client_session const& session, v1::pipeline const& pipeline, v1::change_stream::options const& opts = {});
    /// @}
    ///

    ///
    /// Manage Atlas Search Indexes associated with this collection.
    ///
    /// @see
    /// - [Manage MongoDB Search Indexes (MongoDB Manual)](https://www.mongodb.com/docs/atlas/atlas-search/manage-indexes/)
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::search_indexes) search_indexes();

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::collection.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,               ///< Zero.
        max_await_time_u32, ///< The "maxAwaitTimeMS" field must be representable as an `std::uint32_t`.
        max_time_u32,       ///< The "maxTimeMS" field must be representable as an `std::uint32_t`.
    };

    ///
    /// The error category for @ref mongocxx::v1::collection::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }

    class internal;

   private:
    /* explicit(false) */ collection(void* impl);

    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write) _create_insert_many(
        v1::client_session const* session,
        v1::insert_many_options const& opts);

    static MONGOCXX_ABI_EXPORT_CDECL(void) _append_insert_many(
        v1::bulk_write& bulk,
        std::vector<bsoncxx::v1::types::value>& inserted_ids,
        bsoncxx::v1::document::view doc);

    static MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::insert_many_result>) _execute_insert_many(
        v1::bulk_write& bulk,
        std::vector<bsoncxx::v1::types::value>& inserted_ids);
};

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::collection::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::collection.
///
