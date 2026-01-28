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

#include <mongocxx/v1/delete_one_options.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {

class delete_one_options::internal {
   public:
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& collation(delete_one_options const& self);
    static bsoncxx::v1::stdx::optional<v1::hint> const& hint(delete_one_options const& self);

    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(delete_one_options& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern>& write_concern(delete_one_options& self);
    static bsoncxx::v1::stdx::optional<v1::hint>& hint(delete_one_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& let(delete_one_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& comment(delete_one_options& self);

    static void append_to(delete_one_options const& self, scoped_bson& doc);
};

} // namespace v1
} // namespace mongocxx
