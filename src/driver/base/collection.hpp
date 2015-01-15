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

#include "driver/config/prelude.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

#include "bson/builder.hpp"
#include "bson/document.hpp"
#include "driver/base/bulk_write.hpp"
#include "driver/base/cursor.hpp"
#include "driver/base/read_preference.hpp"
#include "driver/base/write_concern.hpp"
#include "driver/options/aggregate.hpp"
#include "driver/options/bulk_write.hpp"
#include "driver/options/count.hpp"
#include "driver/options/delete.hpp"
#include "driver/options/distinct.hpp"
#include "driver/options/find.hpp"
#include "driver/options/find_one_and_delete.hpp"
#include "driver/options/find_one_and_replace.hpp"
#include "driver/options/find_one_and_update.hpp"
#include "driver/options/insert.hpp"
#include "driver/options/update.hpp"
#include "driver/result/bulk_write.hpp"
#include "driver/result/delete.hpp"
#include "driver/result/insert_many.hpp"
#include "driver/result/insert_one.hpp"
#include "driver/result/replace_one.hpp"
#include "driver/result/update.hpp"

namespace mongo {
namespace driver {
namespace base {

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
///   // Connect and get a collection
///   mongo::driver::client mongo_client;
///   auto coll = mongo_client["database"]["collection"];
/// @endcode
///
/// @todo Make this class copyable when C driver supports this.
///
class LIBMONGOCXX_EXPORT collection {

   public:

    ///
    /// Move constructs a collection.
    ///
    collection(collection&&) noexcept;

    ///
    /// Move assigns a client.
    ///
    collection& operator=(collection&&) noexcept;

    ///
    /// Destroys a collection.
    ///
    ~collection();

    ///
    /// Runs an aggregation framework pipeline against this collection.
    ///
    /// @param pipeline
    ///   The pipeline of aggregation operations to perform.
    /// @param options
    ///   Optional arguments, see mongo::driver::options::aggregate.
    ///
    /// @return A mongo::driver::cursor with the results.
    /// @throws operation_exception if the aggregation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/aggregate/
    ///
    cursor aggregate(
        const pipeline& pipeline,
        const options::aggregate& options = options::aggregate()
    );

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
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    /// @throws bulk_write_exception when there are errors processing the writes.
    ///
    /// @see mongo::driver::bulk_write
    /// @see http://docs.mongodb.org/manual/core/bulk-write-operations/
    ///
    template<typename container_type>
    inline optional<result::bulk_write> bulk_write(
        const container_type& writes,
        const options::bulk_write& options = options::bulk_write()
    );

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
    /// @throws bulk_write_exception when there are errors processing the writes.
    ///
    /// @see mongo::driver::bulk_write
    /// @see http://docs.mongodb.org/manual/core/bulk-write-operations/
    ///
    template<typename write_model_iterator_type>
    inline optional<result::bulk_write> bulk_write(
        write_model_iterator_type begin,
        write_model_iterator_type end,
        const options::bulk_write& options = options::bulk_write()
    );

    ///
    /// Sends a batch of writes represented by the bulk_write to the server.
    ///
    /// @param bulk_write
    ///   A bulk write which contains multiple write operations.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    /// @throws bulk_write_exception when there are errors processing the writes.
    ///
    /// @see http://docs.mongodb.org/manual/core/bulk-write-operations/
    ///
    optional<result::bulk_write> bulk_write(
        const class bulk_write& bulk_write
    );

    ///
    /// Counts the number of documents matching the provided filter.
    ///
    /// @param filter
    ///   The filter that documents must match in order to be counted.
    /// @param options
    ///   Optional arguments, see mongo::driver::options::count.
    ///
    /// @return The count of the documents that matched the filter.
    /// @throws operation_exception if the aggregation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/count/
    ///
    std::int64_t count(
        bson::document::view filter,
        const options::count& options = options::count()
    );

    ///
    /// Creates an index over the collection for the provided keys with the options provided.
    ///
    /// @param keys
    ///   The keys for the index: @c {a: 1, b: -1}
    /// @param options
    ///   Optional arguments to index creation command, see ensureindex-options link
    ///
    /// @throws operation_exception if the aggregation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.createIndex/
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.ensureIndex/#ensureindex-options
    ///
    bson::document::value create_index(
        bson::document::view keys,
        bson::document::view options
    );

