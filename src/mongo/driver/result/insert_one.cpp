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

#include <mongo/driver/result/insert_one.hpp>

namespace mongo {
namespace driver {
namespace result {

insert_one::insert_one(result::bulk_write result, bson::document::element generated_id)
    : _result(std::move(result)), _generated_id(std::move(generated_id)) {
}

const result::bulk_write& insert_one::result() const {
    return _result;
}

bson::document::element insert_one::inserted_id() const {
    return _generated_id;
}

}  // namespace result
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
