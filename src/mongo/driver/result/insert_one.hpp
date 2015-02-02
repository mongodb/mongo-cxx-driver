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

#include <mongo/bson/types.hpp>
#include <mongo/driver/result/bulk_write.hpp>

namespace mongo {
namespace driver {
namespace result {

/// Class representing the result of a MongoDB insert operation.
class LIBMONGOCXX_API insert_one {

   public:
    // This constructor is public for testing purposes only
    insert_one(result::bulk_write result, bson::document::element generated_id);

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
    bson::document::element inserted_id() const;

   private:
    result::bulk_write _result;
    bson::document::element _generated_id;

}; // class insert_one

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
