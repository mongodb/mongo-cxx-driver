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

#include <mongocxx/result/delete.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

delete_result::delete_result(result::bulk_write result) : _result(std::move(result)) {}

result::bulk_write const& delete_result::result() const {
    return _result;
}

std::int32_t delete_result::deleted_count() const {
    return _result.deleted_count();
}

bool operator==(delete_result const& lhs, delete_result const& rhs) {
    return lhs.result() == rhs.result();
}
bool operator!=(delete_result const& lhs, delete_result const& rhs) {
    return !(lhs == rhs);
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
