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

#include <mongocxx/v1/find_one_and_delete_options.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

namespace mongocxx {
namespace v1 {

class find_one_and_delete_options::internal {
   public:
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& projection(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& sort(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern>& write_concern(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<v1::hint>& hint(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& let(find_one_and_delete_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& comment(find_one_and_delete_options& self);
};

} // namespace v1
} // namespace mongocxx
