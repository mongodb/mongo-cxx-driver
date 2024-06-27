// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/result/replace_one-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

/// Class representing the result of a MongoDB replace_one operation.
class replace_one {
   public:
    // This constructor is public for testing purposes only
    explicit replace_one(result::bulk_write result);

    ///
    /// Returns the bulk write result for this replace_one operation.
    ///
    /// @return The raw bulk write result.
    ///
    const result::bulk_write& result() const;

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
    /// Gets the id of the upserted document.
    ///
    /// @return The value of the _id field for upserted document.
    ///
    stdx::optional<bsoncxx::v_noabi::document::element> upserted_id() const;

   private:
    result::bulk_write _result;

    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const replace_one&, const replace_one&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const replace_one&, const replace_one&);
};

}  // namespace result
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
