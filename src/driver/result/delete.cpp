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

#include "driver/result/delete.hpp"

namespace mongo {
namespace driver {
namespace result {

delete_result::delete_result(result::bulk_write result)
    : _result(std::move(result))
{}

const result::bulk_write& delete_result::result() const {
    return _result;
}

std::int64_t delete_result::deleted_count() const {
    return _result.deleted_count();
}

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
