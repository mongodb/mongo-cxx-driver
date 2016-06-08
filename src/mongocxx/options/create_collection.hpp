// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/validation_criteria.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB createCollection command
///
class MONGOCXX_API create_collection {
   public:
    ///
    /// To create a capped collection, specify true.
    ///
    /// @note If you specify true, you must also set a maximum size using the size() method.
    ///
    /// @param capped
    ///   Whether or not this collection will be capped.
    ///
    /// @see https://docs.mongodb.org/manual/reference/glossary/#term-capped-collection
    ///
    create_collection& capped(bool capped);

    ///
    /// Specify false to disable the automatic creation of an index on the _id field.
    ///
    /// @note For replica sets, all collections must have autoIndexId set to true.
    ///
    /// @param auto_index_id
    ///   Whether or not this collection will automatically generate an index on _id.
    ///
    create_collection& auto_index_id(bool auto_index_id);

    ///
    /// A maximum size, in bytes, for a capped collection.
    ///
    /// @note Once a capped collection reaches its maximum size, MongoDB removes older
    ///   documents to make space for new documents.
    ///
    /// @note Size is required for capped collections and ignored for other collections.
    ///
    /// @param max_size
    ///   Maximum size, in bytes, of this collection (if capped)
    ///
    create_collection& size(int max_size);

    ///
    /// The maximum number of documents allowed in the capped collection.
    ///
    /// @note The size limit takes precedence over this limit. If a capped collection reaches
    ///   the size limit before it reaches the maximum number of documents, MongoDB removes
    ///   old documents.
    ///
    /// @param max_documents
    ///   Maximum number of documents allowed in the collection (if capped)
    ///
    create_collection& max(int max_documents);

    ///
    /// Specify configuration to the storage on a per-collection basis.
    ///
    /// @note This option is currently only available with the WiredTiger storage engine.
    ///
    /// @param storage_engine_options
    ///   Configuration options specific to the storage engine.
    ///
    create_collection& storage_engine(bsoncxx::document::view_or_value storage_engine_opts);

    ///
    /// When true, disables the power of 2 sizes allocation for the collection.
    ///
    /// @see: https://docs.mongodb.org/manual/reference/method/db.createCollection/
    ///
    /// @param no_padding
    ///   When true, disables power of 2 sizing for this collection.
    ///
    create_collection& no_padding(bool no_padding);

    ///
    /// Specify validation criteria for this collection.
    ///
    /// @param validation
    ///   Validation criteria for this collection.
    ///
    /// @see https://docs.mongodb.org/manual/core/document-validation/
    ///
    create_collection& validation_criteria(class validation_criteria validation);

    ///
    /// Return a bson document representing the options set on this object.
    ///
    /// @return Options, as a document.
    ///
    bsoncxx::document::value to_document() const;

    MONGOCXX_INLINE operator bsoncxx::document::value() const;

   private:
    stdx::optional<bool> _capped;
    stdx::optional<bool> _auto_index_id;
    stdx::optional<int> _max_size;
    stdx::optional<int> _max_documents;
    stdx::optional<bsoncxx::document::view_or_value> _storage_engine_opts;
    stdx::optional<bool> _no_padding;
    stdx::optional<class validation_criteria> _validation;
};

MONGOCXX_INLINE create_collection::operator bsoncxx::document::value() const {
    return to_document();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
