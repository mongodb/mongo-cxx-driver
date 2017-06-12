// Copyright 2017 MongoDB Inc.
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

#include <mongocxx/index_view.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

index_view::index_view() {}

cursor index_view::iterator() {
    return cursor{nullptr};
}

std::string index_view::create_one(const bsoncxx::document::view_or_value& keys,
                                   const bsoncxx::document::view_or_value& options) {
    return "foo";
}

std::string index_view::create_one(const index_model& index) {
    return "foo";
}

template <typename container_type>
index_names index_view::create_many(const container_type& indexes) {
    index_names created_names;
    return created_names.begin();
}

void index_view::drop_one(stdx::string_view name) {}

void index_view::drop_one(const bsoncxx::document::view_or_value& keys,
                          const bsoncxx::document::view_or_value& options) {}

void index_view::drop_one(const index_model& index) {}

void index_view::drop_all() {}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
