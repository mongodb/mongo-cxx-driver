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

#include "driver/config/prelude.hpp"

#include <cstdint>

#include "bson/types.hpp"
#include "driver/result/bulk_write.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace result {

class LIBMONGOCXX_EXPORT update {

   public:
    explicit update(result::bulk_write result);

    const result::bulk_write& result() const;

    std::int64_t matched_count() const;
    std::int64_t modified_count() const;

    optional <bson::document::element> upserted_id() const;

   private:
    result::bulk_write _result;

}; // class update

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
