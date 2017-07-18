// Copyright 2014 MongoDB Inc.
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

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/options/index_view.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::concatenate;

class client;
class database;
class pipeline;

///
/// Class representing server side document groupings within a MongoDB database.
///
/// Collections do not require or enforce a schema and documents inside of a collection can have
/// different fields. While not a requirement, typically documents in a collection have a similar
/// shape or related purpose.
///
/// Example:
/// @code
///   // Connect and get a collection.
///   mongocxx::client mongo_client{mongocxx::uri{}};
///   auto coll = mongo_client["database"]["collection"];
/// @endcode
///
class MONGOCXX_API collection {
   public:
    ///
    /// Default constructs a collection object. The collection is
    /// equivalent to the state of a moved from collection. The only
    /// valid actions to take with a default constructed collection
    /// are to assign to it, or destroy it.
    ///
    collection() noexcept;

    ///
    /// Move constructs a collection.
    ///
    collection(collection&&) noexcept;

    ///
    /// Move assigns a collection.
    ///
    collection& operator=(collection&&) noexcept;

    ///
    /// Copy constructs a collection.
    ///
    collection(const collection&);

    ///
    /// Copy assigns a collection.
    ///
    collection& operator=(const collection&);

    ///
    /// Destroys a collection.
    ///
    ~collection();

    ///
    /// Returns true if the collection is valid, meaning it was not
    /// default constructed or moved from.
    ///
    explicit operator bool() const noexcept;

    ///
    /// Runs an aggregation framework pipeline against this collection.
    ///
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongocxx::options::aggregate.
    ///
    /// @return A mongocxx::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/aggregate/
    ///
    /// @note
    ///   In order to pass a read concern to this, you must use the
    ///   collection level set read concern - collection::read_concern(rc).
    ///   (Write concern supported only for MongoDB 3.4+).
    ///
    cursor aggregate(const pipeline& pipeline,
                     const options::aggregate& options = options::aggregate());

    ///
    /// Creates a new bulk operation to be executed against this collection.
    ///
    /// @param options
    ///   Optional arguments; see mongocxx::options::bulk_write.
    ///
    /// @return
    ///    The newly-created bulk write.
    ///
    class bulk_write create_bulk_write(const options::bulk_write& options = {});

    ///
    /// Sends a write to the server as a bulk write operation.
    ///
    /// @param write
    ///   A model::write.
    /// @param options
    ///   Optional arguments, see options::bulk_write.
    ///
    /// @return
    ///   The optional result of the bulk operation execution.
    ///   If the write concern is unacknowledged, the optional will be
    ///   disengaged.
    //
    /// @exception
    ///   mongocxx::bulk_write_exception when there are errors processing
    ///   the writes.
    ///
    /// @see mongocxx::bulk_write
    /// @see https://docs.mongodb.com/master/core/bulk-write-operations/
    ///
    MONGOCXX_INLINE stdx::optional<result::bulk_write> write(
        const model::write& write, const options::bulk_write& options = options::bulk_write());

    ///
    /// Sends a container of writes to the server as a bulk write operation.
    ///
    /// @tparam containter_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param writes
    ///   A container of model::write.
    /// @param options
    ///   Optional arguments, see options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    //
    /// @throws mongocxx::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see mongocxx::bulk_write
    /// @see https://docs.mongodb.com/master/core/bulk-write-operations/
    ///
    template <typename container_type>
    MONGOCXX_INLINE stdx::optional<result::bulk_write> bulk_write(
        const container_type& writes, const options::bulk_write& options = options::bulk_write());

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
    ///   Optional arguments, see options::bulk_write.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    ///
    /// @throws mongocxx::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see mongocxx::bulk_write
    /// @see https://docs.mongodb.com/master/core/bulk-write-operations/
    ///
    template <typename write_model_iterator_type>
    MONGOCXX_INLINE stdx::optional<result::bulk_write> bulk_write(
        write_model_iterator_type begin,
        write_model_iterator_type end,
        const options::bulk_write& options = options::bulk_write());

