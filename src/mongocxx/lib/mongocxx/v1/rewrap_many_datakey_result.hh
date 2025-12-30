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

#pragma once

#include <mongocxx/v1/rewrap_many_datakey_result.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/bulk_write.hpp>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {

class rewrap_many_datakey_result::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(rewrap_many_datakey_result) make();

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<v1::bulk_write::result>&) result(
        rewrap_many_datakey_result& self);
};

} // namespace v1
} // namespace mongocxx