    ///
    /// Deletes all matching documents from the collection.
    ///
    /// @param filter
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongo::driver::options::delete_options.
    ///
    /// @return The optional result of performing the deletion, a result::delete_result.
    /// @throws write_exception if the delete fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/delete/
    ///
    optional<result::delete_result> delete_many(
        bson::document::view filter,
        const options::delete_options& options = options::delete_options()
    );

    ///
    /// Deletes a single matching document from the collection.
    ///
    /// @param filter 
    ///   Document view representing the data to be deleted.
    /// @param options
    ///   Optional arguments, see mongo::driver::options::delete_options.
    ///
    /// @return The optional result of performing the deletion, a result::delete_result.
    /// @throws write_exception if the delete fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/delete/
    ///
    optional<result::delete_result> delete_one(
        bson::document::view filter,
        const options::delete_options& options = options::delete_options()
    );

    ///
    /// Finds the distinct values for a specified field accross the collection.
    ///
    /// @param field_name
    ///   The field for which the distinct values will be found.
    /// @param filter
    ///   Document view representing the documents for which the distinct operation will apply.
    /// @param options
    ///   Optional arguments, see options::distinct.
    ///
    /// @return Cursor having the distinct values for the specified field, a driver::cursor.
    /// @throws operation_exception when the distinct fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/distinct/
    ///
    cursor distinct(
        const std::string& field_name,
        bson::document::view filter,
        const options::distinct& options = options::distinct()
    );

    ///
    /// Drops this collection and the contained documents from the database.
    ///
    /// @throws operation_exception when the operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.drop/
    ///
    void drop();

    ///
    /// Finds the documents in this collection which match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see options::find
    ///
    /// @return Cursor with the matching documents from the collection, a driver::cursor.
    /// @throws operation_exception when the operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-operations-introduction/
    ///
    cursor find(
        bson::document::view filter,
        const options::find& options = options::find()
    );


    ///
    /// Finds a single document in this collection matching the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    /// @param options
    ///   Optional arguments, see options::find
    ///
    /// @return An optional document that matched the filter.
    /// @throws operation_exception when the operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-operations-introduction/
    ///
    optional<bson::document::value> find_one(
        bson::document::view filter,
        const options::find& options = options::find()
    );

    ///
    /// Finds a single document matching the filter, deletes it, and returns the original.
    ///
    /// @param filter
    ///   Document view representing a document that should be deleted.
    /// @param options
    ///   Optional arguments, see options::find_one_and_delete
    ///
    /// @return The document that was deleted.
    /// @throws write_exception when the operation fails
    ///
    optional<bson::document::value> find_one_and_delete(
        bson::document::view filter,
        const options::find_one_and_delete& options = options::find_one_and_delete()
    );

    ///
    /// Finds a single document matching the filter, replaces it and returns either the original
    /// or the replaced document.
    ///
    /// @param filter
    ///   Document view representing a document that should be replaced.
    /// @param replacement
    ///   Document view representing the replacement for a matching document.
    /// @param options
    ///   Optional arguments, see options::find_one_and_replace.
    ///
    /// @return The original or replaced document.
    /// @throws write_exception when the operation fails.
    ///
    optional<bson::document::value> find_one_and_replace(
        bson::document::view filter,
        bson::document::view replacement,
        const options::find_one_and_replace& options = options::find_one_and_replace()
    );

    ///
    /// Finds a single document matching the filter, updates it, and returns either the original
    /// or the updated document.
    ///
    /// @param filter
    ///   Document view representing a document that should be updated.
    /// @param update
    ///   Document view representing the update to apply to a matching document.
    /// @param options
    ///   Optional arguments, see options::find_one_and_update.
    ///
    /// @return The original or updated document.
    /// @throws write_exception when the operation fails.
    ///
    optional<bson::document::value> find_one_and_update(
        bson::document::view filter,
        bson::document::view update,
        const options::find_one_and_update& options = options::find_one_and_update()
    );

    ///
    /// Inserts a single document into the collection. If the document is missing an identifier
    /// one will be generated for it.
    ///
    /// @param document
    ///   The document to insert.
    /// @param options
    ///   Optional arguments, see options::insert.
    ///
    /// @return The result of attempting to perform the insert.
    /// @throws write_exception when the operation fails.
    ///
    optional<result::insert_one> insert_one(
        bson::document::view document,
        const options::insert& options = options::insert()
    );

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
    /// @return The result of attempting to performing the insert.
    /// @throws write_exception when the operation fails.
    ///
    template<typename container_type>
    inline optional<result::insert_many> insert_many(
        const container_type& container,
        const options::insert& options = options::insert()
    );

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
    ///   type of bson::document::view.
    ///
    /// @param begin
    ///   Iterator pointing to the first document to be inserted.
    /// @param end
    ///   Iterator pointing to the end of the documents to be inserted.
    /// @param options
    ///   Optional arguments, see options::insert.
    ///
    /// @return The result of attempting to performing the insert.
    /// @throws write_exception when the operation fails.
    ///
    /// TODO: document DocumentViewIterator concept or static assert
    template<typename document_view_iterator_type>
    inline optional<result::insert_many> insert_many(
        document_view_iterator_type begin,
        document_view_iterator_type end,
        const options::insert& options = options::insert()
    );

