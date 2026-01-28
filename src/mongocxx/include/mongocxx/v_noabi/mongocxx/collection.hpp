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

#include <mongocxx/collection-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/collection.hpp> // IWYU pragma: export

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <string> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/bulk_write-fwd.hpp>
#include <mongocxx/client_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>          // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/builder/basic/kvp.hpp>      // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/builder/concatenate.hpp>    // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/oid.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/model/insert_one.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/model/write.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/options/change_stream.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/estimated_document_count.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/index.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/search_index_view.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A MongoDB collection.
///
/// Collections do not require or enforce a schema and documents inside of a collection can have
/// different fields. While not a requirement, typically documents in a collection have a similar
/// shape or related purpose.
///
/// @par Example
/// ```cpp
/// // Connect and get a collection.
/// mongocxx::v_noabi::client mongo_client{mongocxx::v_noabi::uri{}};
/// auto coll = mongo_client["database"]["collection"];
/// ```
///
class collection {
   private:
    v1::collection _coll;

    //
    // Utility class supporting the convenience of {} meaning an empty bsoncxx::v_noabi::document.
    //
    // Users may not use this class directly.
    //
    // In places where driver methods take this class as a parameter, passing {} will
    // translate to a default-constructed bsoncxx::v_noabi::document::view_or_value,
    // regardless of other overloads taking other default-constructible types
    // for that parameter. This class avoids compiler ambiguity with such overloads.
    //
    // See collection::update_one for an example of such overloads.
    //
    class _empty_doc_tag {};

   public:
    ///
    /// Default constructs a collection object. The collection is
    /// equivalent to the state of a moved from collection. The only
    /// valid actions to take with a default constructed collection
    /// are to assign to it, or destroy it.
    ///
    collection() noexcept {}

    ///
    /// Move constructs a collection.
    ///
    collection(collection&& other) noexcept = default;

    ///
    /// Move assigns a collection.
    ///
    collection& operator=(collection&& other) noexcept = default;

    ///
    /// Copy constructs a collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() collection(collection const& other);

    ///
    /// Copy assigns a collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(collection&) operator=(collection const& other);

    ///
    /// Destroys a collection.
    ///
    ~collection() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ collection(v1::collection collection) : _coll{std::move(collection)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::collection() && {
        return std::move(_coll);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::collection() const& {
        return _coll;
    }

    ///
    /// Returns true if the collection is valid, meaning it was not
    /// default constructed or moved from.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const noexcept;

    ///
    /// Runs an aggregation framework pipeline against this collection.
    ///
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::aggregate.
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    /// @note
    ///   In order to pass a read concern to this, you must use the
    ///   collection level set read concern - collection::read_concern(rc).
    ///   (Write concern supported only for MongoDB 3.4+).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    aggregate(v_noabi::pipeline const& pipeline, v_noabi::options::aggregate const& options = {});

    ///
    /// Runs an aggregation framework pipeline against this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the aggregation.
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::aggregate.
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/aggregate/
    ///
    /// @note
    ///   In order to pass a read concern to this, you must use the
    ///   collection level set read concern - collection::read_concern(rc).
    ///   (Write concern supported only for MongoDB 3.4+).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    aggregate(
        v_noabi::client_session const& session,
        v_noabi::pipeline const& pipeline,
        v_noabi::options::aggregate const& options = {});

    ///
    /// Creates a new bulk operation to be executed against this collection.
    /// The lifetime of the bulk_write is independent of the collection.
    ///
    /// @param options
    ///   Optional arguments; see mongocxx::v_noabi::options::bulk_write.
    ///
    /// @return
    ///    The newly-created bulk write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::bulk_write)
    create_bulk_write(v_noabi::options::bulk_write const& options = {});

    ///
    /// Creates a new bulk operation to be executed against this collection.
    /// The lifetime of the bulk_write is independent of the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the bulk operation.
    /// @param options
    ///   Optional arguments; see mongocxx::v_noabi::options::bulk_write.
    ///
    /// @return
    ///    The newly-created bulk write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::bulk_write)
    create_bulk_write(v_noabi::client_session const& session, v_noabi::options::bulk_write const& options = {});

    ///
    /// Sends a write to the server as a bulk write operation.
    ///
    /// @param write
    ///   A model::write.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return
    ///   The optional result of the bulk operation execution.
    ///   If the write concern is unacknowledged, the optional will be
    ///   disengaged.
    ///
    /// @exception
    ///   mongocxx::v_noabi::bulk_write_exception when there are errors processing
    ///   the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> write(
        v_noabi::model::write const& write,
        v_noabi::options::bulk_write const& options = {}) {
        return this->create_bulk_write(options).append(write).execute();
    }

