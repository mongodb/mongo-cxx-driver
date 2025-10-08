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

#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/string/to_string.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/database.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/mongoc_error.hh>
#include <mongocxx/private/pipeline.hh>
#include <mongocxx/private/read_concern.hh>
#include <mongocxx/private/read_preference.hh>

using bsoncxx::v_noabi::builder::concatenate;
using bsoncxx::v_noabi::builder::basic::kvp;
using bsoncxx::v_noabi::builder::basic::make_document;

namespace mongocxx {
namespace v_noabi {

namespace {

class collection_names {
   public:
    explicit collection_names(char** names) {
        _names = names;
    }

    ~collection_names() {
        bson_strfreev(_names);
    }

    collection_names(collection_names&&) noexcept = delete;
    collection_names& operator=(collection_names&&) noexcept = delete;

    collection_names(collection_names const&) = delete;
    collection_names& operator=(collection_names const&) = delete;

    char const* operator[](std::size_t const i) const {
        return _names[i];
    }

    bool operator!() const {
        return _names == nullptr;
    }

   private:
    char** _names;
};

} // namespace

using namespace libbson;

database::database() noexcept = default;

database::database(database&&) noexcept = default;
database& database::operator=(database&&) noexcept = default;

database::~database() = default;

database::database(mongocxx::v_noabi::client const& client, bsoncxx::v_noabi::string::view_or_value name)
    : _impl(bsoncxx::make_unique<impl>(
          libmongoc::client_get_database(client._get_impl().client_t, name.terminated().data()),
          &client._get_impl(),
          name.terminated().data())) {}

database::database(database const& d) {
    if (d) {
        _impl = bsoncxx::make_unique<impl>(d._get_impl());
    }
}

database& database::operator=(database const& d) {
    if (!d) {
        _impl.reset();
    } else if (!*this) {
        _impl = bsoncxx::make_unique<impl>(d._get_impl());
    } else {
        *_impl = d._get_impl();
    }

    return *this;
}

database::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

cursor
database::_aggregate(client_session const* session, pipeline const& pipeline, options::aggregate const& options) {
    scoped_bson_t stages(bsoncxx::v_noabi::document::view(pipeline._impl->view_array()));

    bsoncxx::v_noabi::builder::basic::document b;

    options.append(b);

    if (session) {
        b.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(b.view());

    ::mongoc_read_prefs_t const* rp_ptr = nullptr;

    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    return cursor(libmongoc::database_aggregate(_get_impl().database_t, stages.bson(), options_bson.bson(), rp_ptr));
}

cursor database::aggregate(pipeline const& pipeline, options::aggregate const& options) {
    return _aggregate(nullptr, pipeline, options);
}

cursor database::aggregate(client_session const& session, pipeline const& pipeline, options::aggregate const& options) {
    return _aggregate(&session, pipeline, options);
}

cursor database::_list_collections(client_session const* session, bsoncxx::v_noabi::document::view_or_value filter) {
    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(kvp("filter", filter));

    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());

    return libmongoc::database_find_collections_with_opts(_get_impl().database_t, options_bson.bson());
}

cursor database::list_collections(bsoncxx::v_noabi::document::view_or_value filter) {
    return _list_collections(nullptr, filter);
}

cursor database::list_collections(client_session const& session, bsoncxx::v_noabi::document::view_or_value filter) {
    return _list_collections(&session, filter);
}

std::vector<std::string> database::_list_collection_names(
    client_session const* session,
    bsoncxx::v_noabi::document::view_or_value filter) {
    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(kvp("filter", filter));

    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());

    bson_error_t error;
    collection_names names(
        libmongoc::database_get_collection_names_with_opts(_get_impl().database_t, options_bson.bson(), &error));

    if (!names) {
        throw_exception<operation_exception>(error);
    }

    std::vector<std::string> _names;
    for (std::size_t i = 0; names[i]; ++i) {
        _names.emplace_back(names[i]);
    }

    return _names;
}

std::vector<std::string> database::list_collection_names(bsoncxx::v_noabi::document::view_or_value filter) {
    return _list_collection_names(nullptr, filter);
}

std::vector<std::string> database::list_collection_names(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter) {
    return _list_collection_names(&session, filter);
}

bsoncxx::v_noabi::stdx::string_view database::name() const {
    return _get_impl().name;
}

bsoncxx::v_noabi::document::value database::_run_command(
    client_session const* session,
    bsoncxx::v_noabi::document::view_or_value command) {
    libbson::scoped_bson_t command_bson{command};
    libbson::scoped_bson_t reply_bson;
    bson_error_t error;

    bsoncxx::v_noabi::builder::basic::document options_builder;
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());
    auto result = libmongoc::database_command_with_opts(
        _get_impl().database_t, command_bson.bson(), nullptr, options_bson.bson(), reply_bson.bson_for_init(), &error);

    if (!result) {
        throw_exception<operation_exception>(reply_bson.steal(), error);
    }

    return reply_bson.steal();
}

bsoncxx::v_noabi::document::value database::run_command(bsoncxx::v_noabi::document::view_or_value command) {
    return _run_command(nullptr, command);
}

bsoncxx::v_noabi::document::value database::run_command(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value command) {
    return _run_command(&session, command);
}

