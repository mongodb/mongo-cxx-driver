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

#include <mongocxx/result/update.hpp>

//

#include <mongocxx/v1/update_many_result.hh>
#include <mongocxx/v1/update_one_result.hh>

#include <utility>

#include <mongocxx/result/bulk_write.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

update::update(v1::update_many_result opts)
    : _result{v_noabi::from_v1(std::move(v1::update_many_result::internal::result(opts)))} {}

update::update(v1::update_one_result opts)
    : _result{v_noabi::from_v1(std::move(v1::update_one_result::internal::result(opts)))} {}

update::operator v1::update_many_result() const {
    return v1::update_many_result::internal::make(v_noabi::to_v1(_result));
}

update::operator v1::update_one_result() const {
    return v1::update_one_result::internal::make(v_noabi::to_v1(_result));
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
