// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/options/range.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

range& range::min(bsoncxx::v_noabi::types::bson_value::view_or_value value) {
    _min = std::move(value);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& range::min() const {
    return _min;
}

range& range::max(bsoncxx::v_noabi::types::bson_value::view_or_value value) {
    _max = std::move(value);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& range::max() const {
    return _max;
}

range& range::sparsity(std::int64_t value) {
    _sparsity = value;
    return *this;
}

range& range::trim_factor(std::int32_t value) {
    _trim_factor = value;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::int64_t> const& range::sparsity() const {
    return _sparsity;
}

bsoncxx::v_noabi::stdx::optional<std::int32_t> const& range::trim_factor() const {
    return _trim_factor;
}

range& range::precision(std::int32_t value) {
    _precision = value;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::int32_t> const& range::precision() const {
    return _precision;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
