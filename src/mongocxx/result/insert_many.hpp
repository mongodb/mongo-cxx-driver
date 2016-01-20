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
#include <map>

#include <bsoncxx/types.hpp>
#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection;
class insert_many_builder;

namespace result {

///
/// Class representing the result of a MongoDB insert many operation
/// (executed as a bulk write).
///
class MONGOCXX_API insert_many {
   public:
    using id_map = std::map<std::size_t, bsoncxx::document::element>;

    ///
    /// Returns the bulk write result for this insert many operation.
    ///
    /// @return The raw bulk write result.
    ///
    const result::bulk_write& result() const;

    ///
    /// Gets the number of documents that were inserted during this operation.
    ///
    /// @return The number of documents that were inserted.
    ///
    std::int32_t inserted_count() const;

    ///
    /// Gets the _ids of the inserted documents.
    ///
    /// @return The values of the _id field for inserted documents.
    ///
    id_map inserted_ids();

   private:
    friend collection;
    friend insert_many_builder;

    MONGOCXX_PRIVATE insert_many(result::bulk_write result, id_map inserted_ids);

    result::bulk_write _result;
    id_map _generated_ids;
};

}  // namespace result
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
