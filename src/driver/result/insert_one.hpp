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

#include "bson/types.hpp"
#include "driver/result/bulk_write.hpp"

namespace mongo {
namespace driver {
namespace result {

class LIBMONGOCXX_EXPORT insert_one {

   public:
    insert_one(result::bulk_write result, bson::document::element generated_id);

    const result::bulk_write& result() const;

    bson::document::element inserted_id() const;

   private:
    result::bulk_write _result;
    bson::document::element _generated_id;

}; // class insert_one

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
