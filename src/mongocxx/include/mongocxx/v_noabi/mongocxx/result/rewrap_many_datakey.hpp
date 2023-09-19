// Copyright 2023 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace result {

/// Class representing the result of a MongoDB rewrap_many_datakey operation.
class MONGOCXX_API rewrap_many_datakey {
   public:
    rewrap_many_datakey() = default;

    explicit rewrap_many_datakey(mongocxx::result::bulk_write bulk_write_result_doc);

    ///
    /// Returns the bulk write result for this rewrap_many_datakey operation.
    ///
    /// @return The raw bulk write result.
    ///
    const bsoncxx::stdx::optional<mongocxx::result::bulk_write>& result();

   private:
    bsoncxx::stdx::optional<mongocxx::result::bulk_write> _result;
};

}  // namespace result
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
