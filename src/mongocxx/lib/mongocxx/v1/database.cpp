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

#include <mongocxx/v1/database.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/gridfs/bucket.hpp>
#include <mongocxx/v1/pipeline.hpp>

#include <mongocxx/v1/aggregate_options.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/collection.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class database::impl {
   public:
    mongoc_database_t* _db;
    mongoc_client_t* _client;

    ~impl() {
        libmongoc::database_destroy(_db);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;

    impl(impl const& other) : _db{libmongoc::database_copy(other._db)}, _client{other._client} {}

    impl& operator=(impl const& other) = delete;

    impl(mongoc_database_t* db, mongoc_client_t* client) : _db{db}, _client{client} {}

    static impl const& with(database const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(database const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(database& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(database* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

database::~database() {
    delete impl::with(_impl);
}

database::database(database&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

database& database::operator=(database&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

database::database(database const& other) : _impl{new impl{impl::with(other)}} {}

database& database::operator=(database const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

database::database() : _impl{nullptr} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

database::operator bool() const {
    return _impl != nullptr;
}

namespace {

v1::cursor aggregate_impl(
    mongoc_database_t* db,
    bsoncxx::v1::array::view pipeline,
    bson_t const* opts,
    v1::aggregate_options const& aggregate_opts) {
    auto const& rp_opt = v1::aggregate_options::internal::read_preference(aggregate_opts);
    auto const read_prefs = rp_opt ? v1::read_preference::internal::as_mongoc(*rp_opt) : nullptr;

    return v1::cursor::internal::make(
        libmongoc::database_aggregate(db, scoped_bson_view{pipeline}.bson(), opts, read_prefs));
}

} // namespace

v1::cursor database::aggregate(v1::pipeline const& pipeline, v1::aggregate_options const& opts) {
    scoped_bson doc;

    v1::aggregate_options::internal::append_to(opts, doc);

    return aggregate_impl(impl::with(this)->_db, pipeline.view_array(), doc.bson(), opts);
}

v1::cursor database::aggregate(
    v1::client_session const& session,
    v1::pipeline const& pipeline,
    v1::aggregate_options const& opts) {
    scoped_bson doc;

    v1::aggregate_options::internal::append_to(opts, doc);
    v1::client_session::internal::append_to(session, doc);

    return aggregate_impl(impl::with(this)->_db, pipeline.view_array(), doc.bson(), opts);
}

namespace {

bsoncxx::v1::document::value
run_command_impl(mongoc_database_t* db, bsoncxx::v1::document::view command, bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::database_command_with_opts(
            db, scoped_bson_view{command}.bson(), nullptr, opts, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return std::move(reply).value();
}

} // namespace

bsoncxx::v1::document::value database::run_command(bsoncxx::v1::document::view command) {
    return run_command_impl(impl::with(this)->_db, command, nullptr);
}

bsoncxx::v1::document::value database::run_command(
    v1::client_session const& session,
    bsoncxx::v1::document::view command) {
    scoped_bson opts;

    v1::client_session::internal::append_to(session, opts);

    return run_command_impl(impl::with(this)->_db, command, opts.bson());
}

bsoncxx::v1::document::value database::run_command(bsoncxx::v1::document::view command, std::uint32_t server_id) {
    scoped_bson reply;
    bson_error_t error = {};

    auto const _db = impl::with(this)->_db;
    auto const _client = impl::with(this)->_client;

    if (!libmongoc::client_command_simple_with_server_id(
            _client,
            libmongoc::database_get_name(_db),
            scoped_bson_view{command}.bson(),
            libmongoc::database_get_read_prefs(_db),
            server_id,
            reply.out_ptr(),
            &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return std::move(reply).value();
}

namespace {

v1::collection
create_collection_impl(mongoc_database_t* db, mongoc_client_t* client, char const* name, bson_t const* opts) {
    bson_error_t error = {};

    if (auto const ptr = libmongoc::database_create_collection(db, name, opts, &error)) {
        return v1::collection::internal::make(ptr, client);
    }

    v1::throw_exception(error);
}

} // namespace

v1::collection database::create_collection(
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view opts,
    bsoncxx::v1::stdx::optional<v1::write_concern> const& wc) {
    scoped_bson doc;

    doc += opts;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))};
    }

    return create_collection_impl(
        impl::with(this)->_db, impl::with(this)->_client, std::string{name}.c_str(), doc.bson());
}

v1::collection database::create_collection(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view opts,
    bsoncxx::v1::stdx::optional<v1::write_concern> const& wc) {
    scoped_bson doc;

    doc += opts;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))};
    }

    v1::client_session::internal::append_to(session, doc);

    return create_collection_impl(
        impl::with(this)->_db, impl::with(this)->_client, std::string{name}.c_str(), doc.bson());
}

namespace {

void drop_impl(mongoc_database_t* db, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::database_drop_with_opts(db, opts, &error)) {
        v1::throw_exception(error);
    }
}

} // namespace

void database::drop(bsoncxx::v1::stdx::optional<v1::write_concern> const& wc) {
    if (wc) {
        drop_impl(
            impl::with(this)->_db,
            scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))}.bson());
    } else {
        drop_impl(impl::with(this)->_db, nullptr);
    }
}

void database::drop(v1::client_session const& session, bsoncxx::v1::stdx::optional<v1::write_concern> const& wc) {
    scoped_bson opts;

    if (wc) {
        opts += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))};
    }

    v1::client_session::internal::append_to(session, opts);

    drop_impl(impl::with(this)->_db, opts.bson());
}

bool database::has_collection(bsoncxx::v1::stdx::string_view name) {
    bson_error_t error = {};

    auto const ret = libmongoc::database_has_collection(impl::with(this)->_db, std::string{name}.c_str(), &error);

    if (error.code != 0) {
        v1::throw_exception(error);
    }

    return ret;
}

