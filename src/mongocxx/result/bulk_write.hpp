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

#include <mongocxx/config/prelude.hpp>

#include <cstdint>
#include <vector>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace result {

///
/// Class representing the result of a MongoDB bulk write operation.
///
class MONGOCXX_API bulk_write {

   public:
    explicit bulk_write(bsoncxx::document::value raw_response);

    ///
    /// Gets the number of documents that were inserted during this operation.
    ///
    /// @return The number of documents that were inserted.
    ///
    std::int32_t inserted_count() const;

    ///
    /// Gets the number of documents that were matched during this operation.
    ///
    /// @return The number of documents that were matched.
    ///
    std::int32_t matched_count() const;

    ///
    /// Gets the number of documents that were modified during this operation.
    ///
    /// @return The number of documents that were modified.
    ///
    std::int32_t modified_count() const;

    ///
    /// Gets the number of documents that were deleted during this operation.
    ///
    /// @return The number of documents that were deleted.
    ///
    std::int32_t deleted_count() const;

    ///
    /// Gets the number of documents that were upserted during this operation.
    ///
    /// @return The number of documents that were upserted.
    ///
    std::int32_t upserted_count() const;

    ///
    /// Gets the ids of the inserted documents.
    ///
    /// @return The values of the _id field for inserted documents.
    ///
    bsoncxx::document::element inserted_ids() const;

    ///
    /// Gets the ids of the upserted documents.
    ///
    /// @return The values of the _id field for upserted documents.
    ///
    bsoncxx::document::element upserted_ids() const;

   private:
    MONGOCXX_PRIVATE bsoncxx::document::view view() const;

    bsoncxx::document::value _response;

}; // class bulk_write

}  // namespace result
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
