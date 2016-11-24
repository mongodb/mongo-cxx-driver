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

#include <cstdint>
#include <string>
#include <memory>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection;

///
/// Class representing a MongoDB aggregation pipeline.
///
class MONGOCXX_API pipeline {
   public:
    ///
    /// Creates a new aggregation pipeline.
    ///
    /// @see http://docs.mongodb.org/manual/core/aggregation-pipeline/
    ///
    pipeline();

    ///
    /// Move constructs a pipeline.
    ///
    pipeline(pipeline&&) noexcept;

    ///
    /// Move assigns a pipeline.
    ///
    pipeline& operator=(pipeline&&) noexcept;

    ///
    /// Destroys a pipeline.
    ///
    ~pipeline();

    ///
    /// Groups documents by some specified expression and outputs to the next stage a
    /// document for each distinct grouping. The output documents contain an `_id` field
    /// which contains the the distinct key for that group. The output documents can also
    /// contain computed fields that hold the values of some accumulator expression grouped
    /// by the group's `_id` field.
    ///
    /// @note group does not order output documents.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/group/
    ///
    /// @param group_args
    ///   The specification for the group operation.  The required field `_id` must be included.
    ///
    pipeline& group(bsoncxx::document::view_or_value group_args);

    ///
    /// Limits the number of documents passed to the next stage in the pipeline.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/limit/
    ///
    /// @param limit
    ///   The number of documents to which output should be limited.
    ///
    pipeline& limit(std::int32_t limit);

    ///
    /// Performs a left outer join to an unsharded collection in the same database to filter in
    /// documents from the "joined" collection for processing.
    ///
    /// @see https://docs.mongodb.org/manual/reference/operator/aggregation/lookup/
    ///
    /// @param lookup_args
    ///   The specification for the lookup operation.  The required fields `from`, `localField`,
    ///   `foreignField`, and `as` must be included.
    ///
    pipeline& lookup(bsoncxx::document::view_or_value lookup_args);

    ///
    /// Filters the documents. Only the documents that match the condition(s) specified by the
    /// `filter` will continue to the next pipeline stage.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/match/
    ///
    /// @param filter
    ///   The filter.
    ///
    pipeline& match(bsoncxx::document::view_or_value filter);

    ///
    /// Takes documents returned by the aggregation pipeline and writes them to a specified
    /// collection. This stage must be the last stage in the pipeline. The out operator lets the
    /// aggregation framework return result sets of any size.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/out/
    ///
    /// @param collection_name
    ///   The name of the collection where the output documents should go.
    ///
    pipeline& out(std::string collection_name);

    ///
    /// Projects a subset of the fields in the documents to the next stage of the pipeline.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/project/
    ///
    /// @param projection
    ///   The projection specification.
    ///
    pipeline& project(bsoncxx::document::view_or_value projection);

    ///
    /// Restricts the contents of the documents based on information stored in the documents
    /// themselves.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/redact/
    ///
    /// @param restrictions
    ///   The document restrictions.
    ///
    pipeline& redact(bsoncxx::document::view_or_value restrictions);

    ///
    /// Randomly selects the specified number of documents that pass into the stage and passes the
    /// remaining documents to the next stage in the pipeline.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/sample/
    ///
    /// @param size
    ///   The number of input documents to select.
    ///
    pipeline& sample(std::int32_t size);

    ///
    /// Skips over the specified number of documents that pass into the stage and passes the
    /// remaining documents to the next stage in the pipeline.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/skip/
    ///
    /// @param docs_to_skip
    ///   The number of input documents to skip.
    ///
    pipeline& skip(std::int32_t docs_to_skip);

    ///
    /// Sorts all input documents and returns them to the pipeline in sorted order.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/sort/
    ///
    /// @param ordering
    ///   Document specifying the ordering by which the documents are sorted.
    ///
    pipeline& sort(bsoncxx::document::view_or_value ordering);

    ///
    /// Deconstructs an array field from the input documents to output a document for each element.
    /// Each output document is an input document with the value of its array field replaced by
    /// an element from the unwound array.
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/aggregation/unwind/
    ///
    /// @param field_name
    ///   The name of the field to unwind.
    ///
    pipeline& unwind(std::string field_name);

    ///
    /// @return A view of the underlying BSON array this pipeline represents.
    ///
    bsoncxx::array::view view_array() const;

    ///
    /// @return A view of the underlying BSON document this pipeline represents.
    ///
    /// @deprecated The view_array() method should be used instead of this method.
    ///
    MONGOCXX_DEPRECATED bsoncxx::document::view view() const;

   private:
    friend class collection;

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