    ///
    /// Sends a write to the server as a bulk write operation.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the bulk operation.
    /// @param write
    ///   A model::write.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return
    ///   The optional result of the bulk operation execution.
    ///   If the write concern is unacknowledged, the optional will be
    ///   disengaged.
    ///
    /// @exception
    ///   mongocxx::v_noabi::bulk_write_exception when there are errors processing
    ///   the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> write(
        v_noabi::client_session const& session,
        v_noabi::model::write const& write,
        v_noabi::options::bulk_write const& options = {}) {
        return this->create_bulk_write(session, options).append(write).execute();
    }

    ///
    /// Sends a container of writes to the server as a bulk write operation.
    ///
    /// @tparam container_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param writes
    ///   A container of model::write.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    template <typename container_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> bulk_write(
        container_type const& writes,
        v_noabi::options::bulk_write const& options = {}) {
        return this->bulk_write(writes.begin(), writes.end(), options);
    }

    ///
    /// Sends a container of writes to the server as a bulk write operation.
    ///
    /// @tparam container_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the bulk operation.
    /// @param writes
    ///   A container of model::write.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    template <typename container_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> bulk_write(
        v_noabi::client_session const& session,
        container_type const& writes,
        v_noabi::options::bulk_write const& options = {}) {
        return this->bulk_write(session, writes.begin(), writes.end(), options);
    }

    ///
    /// Sends writes starting at @c begin and ending at @c end to the server as a bulk write
    /// operation.
    ///
    /// @tparam write_model_iterator_type
    ///   The container type. Must meet the requirements for the input iterator concept with a value
    ///   type of model::write.
    ///
    /// @param begin
    ///   Iterator pointing to the first model::write to send.
    /// @param end
    ///   Iterator pointing to the end of the writes to send.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    template <typename write_model_iterator_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> bulk_write(
        write_model_iterator_type begin,
        write_model_iterator_type end,
        v_noabi::options::bulk_write const& options = {}) {
        auto writes = create_bulk_write(options);
        std::for_each(begin, end, [&](model::write const& current) { writes.append(current); });
        return writes.execute();
    }

    ///
    /// Sends writes starting at @c begin and ending at @c end to the server as a bulk write
    /// operation.
    ///
    /// @tparam write_model_iterator_type
    ///   The container type. Must meet the requirements for the input iterator concept with a value
    ///   type of model::write.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the bulk operation.
    /// @param begin
    ///   Iterator pointing to the first model::write to send.
    /// @param end
    ///   Iterator pointing to the end of the writes to send.
    /// @param options
    ///   Optional arguments, see v_noabi::options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::bulk_write
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    template <typename write_model_iterator_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> bulk_write(
        v_noabi::client_session const& session,
        write_model_iterator_type begin,
        write_model_iterator_type end,
        v_noabi::options::bulk_write const& options = {}) {
        auto writes = create_bulk_write(session, options);
        std::for_each(begin, end, [&](model::write const& current) { writes.append(current); });
        return writes.execute();
    }

    ///
    /// Counts the number of documents matching the provided filter.
    ///
    /// @param filter
    ///   The filter that documents must match in order to be counted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::count.
    ///
    /// @return The count of the documents that matched the filter.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the count operation fails.
    ///
    /// @note For a fast count of the total documents in a collection, see
    /// estimated_document_count().
    ///
    /// @note Due to an oversight in MongoDB server versions 5.0.0 through 5.0.7, the `count`
    /// command was not included in Stable API v1. Users of the Stable API with
    /// estimatedDocumentCount are recommended to upgrade their server version to 5.0.8 or newer, or
    /// set `apiStrict: false` to avoid encountering errors.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::collection::estimated_document_count
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t)
    count_documents(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::count const& options = {});

    ///
    /// Counts the number of documents matching the provided filter.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the count.
    /// @param filter
    ///   The filter that documents must match in order to be counted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::count.
    ///
    /// @return The count of the documents that matched the filter.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the count operation fails.
    ///
    /// @note Due to an oversight in MongoDB server versions 5.0.0 through 5.0.7, the `count`
    /// command was not included in Stable API v1. Users of the Stable API with
    /// estimatedDocumentCount are recommended to upgrade their server version to 5.0.8 or newer, or
    /// set `apiStrict: false` to avoid encountering errors.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::collection::estimated_document_count
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t)
    count_documents(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::count const& options = {});

    ///
    /// Returns an estimate of the number of documents in the collection.
    ///
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::count.
    ///
    /// @return The count of the documents that matched the filter.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the count operation fails.
    ///
    /// @note This function is implemented in terms of the count server command. See:
    /// https://www.mongodb.com/docs/manual/reference/command/count/#behavior for more information.
    ///
    /// @see
    /// - @ref mongocxx::v_noabi::collection::count_documents
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t)
    estimated_document_count(v_noabi::options::estimated_document_count const& options = {});

    ///
    /// Creates an index over the collection for the provided keys with the provided options.
    ///
    /// @param keys
    ///   The keys for the index: @c {a: 1, b: -1}
    /// @param index_options
    ///   A document containing optional arguments for creating the index.
    /// @param operation_options
    ///   Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if index creation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/createIndexes/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    bsoncxx::v_noabi::document::value create_index(
        bsoncxx::v_noabi::document::view_or_value keys,
        bsoncxx::v_noabi::document::view_or_value index_options = {},
        v_noabi::options::index_view operation_options = {}) {
        using namespace bsoncxx::v_noabi::builder::basic;

        auto const name_opt = this->indexes().create_one(keys, index_options, operation_options);

        return name_opt ? make_document(kvp("name", *name_opt)) : make_document();
    }

    ///
    /// Creates an index over the collection for the provided keys with the provided options.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the index creation.
    /// @param keys
    ///   The keys for the index: @c {a: 1, b: -1}
    /// @param index_options
    ///   A document containing optional arguments for creating the index.
    /// @param operation_options
    ///   Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if index creation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/createIndexes/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    bsoncxx::v_noabi::document::value create_index(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value keys,
        bsoncxx::v_noabi::document::view_or_value index_options = {},
        v_noabi::options::index_view operation_options = {}) {
        using namespace bsoncxx::v_noabi::builder::basic;

        auto const name_opt = this->indexes().create_one(session, keys, index_options, operation_options);

        return name_opt ? make_document(kvp("name", *name_opt)) : make_document();
    }

    ///
    /// Deletes all matching documents from the collection.
    ///
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the delete fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/delete/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result>)
    delete_many(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::delete_options const& options = {});

    ///
    /// Deletes all matching documents from the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the deletion.
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the delete fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/delete/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result>)
    delete_many(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::delete_options const& options = {});

    ///
    /// Deletes a single matching document from the collection.
    ///
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the delete fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/delete/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result>)
    delete_one(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::delete_options const& options = {});

    ///
    /// Deletes a single matching document from the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the deletion.
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::v_noabi::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the delete fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/delete/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result>)
    delete_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::delete_options const& options = {});

    ///
    /// Finds the distinct values for a specified field across the collection.
    ///
    /// @param name
    ///   The field for which the distinct values will be found.
    /// @param filter
    ///   Document view representing the documents for which the distinct operation will apply.
    /// @param options
    ///   Optional arguments, see v_noabi::options::distinct.
    ///
    /// @return mongocxx::v_noabi::cursor having the distinct values for the specified
    /// field.  If the operation fails, the cursor throws
    /// mongocxx::v_noabi::query_exception when the returned cursor is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/distinct/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    distinct(
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::distinct const& options = {});

    ///
    /// Finds the distinct values for a specified field across the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param name
    ///   The field for which the distinct values will be found.
    /// @param filter
    ///   Document view representing the documents for which the distinct operation will apply.
    /// @param options
    ///   Optional arguments, see v_noabi::options::distinct.
    ///
    /// @return mongocxx::v_noabi::cursor having the distinct values for the specified
    /// field.  If the operation fails, the cursor throws
    /// mongocxx::v_noabi::query_exception when the returned cursor is iterated.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/distinct/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    distinct(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::distinct const& options = {});

    ///
    /// Drops this collection and all its contained documents from the database.
    ///
    /// @param write_concern (optional)
    ///   The write concern to use for this operation. Defaults to the collection wide write
    ///   concern if none is provided.
    ///
    /// @param collection_options (optional)
    ///   Collection options to use for this operation.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/drop/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop(
        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern = {},
        bsoncxx::v_noabi::document::view_or_value collection_options = {});

    ///
    /// Drops this collection and all its contained documents from the database.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the drop.
    /// @param write_concern (optional)
    ///   The write concern to use for this operation. Defaults to the collection wide write
    ///   concern if none is provided.
    /// @param collection_options (optional)
    ///   Collection options to use for this operation.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/drop/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern = {},
        bsoncxx::v_noabi::document::view_or_value collection_options = {});

    ///
    /// Finds the documents in this collection which match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the options are invalid, or if the unsupported
    /// option modifiers "$query" or "$explain" are used.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-operations-introduction/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    find(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::find const& options = {});

    ///
    /// Finds the documents in this collection which match the provided filter.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the query.
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find
    ///
    /// @return A mongocxx::v_noabi::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::v_noabi::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the options are invalid, or if the unsupported
    /// option modifiers "$query" or "$explain" are used.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-operations-introduction/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor)
    find(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::find const& options = {});

    ///
    /// Finds a single document in this collection that match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find
    ///
    /// @return An optional document that matched the filter.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-operations-introduction/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one(bsoncxx::v_noabi::document::view_or_value filter, v_noabi::options::find const& options = {});

    ///
    /// Finds a single document in this collection that match the provided filter.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the query.
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find
    ///
    /// @return An optional document that matched the filter.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-operations-introduction/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::find const& options = {});

    ///
    /// Finds a single document matching the filter, deletes it, and returns the original.
    ///
    /// @param filter
    ///   Document view representing a document that should be deleted.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_delete
    ///
    /// @return The document that was deleted.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_delete(
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::find_one_and_delete const& options = {});

    ///
    /// Finds a single document matching the filter, deletes it, and returns the original.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param filter
    ///   Document view representing a document that should be deleted.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_delete
    ///
    /// @return The document that was deleted.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_delete(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::options::find_one_and_delete const& options = {});

    ///
    /// Finds a single document matching the filter, replaces it, and returns either the original
    /// or the replacement document.
    ///
    /// @param filter
    ///   Document view representing a document that should be replaced.
    /// @param replacement
    ///   Document view representing the replacement for a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_replace.
    ///
    /// @return The original or replaced document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_replace(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value replacement,
        v_noabi::options::find_one_and_replace const& options = {});

    ///
    /// Finds a single document matching the filter, replaces it, and returns either the original
    /// or the replacement document.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param filter
    ///   Document view representing a document that should be replaced.
    /// @param replacement
    ///   Document view representing the replacement for a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_replace.
    ///
    /// @return The original or replaced document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_replace(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value replacement,
        v_noabi::options::find_one_and_replace const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Document view representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Pipeline representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Document view representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Pipeline representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::logic_error if the collation option is specified and an
    ///   unacknowledged write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::v_noabi::write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>)
    find_one_and_update(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::find_one_and_update const& options = {});

    ///
    /// Inserts a single document into the collection. If the document is missing an identifier
    /// (@c _id field) one will be generated for it.
    ///
    /// @param document
    ///   The document to insert.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The optional result of attempting to perform the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_one>)
    insert_one(bsoncxx::v_noabi::document::view_or_value document, v_noabi::options::insert const& options = {});

    ///
    ///
    /// Inserts a single document into the collection. If the document is missing an identifier
    /// (@c _id field) one will be generated for it.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the insert.
    /// @param document
    ///   The document to insert.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The optional result of attempting to perform the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_one>)
    insert_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value document,
        v_noabi::options::insert const& options = {});

    ///
    /// Inserts multiple documents into the collection. If any of the documents are missing
    /// identifiers the driver will generate them.
    ///
    /// @warning This method uses the bulk insert command to execute the insertion as opposed to
    /// the legacy OP_INSERT wire protocol message. As a result, using this method to insert many
    /// documents on MongoDB < 2.6 will be slow.
    ///
    /// @tparam container_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param container
    ///   Container of a documents to insert.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The optional result of attempting to performing the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when the operation fails.
    ///
    template <typename container_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> insert_many(
        container_type const& container,
        v_noabi::options::insert const& options = {}) {
        return this->insert_many(container.begin(), container.end(), options);
    }

    ///
    /// Inserts multiple documents into the collection. If any of the documents are missing
    /// identifiers the driver will generate them.
    ///
    /// @tparam container_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the inserts.
    /// @param container
    ///   Container of a documents to insert.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The optional result of attempting to performing the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when the operation fails.
    ///
    template <typename container_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> insert_many(
        v_noabi::client_session const& session,
        container_type const& container,
        v_noabi::options::insert const& options = {}) {
        return this->insert_many(session, container.begin(), container.end(), options);
    }

    ///
    /// Inserts multiple documents into the collection. If any of the documents are missing
    /// identifiers the driver will generate them.
    ///
    /// @warning This method uses the bulk insert command to execute the insertion as opposed to
    /// the legacy OP_INSERT wire protocol message. As a result, using this method to insert many
    /// documents on MongoDB < 2.6 will be slow.
    ///
    /// @tparam document_view_iterator_type
    ///   The iterator type. Must meet the requirements for the input iterator concept with a value
    ///   type of bsoncxx::v_noabi::document::view.
    ///
    /// @param begin
    ///   Iterator pointing to the first document to be inserted.
    /// @param end
    ///   Iterator pointing to the end of the documents to be inserted.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The result of attempting to performing the insert.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    template <typename document_view_iterator_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> insert_many(
        document_view_iterator_type begin,
        document_view_iterator_type end,
        v_noabi::options::insert const& options = {}) {
        return this->_insert_many(nullptr, begin, end, options);
    }

    ///
    /// Inserts multiple documents into the collection. If any of the documents are missing
    /// identifiers the driver will generate them.
    ///
    /// @tparam document_view_iterator_type
    ///   The iterator type. Must meet the requirements for the input iterator concept with a value
    ///   type of bsoncxx::v_noabi::document::view.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the inserts.
    /// @param begin
    ///   Iterator pointing to the first document to be inserted.
    /// @param end
    ///   Iterator pointing to the end of the documents to be inserted.
    /// @param options
    ///   Optional arguments, see v_noabi::options::insert.
    ///
    /// @return The result of attempting to performing the insert.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    template <typename document_view_iterator_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> insert_many(
        v_noabi::client_session const& session,
        document_view_iterator_type begin,
        document_view_iterator_type end,
        v_noabi::options::insert const& options = {}) {
        return this->_insert_many(&session, begin, end, options);
    }

    ///
    ///
    /// Returns a list of the indexes currently on this collection.
    ///
    /// @return Cursor yielding the index specifications.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listIndexes/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor) list_indexes() const;

    ///
    /// Returns a list of the indexes currently on this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    ///
    /// @return Cursor yielding the index specifications.
    ///
    /// @throws mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/listIndexes/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor) list_indexes(v_noabi::client_session const& session) const;

    ///
    /// Returns the name of this collection.
    ///
    /// @return The name of the collection.  The return value of this method is invalidated by any
    /// subsequent call to collection::rename() on this collection object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::string_view) name() const;

    ///
    /// Rename this collection.
    ///
    /// @param new_name The new name to assign to the collection.
    /// @param drop_target_before_rename Whether to overwrite any
    ///   existing collections called new_name. The default is false.
    /// @param write_concern (optional)
    ///   The write concern to use for this operation. Defaults to the collection wide write
    ///   concern if none is provided.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/renameCollection/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    rename(
        bsoncxx::v_noabi::string::view_or_value new_name,
        bool drop_target_before_rename = false,
        bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern = {});

    ///
    /// Rename this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the rename.
    /// @param new_name The new name to assign to the collection.
    /// @param drop_target_before_rename Whether to overwrite any
    ///   existing collections called new_name. The default is false.
    /// @param write_concern (optional)
    ///   The write concern to use for this operation. Defaults to the collection wide write
    ///   concern if none is provided.
    ///
    /// @exception
    ///   mongocxx::v_noabi::operation_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/renameCollection/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    rename(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::string::view_or_value new_name,
        bool drop_target_before_rename = false,
        bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern = {});

    ///
    /// Sets the read_concern for this collection. Changes will not have any effect on existing
    /// cursors or other read operations which use the previously-set read concern.
    ///
    /// @param rc
    ///   The new @c read_concern
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/read-concern/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_concern(v_noabi::read_concern rc);

    ///
    /// Gets the read_concern for the collection.
    ///
    /// If a read_concern has not been explicitly set for this collection object, it inherits
    /// the read_concern from its parent database or client object.
    ///
    /// @return The current read_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::read_concern) read_concern() const;

    ///
    /// Sets the read_preference for this collection. Changes will not have any effect on existing
    /// cursors or other read operations which use the read preference.
    ///
    /// @param rp
    ///   The read_preference to set.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) read_preference(v_noabi::read_preference rp);

    ///
    /// Gets the read_preference for the collection.
    ///
    /// @return The current read_preference.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/read-preference/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::read_preference) read_preference() const;

    ///
    /// Replaces a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param replacement
    ///   The replacement document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::replace.
    ///
    /// @return The optional result of attempting to replace a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the replacement is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::replace_one>)
    replace_one(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value replacement,
        v_noabi::options::replace const& options = {});

    ///
    /// Replaces a single document matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the replace.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param replacement
    ///   The replacement document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::replace.
    ///
    /// @return The optional result of attempting to replace a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the replacement is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::replace_one>)
    replace_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value replacement,
        v_noabi::options::replace const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to matching documents.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Pipeline representing the update to be applied to matching documents.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to matching documents.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Pipeline representing the update to be applied to matching documents.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_many(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Pipeline representing the update to be applied to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        bsoncxx::v_noabi::document::view_or_value update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Pipeline representing the update to be applied to a matching document.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        v_noabi::pipeline const& update,
        v_noabi::options::update const& options = {});

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the update.
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Supports the empty update {}.
    /// @param options
    ///   Optional arguments, see v_noabi::options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::v_noabi::logic_error if the update is invalid, or
    ///   mongocxx::v_noabi::bulk_write_exception if the operation fails.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/update/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::update>)
    update_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value filter,
        std::initializer_list<_empty_doc_tag> update,
        v_noabi::options::update const& options = {});

    ///
    /// Sets the write_concern for this collection. Changes will not have any effect on existing
    /// write operations.
    ///
    /// @param wc
    ///   The new write_concern to use.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) write_concern(v_noabi::write_concern wc);

    ///
    /// Gets the write_concern for the collection.
    ///
    /// @return The current write_concern.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::write_concern) write_concern() const;

    ///
    /// Gets an index_view to the collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::index_view) indexes();

    ///
    ///
    /// Gets a change stream on this collection with an empty pipeline.
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::change_stream) watch(v_noabi::options::change_stream const& options = {});

    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::change_stream)
    watch(v_noabi::client_session const& session, v_noabi::options::change_stream const& options = {});

    ///
    /// Gets a change stream on this collection.
    /// Change streams are only supported with a "majority" read concern or no read concern.
    ///
    /// @param pipe
    ///   The aggregation pipeline to be used on the change notifications.
    ///   Only a subset of pipeline operations are supported for change streams. For more
    ///   information see the change streams documentation.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::change_stream)
    watch(v_noabi::pipeline const& pipe, v_noabi::options::change_stream const& options = {});

    ///
    /// Gets a change stream on this collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the watch operation.
    /// @param pipe
    ///   The aggregation pipeline to be used on the change notifications.
    /// @param options
    ///   The options to use when creating the change stream.
    ///
    /// @return
    ///  A change stream on this collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/changeStreams/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::change_stream)
    watch(
        v_noabi::client_session const& session,
        v_noabi::pipeline const& pipe,
        v_noabi::options::change_stream const& options = {});

    ///
    /// Gets a search_index_view to the collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::search_index_view) search_indexes();

    class internal;

   private:
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::bulk_write)
    _init_insert_many(v_noabi::options::insert const& options, client_session const* session);

    MONGOCXX_ABI_EXPORT_CDECL(void)
    _insert_many_doc_handler(
        mongocxx::v_noabi::bulk_write& writes,
        bsoncxx::v_noabi::builder::basic::array& inserted_ids,
        bsoncxx::v_noabi::document::view doc) const;

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many>)
    _exec_insert_many(v_noabi::bulk_write& writes, bsoncxx::v_noabi::builder::basic::array& inserted_ids);

    template <typename document_view_iterator_type>
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> _insert_many(
        client_session const* session,
        document_view_iterator_type begin,
        document_view_iterator_type end,
        v_noabi::options::insert const& options) {
        bsoncxx::v_noabi::builder::basic::array inserted_ids;
        auto writes = _init_insert_many(options, session);
        std::for_each(begin, end, [&inserted_ids, &writes, this](bsoncxx::v_noabi::document::view doc) {
            _insert_many_doc_handler(writes, inserted_ids, doc);
        });
        return _exec_insert_many(writes, inserted_ids);
    }
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::collection from_v1(v1::collection v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::collection to_v1(v_noabi::collection v) {
    return v1::collection{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::collection.
///
/// @par Includes
/// - @ref mongocxx/v1/collection.hpp
///