    ///
    /// Sends a batch of writes represented by the bulk_write to the server.
    ///
    /// @param bulk_write
    ///   A bulk write which contains multiple write operations.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    ///
    /// @throws mongocxx::bulk_write_exception when there are errors processing the writes.
    ///
    /// @see https://docs.mongodb.com/master/core/bulk-write-operations/
    ///
    stdx::optional<result::bulk_write> bulk_write(const class bulk_write& bulk_write);

    ///
    /// Counts the number of documents matching the provided filter.
    ///
    /// @param filter
    ///   The filter that documents must match in order to be counted.
    /// @param options
    ///   Optional arguments, see mongocxx::options::count.
    ///
    /// @return The count of the documents that matched the filter.
    ///
    /// @throws mongocxx::query_exception if the count operation fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/count/
    ///
    std::int64_t count(bsoncxx::document::view_or_value filter,
                       const options::count& options = options::count());

    ///
    /// Creates an index over the collection for the provided keys with the provided options.
    ///
    /// @param keys
    ///   The keys for the index: @c {a: 1, b: -1}
    /// @param index_options
    ///   A document containing optional arguments for creating the index.
    /// @param operation_options
    ///   Optional arguments for the overall operation, see mongocxx::options::index_view.
    ///
    /// @exception
    ///   mongocxx::operation_exception if index creation fails.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/createIndexes/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    bsoncxx::document::value create_index(
        bsoncxx::document::view_or_value keys,
        bsoncxx::document::view_or_value index_options = {},
        options::index_view operation_options = options::index_view{});

    ///
    /// Deletes all matching documents from the collection.
    ///
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::bulk_write_exception if the delete fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/delete/
    ///
    stdx::optional<result::delete_result> delete_many(
        bsoncxx::document::view_or_value filter,
        const options::delete_options& options = options::delete_options());

    ///
    /// Deletes a single matching document from the collection.
    ///
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongocxx::options::delete_options.
    ///
    /// @return The optional result of performing the deletion.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::bulk_write_exception if the delete fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/delete/
    ///
    stdx::optional<result::delete_result> delete_one(
        bsoncxx::document::view_or_value filter,
        const options::delete_options& options = options::delete_options());

    ///
    /// Finds the distinct values for a specified field accross the collection.
    ///
    /// @param field_name
    ///   The field for which the distinct values will be found.
    /// @param filter
    ///   Document view representing the documents for which the distinct operation will apply.
    /// @param options
    ///   Optional arguments, see options::distinct.

    /// @return mongocxx::cursor having the distinct values for the specified
    /// field.  If the operation fails, the cursor throws
    /// mongocxx::query_exception when the returned cursor is iterated.

    /// @see https://docs.mongodb.com/master/reference/command/distinct/
    ///
    cursor distinct(bsoncxx::string::view_or_value name,
                    bsoncxx::document::view_or_value filter,
                    const options::distinct& options = options::distinct());

    /// Drops this collection and all its contained documents from the database.
    ///
    /// @param write_concern (optional)
    ///   The write concern to use for this operation. Defaults to the collection wide write
    ///   concern if none is provided.
    ///
    /// @exception
    ///   mongocxx::operation_exception if the operation fails.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/drop/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    void drop(const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern = {});

    ///
    /// Finds the documents in this collection which match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see options::find
    ///
    /// @return A mongocxx::cursor with the results.  If the query fails,
    /// the cursor throws mongocxx::query_exception when the returned cursor
    /// is iterated.
    ///
    /// @throws mongocxx::logic_error if the options are invalid, or if the unsupported option
    /// modifiers "$query" or "$explain" are used.
    ///
    /// @see https://docs.mongodb.com/master/core/read-operations-introduction/
    ///
    cursor find(bsoncxx::document::view_or_value filter,
                const options::find& options = options::find());

    ///
    /// Finds a single document in this collection that match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see options::find
    ///
    /// @return An optional document that matched the filter.
    ///
    /// @throws mongocxx::query_exception if the operation fails.
    ///
    /// @see https://docs.mongodb.com/master/core/read-operations-introduction/
    ///
    stdx::optional<bsoncxx::document::value> find_one(
        bsoncxx::document::view_or_value filter, const options::find& options = options::find());

    ///
    /// Finds a single document matching the filter, deletes it, and returns the original.
    ///
    /// @param filter
    ///   Document view representing a document that should be deleted.
    /// @param options
    ///   Optional arguments, see options::find_one_and_delete
    ///
    /// @return The document that was deleted.
    ///
    /// @exception
    ///   Throws mongocxx::logic_error if the collation option is specified and an unacknowledged
    ///   write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::write_exception if the operation fails.
    ///
    stdx::optional<bsoncxx::document::value> find_one_and_delete(
        bsoncxx::document::view_or_value filter,
        const options::find_one_and_delete& options = options::find_one_and_delete());

    ///
    /// Finds a single document matching the filter, replaces it, and returns either the original
    /// or the replacement document.
    ///
    /// @param filter
    ///   Document view representing a document that should be replaced.
    /// @param replacement
    ///   Document view representing the replacement for a matching document.
    /// @param options
    ///   Optional arguments, see options::find_one_and_replace.
    ///
    /// @return The original or replaced document.
    ///
    /// @exception
    ///   Throws mongocxx::logic_error if the collation option is specified and an unacknowledged
    ///   write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::write_exception if the operation fails.
    ///
    stdx::optional<bsoncxx::document::value> find_one_and_replace(
        bsoncxx::document::view_or_value filter,
        bsoncxx::document::view_or_value replacement,
        const options::find_one_and_replace& options = options::find_one_and_replace());

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the newly-updated document.
    ///
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Document view representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see options::find_one_and_update.
    ///
    /// @return The original or updated document.
    ///
    /// @exception
    ///   Throws mongocxx::logic_error if the collation option is specified and an unacknowledged
    ///   write concern is used.
    ///
    /// @exception
    ///   Throws mongocxx::write_exception if the operation fails.
    ///
    stdx::optional<bsoncxx::document::value> find_one_and_update(
        bsoncxx::document::view_or_value filter,
        bsoncxx::document::view_or_value update,
        const options::find_one_and_update& options = options::find_one_and_update());

    ///
    /// Inserts a single document into the collection. If the document is missing an identifier
    /// (@c _id field) one will be generated for it.
    ///
    /// @param document
    ///   The document to insert.
    /// @param options
    ///   Optional arguments, see options::insert.
    ///
    /// @return The optional result of attempting to perform the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::bulk_write_exception if the operation fails.
    ///
    stdx::optional<result::insert_one> insert_one(
        bsoncxx::document::view_or_value document,
        const options::insert& options = options::insert());

    ///
    /// Inserts multiple documents into the collection. If any of the documents are missing
    /// identifiers the driver will generate them.
    ///
    /// @warning This method uses the bulk insert command to execute the insertion as opposed to
    /// the legacy OP_INSERT wire protocol message. As a result, using this method to insert many
    /// documents on MongoDB < 2.6 will be slow.
    //
    /// @tparam containter_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of model::write.
    ///
    /// @param container
    ///   Container of a documents to insert.
    /// @param options
    ///   Optional arguments, see options::insert.
    ///
    /// @return The optional result of attempting to performing the insert.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws mongocxx::bulk_write_exception when the operation fails.
    ///
    template <typename container_type>
    MONGOCXX_INLINE stdx::optional<result::insert_many> insert_many(
        const container_type& container, const options::insert& options = options::insert());

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
    ///   type of bsoncxx::document::view.
    ///
    /// @param begin
    ///   Iterator pointing to the first document to be inserted.
    /// @param end
    ///   Iterator pointing to the end of the documents to be inserted.
    /// @param options
    ///   Optional arguments, see options::insert.
    ///
    /// @return The result of attempting to performing the insert.
    ///
    /// @throws mongocxx::bulk_write_exception if the operation fails.
    ///
    /// TODO: document DocumentViewIterator concept or static assert
    template <typename document_view_iterator_type>
    MONGOCXX_INLINE stdx::optional<result::insert_many> insert_many(
        document_view_iterator_type begin,
        document_view_iterator_type end,
        const options::insert& options = options::insert());

    ///
    /// Returns a list of the indexes currently on this collection.
    ///
    /// @return Cursor yielding the index specifications.
    ///
    /// @throws mongocxx::operation_exception if the operation fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/listIndexes/
    ///
    cursor list_indexes() const;

    ///
    /// Returns the name of this collection.
    ///
    /// @return The name of the collection.  The return value of this method is invalidated by any
    /// subsequent call to collection::rename() on this collection object.
    ///
    stdx::string_view name() const;

    ///
    /// Rename this collection.
    ///
    /// @param new_name The new name to assign to the collection.
    /// @param drop_target_before_rename Whether to overwrite any
    ///   existing collections called new_name. The default is false.
    ///
    /// @exception
    ///   mongocxx::operation_exception if the operation fails.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/renameCollection/
    ///
    /// @note
    ///   Write concern supported only for MongoDB 3.4+.
    ///
    void rename(bsoncxx::string::view_or_value new_name,
                bool drop_target_before_rename = false,
                const bsoncxx::stdx::optional<write_concern>& write_concern = {});

    ///
    /// Sets the read_concern for this collection. Changes will not have any effect on existing
    /// cursors or other read operations which use the previously-set read concern.
    ///
    /// @param rc
    ///   The new @c read_concern
    ///
    /// @see https://docs.mongodb.com/master/reference/read-concern/
    ///
    void read_concern(class read_concern rc);

    ///
    /// Gets the read_concern for the collection.
    ///
    /// If a read_concern has not been explicitly set for this collection object, it inherits
    /// the read_concern from its parent database or client object.
    ///
    /// @return The current read_concern.
    ///
    class read_concern read_concern() const;

    ///
    /// Sets the read_preference for this collection. Changes will not have any effect on existing
    /// cursors or other read operations which use the read preference.
    ///
    /// @param rp
    ///   The read_preference to set.
    ///
    /// @see https://docs.mongodb.com/master/core/read-preference/
    ///
    void read_preference(class read_preference rp);

    ///
    /// Gets the read_preference for the collection.
    ///
    /// @return The current read_preference.
    ///
    /// @see https://docs.mongodb.com/master/core/read-preference/
    ///
    class read_preference read_preference() const;

    ///
    /// Replaces a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param replacement
    ///   The replacement document.
    /// @param options
    ///   Optional arguments, see options::update.
    ///
    /// @return The optional result of attempting to replace a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::logic_error if the replacement is invalid, or mongocxx::bulk_write_exception if
    ///   the operation fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/update/
    ///
    stdx::optional<result::replace_one> replace_one(
        bsoncxx::document::view_or_value filter,
        bsoncxx::document::view_or_value replacement,
        const options::update& options = options::update());

    ///
    /// Updates multiple documents matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to matching documents.
    /// @param options
    ///   Optional arguments, see options::update.
    ///
    /// @return The optional result of attempting to update multiple documents.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::logic_error if the update is invalid, or mongocxx::bulk_write_exception if the
    ///   operation fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/update/
    ///
    stdx::optional<result::update> update_many(bsoncxx::document::view_or_value filter,
                                               bsoncxx::document::view_or_value update,
                                               const options::update& options = options::update());

    ///
    /// Updates a single document matching the provided filter in this collection.
    ///
    /// @param filter
    ///   Document representing the match criteria.
    /// @param update
    ///   Document representing the update to be applied to a matching document.
    /// @param options
    ///   Optional arguments, see options::update.
    ///
    /// @return The optional result of attempting to update a document.
    /// If the write concern is unacknowledged, the optional will be
    /// disengaged.
    ///
    /// @throws
    ///   mongocxx::logic_error if the update is invalid, or mongocxx::bulk_write_exception if the
    ///   operation fails.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/update/
    ///
    stdx::optional<result::update> update_one(bsoncxx::document::view_or_value filter,
                                              bsoncxx::document::view_or_value update,
                                              const options::update& options = options::update());

    ///
    /// Sets the write_concern for this collection. Changes will not have any effect on existing
    /// write operations.
    ///
    /// @param wc
    ///   The new write_concern to use.
    ///
    void write_concern(class write_concern wc);

    ///
    /// Gets the write_concern for the collection.
    ///
    /// @return The current write_concern.
    ///
    class write_concern write_concern() const;

    ///
    /// Gets an index_view to the collection.
    class index_view indexes();

   private:
    friend class bulk_write;
    friend class database;

    MONGOCXX_PRIVATE collection(const database& database,
                                bsoncxx::string::view_or_value collection_name);

    MONGOCXX_PRIVATE collection(const database& database, void* collection);

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE impl& _get_impl();
    MONGOCXX_PRIVATE const impl& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE stdx::optional<result::bulk_write> collection::write(
    const model::write& write, const options::bulk_write& options) {
    auto writes = create_bulk_write(options);
    writes.append(write);

    return bulk_write(writes);
}

