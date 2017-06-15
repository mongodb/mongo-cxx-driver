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

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/private/index_view.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

index_view::index_view(void* coll)
    : _impl{stdx::make_unique<impl>(static_cast<mongoc_collection_t*>(coll))} {}

index_view::~index_view() = default;

index_view::index_view(index_view&&) = default;

cursor index_view::list() {
    return _get_impl().list();
}

bsoncxx::stdx::optional<std::string> index_view::create_one(
    const bsoncxx::document::view_or_value& keys, const bsoncxx::document::view_or_value& options) {
    return create_one(index_model{keys, options});
}

bsoncxx::stdx::optional<std::string> index_view::create_one(const index_model& model) {
    return _get_impl().create_one(model);
}

bsoncxx::document::value index_view::create_many(const std::vector<index_model>& indexes) {
    return _get_impl().create_many(indexes);
}

void index_view::drop_one(bsoncxx::stdx::string_view name) {
    return _get_impl().drop_one(name);
}

void index_view::drop_one(const bsoncxx::document::view_or_value& keys,
                          const bsoncxx::document::view_or_value& options) {
    bsoncxx::document::view opts_view = options.view();

    if (opts_view["name"]) {
        drop_one(opts_view["name"].get_utf8().value.to_string());
    } else {
        drop_one(_get_impl().get_index_name_from_keys(keys));
    }
}

void index_view::drop_one(const index_model& model) {
    drop_one(model.keys(), model.options());
}

void index_view::drop_all() {
    _get_impl().drop_all();
}

index_view::impl& index_view::_get_impl() {
    return *_impl;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
