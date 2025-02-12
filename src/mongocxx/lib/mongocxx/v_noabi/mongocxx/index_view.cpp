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

#include <bsoncxx/string/to_string.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/options/index_view.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/index_view.hh>
#include <mongocxx/private/mongoc_error.hh>

namespace mongocxx {
namespace v_noabi {

index_view::index_view(void* coll, void* client)
    : _impl{
          bsoncxx::make_unique<impl>(static_cast<mongoc_collection_t*>(coll), static_cast<mongoc_client_t*>(client))} {}

index_view::index_view(index_view&&) noexcept = default;
index_view& index_view::operator=(index_view&&) noexcept = default;
index_view::~index_view() = default;

cursor index_view::list() {
    return _get_impl().list(nullptr);
}

cursor index_view::list(client_session const& session) {
    return _get_impl().list(&session);
}

bsoncxx::v_noabi::stdx::optional<std::string> index_view::create_one(
    bsoncxx::v_noabi::document::view_or_value const& keys,
    bsoncxx::v_noabi::document::view_or_value const& index_options,
    options::index_view const& options) {
    return create_one(index_model{keys, index_options}, options);
}

bsoncxx::v_noabi::stdx::optional<std::string> index_view::create_one(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value const& keys,
    bsoncxx::v_noabi::document::view_or_value const& index_options,
    options::index_view const& options) {
    return create_one(session, index_model{keys, index_options}, options);
}

bsoncxx::v_noabi::stdx::optional<std::string> index_view::create_one(
    index_model const& model,
    options::index_view const& options) {
    return _get_impl().create_one(nullptr, model, options);
}

bsoncxx::v_noabi::stdx::optional<std::string>
index_view::create_one(client_session const& session, index_model const& model, options::index_view const& options) {
    return _get_impl().create_one(&session, model, options);
}

bsoncxx::v_noabi::document::value index_view::create_many(
    std::vector<index_model> const& indexes,
    options::index_view const& options) {
    return _get_impl().create_many(nullptr, indexes, options);
}

bsoncxx::v_noabi::document::value index_view::create_many(
    client_session const& session,
    std::vector<index_model> const& indexes,
    options::index_view const& options) {
    return _get_impl().create_many(&session, indexes, options);
}

void index_view::drop_one(bsoncxx::v_noabi::stdx::string_view name, options::index_view const& options) {
    return _get_impl().drop_one(nullptr, name, options);
}

void index_view::drop_one(
    client_session const& session,
    bsoncxx::v_noabi::stdx::string_view name,
    options::index_view const& options) {
    return _get_impl().drop_one(&session, name, options);
}

void index_view::drop_one(
    bsoncxx::v_noabi::document::view_or_value const& keys,
    bsoncxx::v_noabi::document::view_or_value const& index_options,
    options::index_view const& options) {
    bsoncxx::v_noabi::document::view opts_view = index_options.view();

    if (opts_view["name"]) {
        drop_one(bsoncxx::v_noabi::string::to_string(opts_view["name"].get_string().value), options);
    } else {
        drop_one(_get_impl().get_index_name_from_keys(keys), options);
    }
}

void index_view::drop_one(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value const& keys,
    bsoncxx::v_noabi::document::view_or_value const& index_options,
    options::index_view const& options) {
    bsoncxx::v_noabi::document::view opts_view = index_options.view();

    if (opts_view["name"]) {
        drop_one(session, bsoncxx::v_noabi::string::to_string(opts_view["name"].get_string().value), options);
    } else {
        drop_one(session, _get_impl().get_index_name_from_keys(keys), options);
    }
}

void index_view::drop_one(index_model const& model, options::index_view const& options) {
    drop_one(model.keys(), model.options(), options);
}

void index_view::drop_one(client_session const& session, index_model const& model, options::index_view const& options) {
    drop_one(session, model.keys(), model.options(), options);
}

void index_view::drop_all(options::index_view const& options) {
    _get_impl().drop_all(nullptr, options);
}

void index_view::drop_all(client_session const& session, options::index_view const& options) {
    _get_impl().drop_all(&session, options);
}

index_view::impl& index_view::_get_impl() {
    return *_impl;
}

} // namespace v_noabi
} // namespace mongocxx
