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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/error_category.hh>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/database.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/private/pipeline.hh>
#include <mongocxx/private/read_concern.hh>
#include <mongocxx/private/read_preference.hh>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::concatenate;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

namespace {
class collection_names {
   public:
    explicit collection_names(char** names) {
        _names = names;
    };

    ~collection_names() {
        bson_strfreev(_names);
    }

    const char* operator[](const std::size_t i) const {
        return _names[i];
    };

    bool operator!() const {
        return _names == nullptr;
    }

   private:
    char** _names;
};
}  // namespace

using namespace libbson;

database::database() noexcept = default;

database::database(database&&) noexcept = default;
database& database::operator=(database&&) noexcept = default;

database::~database() = default;

database::database(const class client& client, bsoncxx::string::view_or_value name)
    : _impl(stdx::make_unique<impl>(
          libmongoc::client_get_database(client._get_impl().client_t, name.terminated().data()),
          &client._get_impl(),
          name.terminated().data())) {}

database::database(const database& d) {
    if (d) {
        _impl = stdx::make_unique<impl>(d._get_impl());
    }
}

database& database::operator=(const database& d) {
    if (!d) {
        _impl.reset();
    } else if (!*this) {
        _impl = stdx::make_unique<impl>(d._get_impl());
    } else {
        *_impl = d._get_impl();
    }

    return *this;
}

database::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

