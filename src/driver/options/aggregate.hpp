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
#include "driver/base/read_preference.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace options {

class LIBMONGOCXX_EXPORT aggregate {

   public:
    void allow_disk_use(bool allow_disk_use);
    const optional<bool>& allow_disk_use() const;

    void batch_size(std::int32_t batch_size);
    const optional<std::int32_t>& batch_size() const;

    void max_time_ms(std::int64_t max_time_ms);
    const optional<std::int64_t>& max_time_ms() const;

    void use_cursor(bool use_cursor);
    const optional<bool>& use_cursor() const;

    void read_preference(class read_preference read_preference);
    const optional<class read_preference>& read_preference() const;

   private:
    optional<bool> _allow_disk_use;
    optional<std::int32_t> _batch_size;
    optional<std::int64_t> _max_time_ms;
    // NOTE: It's important to be have this bool be optional as the default changed.
    optional<bool> _use_cursor;
    optional<class read_preference> _read_preference;

}; // class aggregate

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
