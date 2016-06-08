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

#include <mongocxx/options/aggregate.hpp>

#include <mongocxx/private/read_preference.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

aggregate& aggregate::allow_disk_use(bool allow_disk_use) {
    _allow_disk_use = allow_disk_use;
    return *this;
}

aggregate& aggregate::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
    return *this;
}

aggregate& aggregate::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
    return *this;
}

aggregate& aggregate::use_cursor(bool use_cursor) {
    _use_cursor = use_cursor;
    return *this;
}

aggregate& aggregate::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
    return *this;
}

aggregate& aggregate::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

const stdx::optional<bool>& aggregate::allow_disk_use() const {
    return _allow_disk_use;
}
const stdx::optional<std::int32_t>& aggregate::batch_size() const {
    return _batch_size;
}

const stdx::optional<std::chrono::milliseconds>& aggregate::max_time() const {
    return _max_time;
}

const stdx::optional<bool>& aggregate::use_cursor() const {
    return _use_cursor;
}
const stdx::optional<class read_preference>& aggregate::read_preference() const {
    return _read_preference;
}
const stdx::optional<bool>& aggregate::bypass_document_validation() const {
    return _bypass_document_validation;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
