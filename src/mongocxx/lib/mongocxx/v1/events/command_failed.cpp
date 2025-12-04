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

#include <mongocxx/v1/events/command_failed.hh>

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

mongoc_apm_command_failed_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_apm_command_failed_t const*>(ptr);
}

} // namespace

bsoncxx::v1::document::view command_failed::failure() const {
    return scoped_bson_view{libmongoc::apm_command_failed_get_reply(to_mongoc(_impl))}.view();
}

bsoncxx::v1::stdx::string_view command_failed::command_name() const {
    return libmongoc::apm_command_failed_get_command_name(to_mongoc(_impl));
}

std::int64_t command_failed::duration() const {
    return libmongoc::apm_command_failed_get_duration(to_mongoc(_impl));
}

std::int64_t command_failed::request_id() const {
    return libmongoc::apm_command_failed_get_request_id(to_mongoc(_impl));
}

std::int64_t command_failed::operation_id() const {
    return libmongoc::apm_command_failed_get_operation_id(to_mongoc(_impl));
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::oid> command_failed::service_id() const {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::oid> ret;

    if (auto const id = libmongoc::apm_command_failed_get_service_id(to_mongoc(_impl))) {
        ret.emplace(reinterpret_cast<std::uint8_t const*>(id), bsoncxx::v1::oid::k_oid_length);
    }

    return ret;
}

bsoncxx::v1::stdx::string_view command_failed::host() const {
    return libmongoc::apm_command_failed_get_host(to_mongoc(_impl))->host;
}

std::uint16_t command_failed::port() const {
    return libmongoc::apm_command_failed_get_host(to_mongoc(_impl))->port;
}

command_failed::command_failed(void const* impl) : _impl{impl} {}

command_failed command_failed::internal::make(mongoc_apm_command_failed_t const* ptr) {
    return {ptr};
}

mongoc_apm_command_failed_t const* command_failed::internal::as_mongoc(command_failed const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