bsoncxx::v_noabi::document::value database::run_command(
    bsoncxx::v_noabi::document::view_or_value command,
    uint32_t server_id) {
    libbson::scoped_bson_t command_bson{command};
    libbson::scoped_bson_t reply_bson;
    bson_error_t error;

    auto result = libmongoc::client_command_simple_with_server_id(
        _get_impl().client_impl->client_t,
        _get_impl().name.c_str(),
        command_bson.bson(),
        read_preference()._impl->read_preference_t,
        server_id,
        reply_bson.bson_for_init(),
        &error);

    if (!result) {
        throw_exception<operation_exception>(reply_bson.steal(), error);
    }

    return reply_bson.steal();
}

collection database::_create_collection(
    client_session const* session,
    bsoncxx::v_noabi::stdx::string_view name,
    bsoncxx::v_noabi::document::view_or_value collection_options,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    bsoncxx::v_noabi::builder::basic::document options_builder;
    bson_error_t error;

    options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{collection_options});

    if (write_concern) {
        options_builder.append(kvp("writeConcern", write_concern->to_document()));
    }

    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    libbson::scoped_bson_t opts_bson{options_builder.view()};
    auto result = libmongoc::database_create_collection(
        _get_impl().database_t, bsoncxx::v_noabi::string::to_string(name).c_str(), opts_bson.bson(), &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }

    return mongocxx::v_noabi::collection(*this, result);
}

mongocxx::v_noabi::collection database::create_collection(
    bsoncxx::v_noabi::stdx::string_view name,
    bsoncxx::v_noabi::document::view_or_value collection_options,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    return _create_collection(nullptr, name, collection_options, write_concern);
}

mongocxx::v_noabi::collection database::create_collection(
    client_session const& session,
    bsoncxx::v_noabi::stdx::string_view name,
    bsoncxx::v_noabi::document::view_or_value collection_options,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    return _create_collection(&session, name, collection_options, write_concern);
}

void database::_drop(
    client_session const* session,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    bson_error_t error;

    bsoncxx::v_noabi::builder::basic::document opts_doc;
    if (write_concern) {
        opts_doc.append(kvp("writeConcern", write_concern->to_document()));
    }

    if (session) {
        opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    libbson::scoped_bson_t opts_bson{opts_doc.view()};

    if (!libmongoc::database_drop_with_opts(_get_impl().database_t, opts_bson.bson(), &error)) {
        throw_exception<operation_exception>(error);
    }
}

void database::drop(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    return _drop(nullptr, write_concern);
}

void database::drop(
    client_session const& session,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& write_concern) {
    return _drop(&session, write_concern);
}

void database::read_concern(mongocxx::v_noabi::read_concern rc) {
    libmongoc::database_set_read_concern(_get_impl().database_t, rc._impl->read_concern_t);
}

mongocxx::v_noabi::read_concern database::read_concern() const {
    auto rc = libmongoc::database_get_read_concern(_get_impl().database_t);
    return {bsoncxx::make_unique<read_concern::impl>(libmongoc::read_concern_copy(rc))};
}

void database::read_preference(mongocxx::v_noabi::read_preference rp) {
    libmongoc::database_set_read_prefs(_get_impl().database_t, rp._impl->read_preference_t);
}

bool database::has_collection(bsoncxx::v_noabi::string::view_or_value name) const {
    bson_error_t error;
    auto result = libmongoc::database_has_collection(_get_impl().database_t, name.terminated().data(), &error);
    if (error.domain != 0) {
        throw_exception<operation_exception>(error);
    }

    return result;
}

mongocxx::v_noabi::read_preference database::read_preference() const {
    mongocxx::v_noabi::read_preference rp(bsoncxx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::database_get_read_prefs(_get_impl().database_t))));
    return rp;
}

void database::write_concern(mongocxx::v_noabi::write_concern wc) {
    libmongoc::database_set_write_concern(_get_impl().database_t, wc._impl->write_concern_t);
}

mongocxx::v_noabi::write_concern database::write_concern() const {
    mongocxx::v_noabi::write_concern wc(bsoncxx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::database_get_write_concern(_get_impl().database_t))));
    return wc;
}

collection database::collection(bsoncxx::v_noabi::string::view_or_value name) const {
    return mongocxx::v_noabi::collection(*this, std::move(name));
}

gridfs::bucket database::gridfs_bucket(options::gridfs::bucket const& options) const {
    return gridfs::bucket{*this, options};
}

change_stream database::watch(options::change_stream const& options) {
    return watch(pipeline{}, options);
}

change_stream database::watch(client_session const& session, options::change_stream const& options) {
    return _watch(&session, pipeline{}, options);
}

change_stream database::watch(pipeline const& pipe, options::change_stream const& options) {
    return _watch(nullptr, pipe, options);
}

change_stream
database::watch(client_session const& session, pipeline const& pipe, options::change_stream const& options) {
    return _watch(&session, pipe, options);
}

change_stream
database::_watch(client_session const* session, pipeline const& pipe, options::change_stream const& options) {
    bsoncxx::v_noabi::builder::basic::document container;
    container.append(kvp("pipeline", pipe._impl->view_array()));
    scoped_bson_t pipeline_bson{container.view()};

    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(bsoncxx::v_noabi::builder::concatenate(options.as_bson()));
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson{options_builder.extract()};

    return change_stream{libmongoc::database_watch(_get_impl().database_t, pipeline_bson.bson(), options_bson.bson())};
}

database::impl const& database::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_database_object};
    }
    return *_impl;
}

database::impl& database::_get_impl() {
    auto cthis = const_cast<database const*>(this);
    return const_cast<database::impl&>(cthis->_get_impl());
}

} // namespace v_noabi
} // namespace mongocxx
