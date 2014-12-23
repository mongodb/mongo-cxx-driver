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

#include "bson/document.hpp"
#include "bson/types.hpp"

namespace mongo {
namespace driver {
namespace result {

class LIBMONGOCXX_EXPORT bulk_write {

   public:
    explicit bulk_write(bson::document::value raw_response);

    bson::document::view view() const;

    std::int64_t inserted_count() const;
    std::int64_t matched_count() const;
    std::int64_t modified_count() const;
    std::int64_t deleted_count() const;
    std::int64_t upserted_count() const;

    bson::document::element inserted_ids() const;
    bson::document::element upserted_ids() const;

   private:
    bson::document::value _response;

}; // class bulk_write

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