cursor database::_list_collections(const client_session* session,
                                   bsoncxx::document::view_or_value filter) {
    bsoncxx::builder::basic::document options_builder;
    options_builder.append(kvp("filter", filter));

    if (session) {
        options_builder.append(
            bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());

    return libmongoc::database_find_collections_with_opts(_get_impl().database_t,
                                                          options_bson.bson());
}

cursor database::list_collections(bsoncxx::document::view_or_value filter) {
    return _list_collections(nullptr, filter);
}

cursor database::list_collections(const client_session& session,
                                  bsoncxx::document::view_or_value filter) {
    return _list_collections(&session, filter);
}

std::vector<std::string> database::_list_collection_names(const client_session* session,
                                                          bsoncxx::document::view_or_value filter) {
    bsoncxx::builder::basic::document options_builder;
    options_builder.append(kvp("filter", filter));

    if (session) {
        options_builder.append(
            bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());

    bson_error_t error;
    collection_names names(libmongoc::database_get_collection_names_with_opts(
        _get_impl().database_t, options_bson.bson(), &error));

    if (!names) {
        throw_exception<operation_exception>(error);
    }

    std::vector<std::string> _names;
    for (std::size_t i = 0; names[i]; ++i) {
        _names.emplace_back(names[i]);
    }

    return _names;
}

std::vector<std::string> database::list_collection_names(bsoncxx::document::view_or_value filter) {
    return _list_collection_names(nullptr, filter);
}

std::vector<std::string> database::list_collection_names(const client_session& session,
                                                         bsoncxx::document::view_or_value filter) {
    return _list_collection_names(&session, filter);
}

stdx::string_view database::name() const {
    return _get_impl().name;
}

bsoncxx::document::value database::_run_command(const client_session* session,
                                                bsoncxx::document::view_or_value command) {
    libbson::scoped_bson_t command_bson{command};
    libbson::scoped_bson_t reply_bson;
    bson_error_t error;

    bsoncxx::builder::basic::document options_builder;
    if (session) {
        options_builder.append(
            bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson(options_builder.extract());
    auto result = libmongoc::database_command_with_opts(_get_impl().database_t,
                                                        command_bson.bson(),
                                                        NULL,
                                                        options_bson.bson(),
                                                        reply_bson.bson_for_init(),
                                                        &error);

    if (!result) {
        throw_exception<operation_exception>(reply_bson.steal(), error);
    }

    return reply_bson.steal();
}

bsoncxx::document::value database::run_command(bsoncxx::document::view_or_value command) {
    return _run_command(nullptr, command);
}

bsoncxx::document::value database::run_command(const client_session& session,
                                               bsoncxx::document::view_or_value command) {
    return _run_command(&session, command);
}

collection database::_create_collection(const client_session* session,
                                        stdx::string_view name,
                                        bsoncxx::document::view_or_value collection_options,
                                        const stdx::optional<class write_concern>& write_concern) {
    bsoncxx::builder::basic::document options_builder;
    bson_error_t error;

    options_builder.append(bsoncxx::builder::concatenate_doc{collection_options});

    if (write_concern) {
        options_builder.append(kvp("writeConcern", write_concern->to_document()));
    }

    if (session) {
        options_builder.append(
            bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    libbson::scoped_bson_t opts_bson{options_builder.view()};
    auto result = libmongoc::database_create_collection(
        _get_impl().database_t, name.to_string().c_str(), opts_bson.bson(), &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }

    return mongocxx::collection(*this, result);
}

collection database::_create_collection_deprecated(
    const client_session* session,
    bsoncxx::string::view_or_value name,
    const options::create_collection& collection_options,
    const stdx::optional<class write_concern>& write_concern) {
    bsoncxx::builder::basic::document options_builder;

    if (collection_options.auto_index_id_deprecated()) {
        options_builder.append(kvp("autoIndexId", *collection_options.auto_index_id_deprecated()));
    }

    if (collection_options.capped()) {
        options_builder.append(kvp("capped", *collection_options.capped()));
    }

    if (collection_options.collation()) {
        options_builder.append(kvp("collation", *collection_options.collation()));
    }

    if (collection_options.max()) {
        options_builder.append(kvp("max", *collection_options.max()));
    }

    if (collection_options.no_padding()) {
        options_builder.append(kvp("flags", (*collection_options.no_padding() ? 0x10 : 0x00)));
    }

    if (collection_options.size()) {
        options_builder.append(kvp("size", *collection_options.size()));
    }

    if (collection_options.storage_engine()) {
        options_builder.append(kvp("storageEngine", *collection_options.storage_engine()));
    }

    if (collection_options.validation_criteria()) {
        auto validation_level_to_string = [](validation_criteria::validation_level level) {
            switch (level) {
                case validation_criteria::validation_level::k_off:
                    return "off";
                case validation_criteria::validation_level::k_moderate:
                    return "moderate";
                case validation_criteria::validation_level::k_strict:
                    return "strict";
            }
            MONGOCXX_UNREACHABLE;
        };

        auto validation_action_to_string = [](validation_criteria::validation_action action) {
            switch (action) {
                case validation_criteria::validation_action::k_warn:
                    return "warn";
                case validation_criteria::validation_action::k_error:
                    return "error";
            }
            MONGOCXX_UNREACHABLE;
        };

        auto validation_criteria = *collection_options.validation_criteria();

        if (validation_criteria.rule()) {
            options_builder.append(kvp("validator", *validation_criteria.rule()));
        }

        if (validation_criteria.level()) {
            options_builder.append(
                kvp("validationLevel", validation_level_to_string(*validation_criteria.level())));
        }

        if (validation_criteria.action()) {
            options_builder.append(kvp("validationAction",
                                       validation_action_to_string(*validation_criteria.action())));
        }
    }

    return _create_collection(session, name, options_builder.view(), write_concern);
}

class collection database::create_collection(
    stdx::string_view name,
    bsoncxx::document::view_or_value collection_options,
    const stdx::optional<class write_concern>& write_concern) {
    return _create_collection(nullptr, name, collection_options, write_concern);
}

class collection database::create_collection(
    const client_session& session,
    stdx::string_view name,
    bsoncxx::document::view_or_value collection_options,
    const stdx::optional<class write_concern>& write_concern) {
    return _create_collection(&session, name, collection_options, write_concern);
}

class collection database::create_collection_deprecated(
    bsoncxx::string::view_or_value name,
    const options::create_collection& collection_options,
    const stdx::optional<class write_concern>& write_concern) {
    return _create_collection_deprecated(nullptr, name, collection_options, write_concern);
}

class collection database::create_collection_deprecated(
    const client_session& session,
    bsoncxx::string::view_or_value name,
    const options::create_collection& collection_options,
    const stdx::optional<class write_concern>& write_concern) {
    return _create_collection_deprecated(&session, name, collection_options, write_concern);
}

class collection database::create_view(bsoncxx::string::view_or_value name,
                                       bsoncxx::string::view_or_value view_on,
                                       const options::create_view& options) {
    return create_view_deprecated(name, view_on, options);
}

class collection database::create_view_deprecated(bsoncxx::string::view_or_value name,
                                                  bsoncxx::string::view_or_value view_on,
                                                  const options::create_view& options) {
    bsoncxx::builder::basic::document options_builder;
    options_builder.append(kvp("viewOn", view_on));

    if (options.collation()) {
        options_builder.append(kvp("collation", *options.collation()));
    }

    if (options.pipeline()) {
        options_builder.append(kvp("pipeline", options.pipeline()->view_array()));
    }

    if (options.write_concern()) {
        options_builder.append(kvp("writeConcern", options.write_concern()->to_document()));
    }

    libbson::scoped_bson_t opts_bson{options_builder.view()};
    bson_error_t error;
    auto result = libmongoc::database_create_collection(
        _get_impl().database_t, name.terminated().data(), opts_bson.bson(), &error);
    if (!result) {
        throw_exception<operation_exception>(error);
    }

    return mongocxx::collection(*this, result);
}

void database::_drop(const client_session* session,
                     const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern) {
    bson_error_t error;

    bsoncxx::builder::basic::document opts_doc;
    if (write_concern) {
        opts_doc.append(kvp("writeConcern", write_concern->to_document()));
    }

    if (session) {
        opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    libbson::scoped_bson_t opts_bson{opts_doc.view()};

    if (!libmongoc::database_drop_with_opts(_get_impl().database_t, opts_bson.bson(), &error)) {
        throw_exception<operation_exception>(error);
    }
}

void database::drop(const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern) {
    return _drop(nullptr, write_concern);
}

void database::drop(const client_session& session,
                    const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern) {
    return _drop(&session, write_concern);
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
    auto result = libmongoc::database_has_collection(
        _get_impl().database_t, name.terminated().data(), &error);
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

gridfs::bucket database::gridfs_bucket(const options::gridfs::bucket& options) const {
    return gridfs::bucket{*this, options};
}

class change_stream database::watch(const options::change_stream& options) {
    return watch(pipeline{}, options);
}

class change_stream database::watch(const client_session& session,
                                    const options::change_stream& options) {
    return _watch(&session, pipeline{}, options);
}

class change_stream database::watch(const pipeline& pipe, const options::change_stream& options) {
    return _watch(nullptr, pipe, options);
}

class change_stream database::watch(const client_session& session,
                                    const pipeline& pipe,
                                    const options::change_stream& options) {
    return _watch(&session, pipe, options);
}

class change_stream database::_watch(const client_session* session,
                                     const pipeline& pipe,
                                     const options::change_stream& options) {
    bsoncxx::builder::basic::document container;
    container.append(kvp("pipeline", pipe._impl->view_array()));
    scoped_bson_t pipeline_bson{container.view()};

    bsoncxx::builder::basic::document options_builder;
    options_builder.append(bsoncxx::builder::concatenate(options.as_bson()));
    if (session) {
        options_builder.append(
            bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson{options_builder.extract()};

    return change_stream{libmongoc::database_watch(
        _get_impl().database_t, pipeline_bson.bson(), options_bson.bson())};
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
