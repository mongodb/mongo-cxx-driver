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

#include <mongocxx/bulk_write.hpp> // IWYU pragma: export

//

#include <mongocxx/collection-fwd.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/options/bulk_write.hpp>

namespace mongocxx {
namespace v_noabi {

class bulk_write::internal {
   public:
    static bulk_write make(collection const& coll, options::bulk_write const& opts, client_session const* session);
};

} // namespace v_noabi
} // namespace mongocxx
