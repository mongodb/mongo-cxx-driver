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

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace result {

/// Class representing the result of a MongoDB insert operation.
class MONGOCXX_API insert_one {
   public:
    // This constructor is public for testing purposes only
    insert_one(result::bulk_write result, bsoncxx::types::value generated_id);

    ///
    /// Returns the bulk write result for this insert operation.
    ///
    /// @return The raw bulk write result.
    ///
    const result::bulk_write& result() const;

    ///
    /// Gets the _id of the inserted document.
    ///
    /// @return The value of the _id field for inserted document.
    ///
    const bsoncxx::types::value& inserted_id() const;

   private:
    result::bulk_write _result;
    bsoncxx::types::value _generated_id;
};

}  // namespace result
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
