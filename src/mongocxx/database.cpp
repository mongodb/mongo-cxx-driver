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

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/error_category.hpp>
#include <mongocxx/exception/private/mongoc_error.hpp>
#include <mongocxx/private/client.hpp>
#include <mongocxx/private/database.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/private/read_concern.hpp>
#include <mongocxx/private/read_preference.hpp>

#include <mongocxx/config/private/prelude.hpp>

using bsoncxx::builder::stream::concatenate;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

database::database() noexcept = default;

database::database(database&&) noexcept = default;
database& database::operator=(database&&) noexcept = default;

database::~database() = default;

database::database(const class client& client, bsoncxx::string::view_or_value name)
    : _impl(stdx::make_unique<impl>(
          libmongoc::client_get_database(client._get_impl().client_t, name.terminated().data()),
          &client._get_impl(), name.terminated().data())) {
}

database::database(const database& d) {
    if (d) {
        _impl = stdx::make_unique<impl>(d._get_impl());
    }
}

database& database::operator=(const database& d) {
    if (d) {
        _impl = stdx::make_unique<impl>(d._get_impl());
    }
    return *this;
}

database::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

cursor database::list_collections(bsoncxx::document::view_or_value filter) {
    libbson::scoped_bson_t filter_bson{filter};
    bson_error_t error;

    auto result =
        libmongoc::database_find_collections(_get_impl().database_t, filter_bson.bson(), &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }

    return cursor(result);
}

stdx::string_view database::name() const {
    return _get_impl().name;
}

bsoncxx::document::value database::run_command(bsoncxx::document::view_or_value command) {
    libbson::scoped_bson_t command_bson{command};
    libbson::scoped_bson_t reply_bson;
    bson_error_t error;

    reply_bson.flag_init();
    auto result = libmongoc::database_command_simple(_get_impl().database_t, command_bson.bson(),
                                                     NULL, reply_bson.bson(), &error);

    if (!result) {
        throw_exception<operation_exception>(std::move(reply_bson.steal()), error);
    }

    return reply_bson.steal();
}

bsoncxx::document::value database::modify_collection(stdx::string_view name,
                                                     const options::modify_collection& options) {
    auto doc = document{} << "collMod" << name << concatenate(options.to_document()) << finalize;

    return run_command(doc.view());
}

class collection database::create_collection(bsoncxx::string::view_or_value name,
                                             const options::create_collection& options) {
    bson_error_t error;

    libbson::scoped_bson_t opts_bson{options.to_document()};
    auto result = libmongoc::database_create_collection(
        _get_impl().database_t, name.terminated().data(), opts_bson.bson(), &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }

    return mongocxx::collection(*this, static_cast<void*>(result));
}

void database::drop() {
    bson_error_t error;
    if (!libmongoc::database_drop(_get_impl().database_t, &error)) {
        throw_exception<operation_exception>(error);
    }
}

void database::read_concern(class read_concern rc) {
    libmongoc::database_set_read_concern(_get_impl().database_t, rc._impl->read_concern_t);
}

class read_concern database::read_concern() const {
    auto rc = libmongoc::database_get_read_concern(_get_impl().database_t);
    return {stdx::make_unique<read_concern::impl>(libmongoc::read_concern_copy(rc))};
}

void database::read_preference(class read_preference rp) {
    libmongoc::database_set_read_prefs(_get_impl().database_t, rp._impl->read_preference_t);
}

bool database::has_collection(bsoncxx::string::view_or_value name) const {
    bson_error_t error;
    auto result = libmongoc::database_has_collection(_get_impl().database_t,
                                                     name.terminated().data(), &error);
    if (error.domain != 0) {
        throw_exception<operation_exception>(error);
    }

    return result;
}

class read_preference database::read_preference() const {
    class read_preference rp(stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::database_get_read_prefs(_get_impl().database_t))));
    return rp;
}

void database::write_concern(class write_concern wc) {
    libmongoc::database_set_write_concern(_get_impl().database_t, wc._impl->write_concern_t);
}

class write_concern database::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(libmongoc::write_concern_copy(
        libmongoc::database_get_write_concern(_get_impl().database_t))));
    return wc;
}

collection database::collection(bsoncxx::string::view_or_value name) const {
    return mongocxx::collection(*this, std::move(name));
}

const database::impl& database::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_database_object};
    }
    return *_impl;
}

database::impl& database::_get_impl() {
    auto cthis = const_cast<const database*>(this);
    return const_cast<database::impl&>(cthis->_get_impl());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
