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

#include <mongocxx/database.hh>

//

#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/change_stream.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/options/gridfs/bucket.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/database.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/aggregate.hh>
#include <mongocxx/options/change_stream.hh>
#include <mongocxx/pipeline.hh>
#include <mongocxx/read_concern.hh>
#include <mongocxx/read_preference.hh>
#include <mongocxx/scoped_bson.hh>
#include <mongocxx/write_concern.hh>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

template <typename Database>
Database& check_moved_from(Database& db) {
    if (!db) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_database_object};
    }
    return db;
}

} // namespace

database::database(database const& other) {
    if (other) {
        _db = check_moved_from(other)._db;
    }
}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by v1::database.
database& database::operator=(database const& other) {
    if (this != &other) {
        if (!other) {
            _db = v1::database{};
        } else {
            _db = check_moved_from(other)._db;
        }
    }
    return *this;
}

namespace {

v_noabi::cursor aggregate_impl(
    v1::database& db,
    v_noabi::pipeline const& pipeline,
    bson_t const* opts,
    v_noabi::options::aggregate const& aggregate_opts) {
    auto const& rp_opt = aggregate_opts.read_preference();
    auto const read_prefs = rp_opt ? v_noabi::read_preference::internal::as_mongoc(*rp_opt) : nullptr;

    return v1::cursor::internal::make(
        libmongoc::database_aggregate(
            v1::database::internal::as_mongoc(check_moved_from(db)),
            to_scoped_bson_view(pipeline.view_array()).bson(),
            opts,
            read_prefs));
}

} // namespace

v_noabi::cursor database::aggregate(v_noabi::pipeline const& pipeline, v_noabi::options::aggregate const& options) {
    scoped_bson doc;
    v_noabi::options::aggregate::internal::append_to(options, doc);
    return aggregate_impl(_db, pipeline, doc.bson(), options);
}

v_noabi::cursor database::aggregate(
    v_noabi::client_session const& session,
    v_noabi::pipeline const& pipeline,
    v_noabi::options::aggregate const& options) {
    scoped_bson doc;
    v_noabi::options::aggregate::internal::append_to(options, doc);
    v_noabi::client_session::internal::append_to(session, doc);
    return aggregate_impl(_db, pipeline, doc.bson(), options);
}

namespace {

v_noabi::cursor list_collections_impl(v1::database& db, bson_t const* opts) {
    return v1::cursor::internal::make(
        libmongoc::database_find_collections_with_opts(v1::database::internal::as_mongoc(check_moved_from(db)), opts));
}

} // namespace

v_noabi::cursor database::list_collections(bsoncxx::v_noabi::document::view_or_value filter) {
    scoped_bson doc{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    return list_collections_impl(_db, doc.bson());
}

v_noabi::cursor database::list_collections(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter) {
    scoped_bson doc{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    v_noabi::client_session::internal::append_to(session, doc);

    return list_collections_impl(_db, doc.bson());
}

namespace {

std::vector<std::string> list_collection_names_impl(v1::database& db, bson_t const* opts) {
    struct names_deleter {
        void operator()(char** ptr) const noexcept {
            bson_strfreev(ptr);
        }
    };

    using names_type = std::unique_ptr<char*, names_deleter>;

    bson_error_t error = {};

    if (auto const names = names_type{libmongoc::database_get_collection_names_with_opts(
            v1::database::internal::as_mongoc(check_moved_from(db)), opts, &error)}) {
        std::vector<std::string> ret;

        for (char const* const* iter = names.get(); *iter != nullptr; ++iter) {
            ret.emplace_back(*iter);
        }

        return ret;
    }

    v_noabi::throw_exception<v_noabi::operation_exception>(error);
}

} // namespace

std::vector<std::string> database::list_collection_names(bsoncxx::v_noabi::document::view_or_value filter) {
    scoped_bson doc{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    return list_collection_names_impl(_db, doc.bson());
}

std::vector<std::string> database::list_collection_names(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter) {
    scoped_bson doc{BCON_NEW("filter", BCON_DOCUMENT(to_scoped_bson_view(filter).bson()))};

    v_noabi::client_session::internal::append_to(session, doc);

    return list_collection_names_impl(_db, doc.bson());
}

bsoncxx::v_noabi::stdx::string_view database::name() const {
    return check_moved_from(_db).name();
}

namespace {

bsoncxx::v_noabi::document::value run_command_impl(v1::database& db, bson_t const* command, bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::database_command_with_opts(
            v1::database::internal::as_mongoc(check_moved_from(db)), command, nullptr, opts, reply.out_ptr(), &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
    }

    return from_v1(std::move(reply));
}

} // namespace

bsoncxx::v_noabi::document::value database::run_command(bsoncxx::v_noabi::document::view_or_value command) {
    return run_command_impl(_db, to_scoped_bson_view(command).bson(), nullptr);
}

bsoncxx::v_noabi::document::value database::run_command(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value command) {
    scoped_bson doc;

    v_noabi::client_session::internal::append_to(session, doc);

    return run_command_impl(_db, to_scoped_bson_view(command).bson(), doc.bson());
}

bsoncxx::v_noabi::document::value database::run_command(
    bsoncxx::v_noabi::document::view_or_value command,
    std::uint32_t server_id) {
    scoped_bson reply;
    bson_error_t error;

    auto& db = check_moved_from(_db);
    auto const ptr = v1::database::internal::as_mongoc(db);

    if (!libmongoc::client_command_simple_with_server_id(
            v1::database::internal::get_client(db),
            libmongoc::database_get_name(ptr),
            to_scoped_bson_view(command.view()),
            libmongoc::database_get_read_prefs(ptr),
            server_id,
            reply.out_ptr(),
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
    }

    return from_v1(std::move(reply));
}

namespace {

mongoc_collection_t* create_collection_impl(v1::database& db, char const* name, bson_t const* opts) {
    bson_error_t error = {};

    if (auto const ptr = libmongoc::database_create_collection(
            v1::database::internal::as_mongoc(check_moved_from(db)), name, opts, &error)) {
        return ptr;
    }

    v_noabi::throw_exception<v_noabi::operation_exception>(error);
}

} // namespace

v_noabi::collection database::create_collection(
    bsoncxx::v_noabi::stdx::string_view name,
    bsoncxx::v_noabi::document::view_or_value collection_options,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern) {
    scoped_bson doc;

    doc += to_scoped_bson_view(collection_options);

    if (write_concern) {
        doc +=
            scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(write_concern->to_document()).bson()))};
    }

    return v_noabi::collection(*this, create_collection_impl(_db, std::string{name}.c_str(), doc.bson()));
}

v_noabi::collection database::create_collection(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::string_view name,
    bsoncxx::v_noabi::document::view_or_value collection_options,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern) {
    scoped_bson doc;

    doc += to_scoped_bson_view(collection_options);

    if (write_concern) {
        doc +=
            scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(write_concern->to_document()).bson()))};
    }

    v_noabi::client_session::internal::append_to(session, doc);

    return v_noabi::collection(*this, create_collection_impl(_db, std::string{name}.c_str(), doc.bson()));
}

