// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/database.hpp>

#include <utility>

#include <bsoncxx/stdx/make_unique.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation.hpp>
#include <mongocxx/private/database.hpp>
#include <mongocxx/private/client.hpp>
#include <mongocxx/private/read_preference.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/libmongoc.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

database::database(database&&) noexcept = default;
database& database::operator=(database&&) noexcept = default;

database::~database() = default;

database::database(const class client& client, bsoncxx::stdx::string_view name)
    : _impl(bsoncxx::stdx::make_unique<impl>(
          libmongoc::client_get_database(client._impl->client_t, name.data()), client._impl.get(),
          name.data())) {
}

void* database::implementation() const {
    return _impl->database_t;
}

cursor database::list_collections(bsoncxx::document::view filter) {
    libbson::scoped_bson_t filter_bson{filter};
    bson_error_t error;

    auto result = libmongoc::database_find_collections(_impl->database_t,
                                                       filter_bson.bson(), &error);

    if (!result) {
        throw exception::operation(std::make_tuple(error.message, error.code));
    }

    return cursor(result);
}
bsoncxx::stdx::string_view database::name() const {
    return _impl->name;
}

bsoncxx::document::value database::command(bsoncxx::document::view command) {
    libbson::scoped_bson_t command_bson{command};
    libbson::scoped_bson_t reply_bson;
    bson_error_t error;

    reply_bson.flag_init();
    auto result = libmongoc::database_command_simple(_impl->database_t, command_bson.bson(), NULL,
                                                     reply_bson.bson(), &error);

    if (!result) throw exception::operation(std::move(reply_bson.steal()));

    return reply_bson.steal();
}

void database::read_preference(class read_preference rp) {
    libmongoc::database_set_read_prefs(_impl->database_t, rp._impl->read_preference_t);
}

bool database::has_collection(bsoncxx::stdx::string_view name) {
    bson_error_t error;
    return libmongoc::database_has_collection(_impl->database_t, name.data(), &error);
}

class read_preference database::read_preference() const {
    class read_preference rp(bsoncxx::stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::database_get_read_prefs(_impl->database_t))));
    return rp;
}

void database::write_concern(class write_concern wc) {
    libmongoc::database_set_write_concern(_impl->database_t, wc._impl->write_concern_t);
}

class write_concern database::write_concern() const {
    class write_concern wc(bsoncxx::stdx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::database_get_write_concern(_impl->database_t))));
    return wc;
}

collection database::collection(bsoncxx::stdx::string_view name) const {
    return mongocxx::collection(*this, name);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
