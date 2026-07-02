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

#include <mongocxx/v1/events/server_description.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

mongoc_server_description_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_server_description_t*>(ptr);
}

} // namespace

server_description::~server_description() {
    libmongoc::server_description_destroy(to_mongoc(_impl));
}

server_description::server_description(server_description&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

server_description& server_description::operator=(server_description&& other) noexcept {
    if (this != &other) {
        libmongoc::server_description_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }

    return *this;
}

server_description::server_description(server_description const& other)
    : _impl{libmongoc::server_description_new_copy(to_mongoc(other._impl))} {}

server_description& server_description::operator=(server_description const& other) {
    if (this != &other) {
        libmongoc::server_description_destroy(
            to_mongoc(exchange(_impl, libmongoc::server_description_new_copy(to_mongoc(other._impl)))));
    }

    return *this;
}

std::uint32_t server_description::id() const {
    return libmongoc::server_description_id(to_mongoc(_impl));
}

bsoncxx::v1::stdx::optional<std::int64_t> server_description::round_trip_time() const {
    return libmongoc::server_description_round_trip_time(to_mongoc(_impl));
}

bsoncxx::v1::stdx::string_view server_description::type() const {
    return libmongoc::server_description_type(to_mongoc(_impl));
}

bsoncxx::v1::document::view server_description::hello() const {
    return scoped_bson_view{libmongoc::server_description_hello_response(to_mongoc(_impl))}.view();
}

bsoncxx::v1::stdx::string_view server_description::host() const {
    return libmongoc::server_description_host(to_mongoc(_impl))->host;
}

std::uint16_t server_description::port() const {
    return libmongoc::server_description_host(to_mongoc(_impl))->port;
}

server_description::server_description(void* impl) : _impl{impl} {}

server_description server_description::internal::make(mongoc_server_description_t const* sd) {
    return {libmongoc::server_description_new_copy(sd)};
}

server_description server_description::internal::make(mongoc_server_description_t* sd) {
    return {sd};
}

mongoc_server_description_t const* server_description::internal::as_mongoc(server_description const& self) {
    return to_mongoc(self._impl);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