template <typename container_type>
MONGOCXX_INLINE stdx::optional<result::bulk_write> collection::bulk_write(
    const container_type& requests, const options::bulk_write& options) {
    return bulk_write(requests.begin(), requests.end(), options);
}

template <typename write_model_iterator_type>
MONGOCXX_INLINE stdx::optional<result::bulk_write> collection::bulk_write(
    write_model_iterator_type begin,
    write_model_iterator_type end,
    const options::bulk_write& options) {
    auto writes = create_bulk_write(options);

    std::for_each(begin, end, [&](const model::write& current) { writes.append(current); });

    return bulk_write(writes);
}

template <typename container_type>
MONGOCXX_INLINE stdx::optional<result::insert_many> collection::insert_many(
    const container_type& container, const options::insert& options) {
    return insert_many(container.begin(), container.end(), options);
}

template <typename document_view_iterator_type>
MONGOCXX_INLINE stdx::optional<result::insert_many> collection::insert_many(
    document_view_iterator_type begin,
    document_view_iterator_type end,
    const options::insert& options) {
    options::bulk_write bulk_write_options;
    bulk_write_options.ordered(options.ordered().value_or(true));
    if (options.write_concern()) {
        bulk_write_options.write_concern(*options.write_concern());
    }
    if (options.bypass_document_validation()) {
        bulk_write_options.bypass_document_validation(*options.bypass_document_validation());
    }

    auto writes = create_bulk_write(bulk_write_options);
    bsoncxx::builder::basic::array inserted_ids;

    std::for_each(begin, end, [&inserted_ids, &writes](const bsoncxx::document::view& doc) {
        bsoncxx::builder::basic::document id_doc;

        if (!doc["_id"]) {
            id_doc.append(kvp("_id", bsoncxx::oid{}));
            writes.append(
                model::insert_one{make_document(concatenate(id_doc.view()), concatenate(doc))});
        } else {
            id_doc.append(kvp("_id", doc["_id"].get_value()));
            writes.append(model::insert_one{doc});
        }

        inserted_ids.append(id_doc.view());
    });

    auto result = bulk_write(writes);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::insert_many>{
        result::insert_many{std::move(result.value()), inserted_ids.view()}};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
