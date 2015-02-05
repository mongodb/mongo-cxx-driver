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

#include <mongo/driver/options/aggregate.hpp>
#include <mongo/driver/private/read_preference.hpp>

namespace mongo {
namespace driver {
namespace options {

void aggregate::allow_disk_use(bool allow_disk_use) {
    _allow_disk_use = allow_disk_use;
}
void aggregate::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
}
void aggregate::max_time_ms(std::int64_t max_time_ms) {
    _max_time_ms = max_time_ms;
}
void aggregate::use_cursor(bool use_cursor) {
    _use_cursor = use_cursor;
}
void aggregate::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
}

const stdx::optional<bool>& aggregate::allow_disk_use() const {
    return _allow_disk_use;
}
const stdx::optional<std::int32_t>& aggregate::batch_size() const {
    return _batch_size;
}
const stdx::optional<std::int64_t>& aggregate::max_time_ms() const {
    return _max_time_ms;
}
const stdx::optional<bool>& aggregate::use_cursor() const {
    return _use_cursor;
}
const stdx::optional<class read_preference>& aggregate::read_preference() const {
    return _read_preference;
}

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
