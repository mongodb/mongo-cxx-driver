/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "bson/document.hpp"
#include "driver/model/write.hpp"
#include "driver/util/optional.hpp"

namespace mongo {
namespace driver {
namespace model {

class find_one_and_remove : public write<find_one_and_remove> {

   public:
    find_one_and_remove(bson::document::view filter);

    find_one_and_remove& projection(bson::document::view projection);
    find_one_and_remove& sort(bson::document::view ordering);

    bson::document::view filter() const;

    optional<bson::document::view> projection() const;
    optional<bson::document::view> sort() const;

   private:
    bson::document::view _filter;

    optional<bson::document::view> _projection;
    optional<bson::document::view> _ordering;
};

}  // namespace model
}  // namespace driver
}  // namespace mongo
