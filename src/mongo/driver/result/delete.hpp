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

#include <cstdint>

#include <mongo/bson/document.hpp>
#include <mongo/driver/result/bulk_write.hpp>

namespace mongo {
namespace driver {
namespace result {

///
/// Class representing the result of a MongoDB delete operation.
///
class LIBMONGOCXX_API delete_result {

   public:
    // This constructor is public for testing purposes only
    explicit delete_result(result::bulk_write result);

    ///
    /// Returns the bulk write result.
    ///
    /// @return The raw bulk write result.
    ///
    const result::bulk_write& result() const;

    ///
    /// Gets the number of documents that were deleted during this operation.
    ///
    /// @return The number of documents that were deleted.
    ///
    std::int64_t deleted_count() const;

   private:
    result::bulk_write _result;

}; // class delete_result

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
