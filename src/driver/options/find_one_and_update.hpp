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
#include "driver/options/find_one_and_modify.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace options {

class LIBMONGOCXX_EXPORT find_one_and_update {

   public:
    void max_time_ms(std::int64_t max_time_ms);
    const optional<std::int64_t>& max_time_ms() const;

    void projection(bson::document::view projection);
    const optional<bson::document::view>& projection() const;

    void return_document(return_document return_document);
    const optional<enum return_document>& return_document() const;

    void sort(bson::document::view ordering);
    const optional<bson::document::view>& sort() const;

    void upsert(bool upsert);
    const optional<bool>& upsert() const;

   private:
    optional<std::int64_t> _max_time_ms;
    optional<bson::document::view> _projection;
    optional<enum return_document> _return_document;
    optional<bson::document::view> _ordering;
    optional<bool> _upsert;

}; // class find_one_and_update

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
