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

#include <mongocxx/v1/events/command_started.hh>

//

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/oid.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_apm_command_started_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_command_started_t const*>(ptr);
}

} // namespace

bsoncxx::v1::document::view command_started::command() const {
    return scoped_bson_view{libmongoc::apm_command_started_get_command(to_mongoc(_impl))}.view();
}

bsoncxx::v1::stdx::string_view command_started::database_name() const {
    return libmongoc::apm_command_started_get_database_name(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view command_started::command_name() const {
    return libmongoc::apm_command_started_get_command_name(to_mongoc(_impl));
}

std::int64_t command_started::request_id() const {
    return libmongoc::apm_command_started_get_request_id(to_mongoc(_impl));
}

std::int64_t command_started::operation_id() const {
    return libmongoc::apm_command_started_get_operation_id(to_mongoc(_impl));
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::oid> command_started::service_id() const {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::oid> ret;

    if (auto const id = libmongoc::apm_command_started_get_service_id(to_mongoc(_impl))) {
        ret.emplace(id->bytes, bsoncxx::v1::oid::k_oid_length);
    }

    return ret;
}

bsoncxx::v1::stdx::string_view command_started::host() const {
    return libmongoc::apm_command_started_get_host(to_mongoc(_impl))->host;
}

std::uint16_t command_started::port() const {
    return libmongoc::apm_command_started_get_host(to_mongoc(_impl))->port;
}

command_started::command_started(void const* impl) : _impl{impl} {}

command_started command_started::internal::make(mongoc_apm_command_started_t const* ptr) {
    return {ptr};
}

mongoc_apm_command_started_t const* command_started::internal::as_mongoc(command_started const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
