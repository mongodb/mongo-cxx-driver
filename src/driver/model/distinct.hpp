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

#include <cstdint>
#include <string>

#include "bson/document.hpp"
#include "driver/model/read.hpp"
#include "driver/util/optional.hpp"

namespace mongo {
namespace driver {
namespace model {

class distinct : public read<distinct> {

   public:
    distinct(std::string field_name);

    distinct& filter(bson::document::view filter);
    distinct& max_time_ms(int64_t max_time_ms);

    bson::document::view field_name() const;

    optional<bson::document::view> filter() const;
    optional<int64_t> max_time_ms() const;

   private:
    bson::document::view _field_name;

    optional<bson::document::view> _filter;
    optional<int64_t> _max_time_ms;
};

}  // namespace model
}  // namespace driver
}  // namespace mongo