namespace {

v1::cursor list_collections_impl(mongoc_database_t* db, bson_t const* opts) {
    return v1::cursor::internal::make(libmongoc::database_find_collections_with_opts(db, opts));
}

} // namespace

v1::cursor database::list_collections(bsoncxx::v1::document::view filter) {
    scoped_bson opts{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};

    return list_collections_impl(impl::with(this)->_db, opts.bson());
}

v1::cursor database::list_collections(v1::client_session const& session, bsoncxx::v1::document::view filter) {
    scoped_bson opts{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};

    v1::client_session::internal::append_to(session, opts);

    return list_collections_impl(impl::with(this)->_db, opts.bson());
}

namespace {

std::vector<std::string> list_collection_names_impl(mongoc_database_t* db, bson_t const* opts) {
    struct names_deleter {
        void operator()(char** ptr) const noexcept {
            bson_strfreev(ptr);
        }
    };

    using names_type = std::unique_ptr<char*, names_deleter>;

    bson_error_t error = {};

    if (auto const names = names_type{libmongoc::database_get_collection_names_with_opts(db, opts, &error)}) {
        std::vector<std::string> ret;

        for (char const* const* iter = names.get(); *iter != nullptr; ++iter) {
            ret.emplace_back(*iter);
        }

        return ret;
    }

    v1::throw_exception(error);
}

} // namespace

std::vector<std::string> database::list_collection_names(bsoncxx::v1::document::view filter) {
    scoped_bson opts{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};

    return list_collection_names_impl(impl::with(this)->_db, opts.bson());
}

std::vector<std::string> database::list_collection_names(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter) {
    scoped_bson opts{BCON_NEW("filter", BCON_DOCUMENT(scoped_bson_view{filter}.bson()))};

    v1::client_session::internal::append_to(session, opts);

    return list_collection_names_impl(impl::with(this)->_db, opts.bson());
}

bsoncxx::v1::stdx::string_view database::name() const {
    return libmongoc::database_get_name(impl::with(this)->_db);
}

void database::read_concern(v1::read_concern const& rc) {
    libmongoc::database_set_read_concern(impl::with(this)->_db, v1::read_concern::internal::as_mongoc(rc));
}

v1::read_concern database::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(libmongoc::database_get_read_concern(impl::with(this)->_db)));
}

void database::read_preference(v1::read_preference const& rp) {
    libmongoc::database_set_read_prefs(impl::with(this)->_db, v1::read_preference::internal::as_mongoc(rp));
}

v1::read_preference database::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(libmongoc::database_get_read_prefs(impl::with(this)->_db)));
}

void database::write_concern(v1::write_concern const& wc) {
    libmongoc::database_set_write_concern(impl::with(this)->_db, v1::write_concern::internal::as_mongoc(wc));
}

v1::write_concern database::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(libmongoc::database_get_write_concern(impl::with(this)->_db)));
}

v1::collection database::collection(bsoncxx::v1::stdx::string_view name) const {
    return v1::collection::internal::make(
        libmongoc::database_get_collection(impl::with(this)->_db, std::string{name}.c_str()),
        impl::with(this)->_client);
}

v1::collection database::operator[](bsoncxx::v1::stdx::string_view name) const {
    return this->collection(name);
}

v1::gridfs::bucket database::gridfs_bucket(v1::gridfs::bucket::options const& opts) const {
    // TODO: v1::gridfs (CXX-3237)
    (void)opts;
    MONGOCXX_PRIVATE_UNREACHABLE;
}

namespace {

v1::change_stream watch_impl(mongoc_database_t* db, bsoncxx::v1::array::view pipeline, bson_t const* opts) {
    return v1::change_stream::internal::make(libmongoc::database_watch(db, scoped_bson_view{pipeline}.bson(), opts));
}

} // namespace

v1::change_stream database::watch(v1::change_stream::options const& opts) {
    scoped_bson doc;

    doc += v1::change_stream::options::internal::to_document(opts);

    return watch_impl(impl::with(this)->_db, bsoncxx::v1::array::view{}, doc.bson());
}

v1::change_stream database::watch(v1::client_session const& session, v1::change_stream::options const& opts) {
    scoped_bson doc;

    doc += v1::change_stream::options::internal::to_document(opts);
    v1::client_session::internal::append_to(session, doc);

    return watch_impl(impl::with(this)->_db, bsoncxx::v1::array::view{}, doc.bson());
}

v1::change_stream database::watch(v1::pipeline const& pipeline, v1::change_stream::options const& opts) {
    scoped_bson doc;

    doc += v1::change_stream::options::internal::to_document(opts);

    return watch_impl(impl::with(this)->_db, pipeline.view_array(), doc.bson());
}

v1::change_stream database::watch(
    v1::client_session const& session,
    v1::pipeline const& pipeline,
    v1::change_stream::options const& opts) {
    scoped_bson doc;

    doc += v1::change_stream::options::internal::to_document(opts);
    v1::client_session::internal::append_to(session, doc);

    return watch_impl(impl::with(this)->_db, pipeline.view_array(), doc.bson());
}

database database::internal::make(mongoc_database_t* db, mongoc_client_t* client) {
    return {new impl{db, client}};
}

mongoc_database_t const* database::internal::as_mongoc(database const& self) {
    return impl::with(self)._db;
}

mongoc_database_t* database::internal::as_mongoc(database& self) {
    return impl::with(self)._db;
}

mongoc_client_t* database::internal::get_client(database& self) {
    return impl::with(self)._client;
}

database::database(void* impl) : _impl{impl} {}

} // namespace v1
} // namespace mongocxx
