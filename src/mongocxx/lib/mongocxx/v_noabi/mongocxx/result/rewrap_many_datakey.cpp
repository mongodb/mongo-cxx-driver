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

#include <mongocxx/result/rewrap_many_datakey.hpp>

//

#include <mongocxx/v1/rewrap_many_datakey_result.hh>

#include <utility>

#include <mongocxx/result/bulk_write.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

rewrap_many_datakey::rewrap_many_datakey(v1::rewrap_many_datakey_result opts)
    : _result{std::move(v1::rewrap_many_datakey_result::internal::result(opts))} {}

rewrap_many_datakey::operator v1::rewrap_many_datakey_result() const {
    auto ret = v1::rewrap_many_datakey_result::internal::make();

    if (_result) {
        v1::rewrap_many_datakey_result::internal::result(ret) = v_noabi::to_v1(*_result);
    }

    return ret;
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
