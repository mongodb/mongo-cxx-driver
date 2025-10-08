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

#include <mongocxx/result/replace_one.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

replace_one::replace_one(result::bulk_write result) : _result(std::move(result)) {}

result::bulk_write const& replace_one::result() const {
    return _result;
}

std::int32_t replace_one::matched_count() const {
    return _result.matched_count();
}

std::int32_t replace_one::modified_count() const {
    return _result.modified_count();
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::element> replace_one::upserted_id() const {
    if (_result.upserted_ids().size() == 0) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }
    return _result.upserted_ids()[0];
}

bool operator==(replace_one const& lhs, replace_one const& rhs) {
    return lhs.result() == rhs.result();
}
bool operator!=(replace_one const& lhs, replace_one const& rhs) {
    return !(lhs == rhs);
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