namespace {

void drop_impl(v1::database& db, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::database_drop_with_opts(v1::database::internal::as_mongoc(check_moved_from(db)), opts, &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }
}

} // namespace

void database::drop(bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern) {
    scoped_bson doc;

    if (write_concern) {
        doc += scoped_bson{
            BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson_view(write_concern->to_document()).bson()))};
    }

    drop_impl(_db, doc.bson());
}

void database::drop(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern) {
    scoped_bson doc;

    if (write_concern) {
        doc += scoped_bson{
            BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson_view(write_concern->to_document()).bson()))};
    }

    v_noabi::client_session::internal::append_to(session, doc);

    drop_impl(_db, doc.bson());
}

void database::read_concern(v_noabi::read_concern rc) {
    return check_moved_from(_db).read_concern(v_noabi::to_v1(std::move(rc)));
}

v_noabi::read_concern database::read_concern() const {
    return check_moved_from(_db).read_concern();
}

void database::read_preference(v_noabi::read_preference rp) {
    return check_moved_from(_db).read_preference(v_noabi::to_v1(std::move(rp)));
}

bool database::has_collection(bsoncxx::v_noabi::string::view_or_value name) const {
    // Backward compatibility: `has_collection()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& d = const_cast<v1::database&>(check_moved_from(_db));

    bson_error_t error = {};

    auto const ret =
        libmongoc::database_has_collection(v1::database::internal::as_mongoc(d), name.terminated().data(), &error);

    if (error.domain != 0) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }

    return ret;
}

v_noabi::read_preference database::read_preference() const {
    return check_moved_from(_db).read_preference();
}

void database::write_concern(v_noabi::write_concern wc) {
    return check_moved_from(_db).write_concern(v_noabi::to_v1(std::move(wc)));
}

v_noabi::write_concern database::write_concern() const {
    return check_moved_from(_db).write_concern();
}

collection database::collection(bsoncxx::v_noabi::string::view_or_value name) const {
    // Backward compatibility: `collection()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& d = const_cast<v_noabi::database&>(check_moved_from(*this));

    return v_noabi::collection(d, std::move(name));
}

v_noabi::gridfs::bucket database::gridfs_bucket(v_noabi::options::gridfs::bucket const& options) const {
    // Backward compatibility: `gridfs_bucket()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& d = const_cast<v_noabi::database&>(check_moved_from(*this));

    return v_noabi::gridfs::bucket{d, options};
}

namespace {

v_noabi::change_stream watch_impl(v1::database& db, bsoncxx::v_noabi::array::view pipeline, bson_t const* opts) {
    return v1::change_stream::internal::make(
        libmongoc::database_watch(
            v1::database::internal::as_mongoc(check_moved_from(db)),
            scoped_bson{BCON_NEW("pipeline", BCON_ARRAY(to_scoped_bson_view(pipeline).bson()))}.bson(),
            opts));
}

} // namespace

v_noabi::change_stream database::watch(v_noabi::options::change_stream const& options) {
    scoped_bson doc;

    doc += to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));

    return watch_impl(_db, bsoncxx::v_noabi::array::view{}, doc.bson());
}

v_noabi::change_stream database::watch(client_session const& session, v_noabi::options::change_stream const& options) {
    scoped_bson doc;

    doc += to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));
    v_noabi::client_session::internal::append_to(session, doc);

    return watch_impl(_db, bsoncxx::v_noabi::array::view{}, doc.bson());
}

v_noabi::change_stream database::watch(v_noabi::pipeline const& pipe, v_noabi::options::change_stream const& options) {
    scoped_bson doc;

    doc += to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));

    return watch_impl(_db, pipe.view_array(), doc.bson());
}

v_noabi::change_stream database::watch(
    client_session const& session,
    v_noabi::pipeline const& pipe,
    v_noabi::options::change_stream const& options) {
    scoped_bson doc;

    doc += to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));
    v_noabi::client_session::internal::append_to(session, doc);

    return watch_impl(_db, pipe.view_array(), doc.bson());
}

mongoc_database_t* database::internal::as_mongoc(database& self) {
    return v1::database::internal::as_mongoc(check_moved_from(self._db));
}

mongoc_client_t* database::internal::get_client(database& self) {
    return v1::database::internal::get_client(check_moved_from(self._db));
}

} // namespace v_noabi
} // namespace mongocxx
