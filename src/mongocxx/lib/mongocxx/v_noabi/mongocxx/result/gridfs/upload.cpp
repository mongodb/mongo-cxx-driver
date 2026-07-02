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

#include <mongocxx/result/gridfs/upload.hpp>

//

#include <mongocxx/v1/gridfs/upload_result.hh>

#include <utility>

#include <bsoncxx/types/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {
namespace gridfs {

upload::upload(v1::gridfs::upload_result opts)
    : _id_owner{std::move(v1::gridfs::upload_result::internal::id(opts))}, _id{_id_owner} {}

upload::operator v1::gridfs::upload_result() const {
    return v1::gridfs::upload_result::internal::make(bsoncxx::v_noabi::to_v1(_id_owner));
}

} // namespace gridfs
} // namespace result
} // namespace v_noabi
} // namespace mongocxx
