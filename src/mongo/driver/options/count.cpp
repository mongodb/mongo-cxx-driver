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

#include <mongo/driver/options/count.hpp>
#include <mongo/driver/private/read_preference.hpp>

namespace mongo {
namespace driver {
namespace options {

void count::hint(bson::document::view hint) {
    _hint = std::move(hint);
}

void count::limit(std::int64_t limit) {
    _limit = limit;
}

void count::max_time_ms(std::int64_t max_time_ms) {
    _max_time_ms = max_time_ms;
}

void count::skip(std::int64_t skip) {
    _skip = skip;
}

void count::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
}

const stdx::optional<bson::document::view>& count::hint() const {
    return _hint;
}

const stdx::optional<std::int64_t>& count::limit() const {
    return _limit;
}

const stdx::optional<std::int64_t>& count::max_time_ms() const {
    return _max_time_ms;
}

const stdx::optional<std::int64_t>& count::skip() const {
    return _skip;
}

const stdx::optional<read_preference>& count::read_preference() const {
    return _read_preference;
}

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