    ///
    /// Produces a list of indexes currently on this collection.
    ///
    /// @return Cursor yielding the index specifications.
    /// @throws write_exception when the operation fails.
    ///
    cursor list_indexes() const;

    ///
    /// Returns the name of this collection.
    ///
    /// @return The name of the collection.
    ///
    const std::string& name() const noexcept;

    ///
    /// Sets the read_preference for the collection. Changes will not have any effect on existing
    /// cursors or other read operations which use the read preference.
    ///
    /// @param rp
    ///   The read_preference to set.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
    ///
    void read_preference(class read_preference rp);

    ///
    /// Gets the read_preference for the collection.
    ///
    /// @return The current read_preference.
    ///
    /// @see http://docs.mongodb.org/manual/core/read-preference/
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
    /// @return The result of attempting to replace a document.
    /// @throws write_exception if the operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/update/
    ///
    optional<result::replace_one> replace_one(
        bson::document::view filter,
        bson::document::view replacement,
        const options::update& options = options::update()
    );

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
    /// @return The result of attempting to update multiple documents.
    /// @throws write_exception if the update operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/update/
    ///
    optional<result::update> update_many(
        bson::document::view filter,
        bson::document::view update,
        const options::update& options = options::update()
    );

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
    /// @return The result of attempting to update a document.
    /// @throws write_exception when the update operation fails.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/update/
    ///
    optional<result::update> update_one(
        bson::document::view filter,
        bson::document::view update,
        const options::update& options = options::update()
    );

    ///
    /// Sets the write_concern for the collection. Changes will not have any effect on existing
    /// write operations.
    ///
    /// @param wc
    ///   The write_concern to set.
    ///
    void write_concern(class write_concern wc);

    ///
    /// Gets the write_concern for the collection.
    ///
    /// @return The current write_concern.
    ///
    class write_concern write_concern() const;

   private:
    friend class database;

    collection(const database& database, const std::string& collection_name);

    class impl;
    std::unique_ptr<impl> _impl;

};

template<typename container_type>
inline optional<result::bulk_write> collection::bulk_write(
    const container_type& requests,
    const options::bulk_write& options
) {
    return bulk_write(requests.begin(), requests.end(), options);
}

template<typename write_model_iterator_type>
inline optional<result::bulk_write> collection::bulk_write(
    write_model_iterator_type begin,
    write_model_iterator_type end,
    const options::bulk_write& options
) {
    class bulk_write writes(options.ordered().value_or(true));

    std::for_each(begin, end, [&](const model::write& current){
        writes.append(current);
    });

    return bulk_write(writes);
}

template<typename container_type>
inline optional<result::insert_many> collection::insert_many(
    const container_type& container,
    const options::insert& options
) {
    return insert_many(container.begin(), container.end(), options);
}

template<typename document_view_iterator_type>
inline optional<result::insert_many> collection::insert_many(
    document_view_iterator_type begin,
    document_view_iterator_type end,
    const options::insert& options
) {
    class bulk_write writes(false);

    std::map<std::size_t, bson::document::element> inserted_ids{};
    size_t index = 0;
    std::for_each(begin, end, [&](const bson::document::view& current){
        // TODO: put this somewhere else not in header scope (bson::builder)
        if ( !current.has_key("_id")) {
            bson::builder::document new_document;
            new_document << "_id" << bson::oid(bson::oid::init_tag);
            new_document << bson::builder::helpers::concat{current};

            writes.append(model::insert_one(new_document.view()));

            inserted_ids.emplace(index++, new_document.view()["_id"]);
        } else {
            writes.append(model::insert_one(current));

            inserted_ids.emplace(index++, current["_id"]);
        }

    });

    if (options.write_concern())
        writes.write_concern(*options.write_concern());
    result::bulk_write res(std::move(bulk_write(writes).value()));
    optional<result::insert_many> result(result::insert_many(std::move(res), std::move(inserted_ids)));
    return result;
}

}  // namespace base
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
