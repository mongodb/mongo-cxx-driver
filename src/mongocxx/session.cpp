// Copyright 2017-present MongoDB Inc.
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

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/session.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

// Private constructors.
session::session(const class client* client, const mongocxx::options::session& options)
    : _impl(stdx::make_unique<impl>(client, options)) {}

session::session(session&&) noexcept = default;

session& session::operator=(session&&) noexcept = default;

session::~session() noexcept = default;

const mongocxx::client& session::client() const noexcept {
    return _impl->client();
}

const mongocxx::options::session& session::options() const noexcept {
    return _impl->options();
}

bsoncxx::document::view session::id() const noexcept {
    return _impl->id();
}

bsoncxx::document::view session::cluster_time() const noexcept {
    return _impl->cluster_time();
}

bsoncxx::types::b_timestamp session::operation_time() const noexcept {
    return _impl->operation_time();
}

void session::advance_cluster_time(const bsoncxx::document::view& cluster_time) {
    _impl->advance_cluster_time(cluster_time);
}

void session::advance_operation_time(const bsoncxx::types::b_timestamp& operation_time) {
    _impl->advance_operation_time(operation_time);
}

const session::impl& session::_get_impl() const {
    // Never null.
    return *_impl;
}

session::impl& session::_get_impl() {
    auto cthis = const_cast<const session*>(this);
    return const_cast<session::impl&>(cthis->_get_impl());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
