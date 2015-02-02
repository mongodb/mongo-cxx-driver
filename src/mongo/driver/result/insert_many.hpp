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

#include <mongo/driver/config/prelude.hpp>
#include <mongo/driver/result/bulk_write.hpp>

#include <cstdint>
#include <map>

#include <mongo/bson/types.hpp>

namespace mongo {
namespace driver {
namespace result {

///
/// Class representing the result of a MongoDB insert many operation
/// (executed as a bulk write).
///
class LIBMONGOCXX_API insert_many {

   public:
    // TODO: public alias the map
    // This constructor is public for testing purposes only
    insert_many(
        result::bulk_write result,
        std::map<std::size_t, bson::document::element> inserted_ids
    );

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
    std::int64_t inserted_count() const;

    ///
    /// Gets the _ids of the inserted documents.
    ///
    /// @return The values of the _id field for inserted documents.
    ///
    std::map<std::size_t, bson::document::element> inserted_ids();

   private:
    result::bulk_write _result;
    std::map<std::size_t, bson::document::element> _generated_ids;

}; // class insert_many

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
