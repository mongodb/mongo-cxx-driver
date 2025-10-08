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

#include <chrono>
#include <cstdint>
#include <limits>
#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/write_concern.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/helpers.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/bulk_write.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/collection.hh>
#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/mongoc_error.hh>
#include <mongocxx/private/pipeline.hh>
#include <mongocxx/private/read_concern.hh>
#include <mongocxx/private/read_preference.hh>
#include <mongocxx/private/write_concern.hh>

using bsoncxx::v_noabi::builder::concatenate;
using bsoncxx::v_noabi::builder::basic::kvp;
using bsoncxx::v_noabi::builder::basic::make_array;
using bsoncxx::v_noabi::builder::basic::make_document;
using bsoncxx::v_noabi::builder::basic::sub_array;
using bsoncxx::v_noabi::builder::basic::sub_document;
using bsoncxx::v_noabi::document::view_or_value;

namespace {

using mongocxx::libbson::scoped_bson_t;

char const* get_collection_name(mongoc_collection_t* collection) {
    return mongocxx::libmongoc::collection_get_name(collection);
}

void destroy_fam_opts(mongoc_find_and_modify_opts_t* opts) {
    mongocxx::libmongoc::find_and_modify_opts_destroy(opts);
}

template <typename T>
bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_and_modify(
    mongoc_collection_t* collection_t,
    mongoc_client_session_t const* session_t,
    view_or_value filter,
    view_or_value* update,
    mongoc_find_and_modify_flags_t flags,
    bool bypass,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> array_filters,
    T const& options) {
    using unique_opts =
        std::unique_ptr<mongoc_find_and_modify_opts_t, std::function<void(mongoc_find_and_modify_opts_t*)>>;

    auto opts = unique_opts(mongocxx::libmongoc::find_and_modify_opts_new(), destroy_fam_opts);

    bsoncxx::v_noabi::builder::basic::document extra;
    ::bson_error_t error;

    // Write concern, collation, and session are passed in "extra".
    if (auto const& wc = options.write_concern()) {
        if (!wc->is_acknowledged() && options.collation()) {
            throw mongocxx::v_noabi::logic_error{mongocxx::v_noabi::error_code::k_invalid_parameter};
        }
        extra.append(kvp("writeConcern", wc->to_document()));
    }

    if (session_t) {
        bson_t bson = BSON_INITIALIZER;
        if (!mongocxx::libmongoc::client_session_append(session_t, &bson, &error)) {
            bson_destroy(&bson);
            throw mongocxx::v_noabi::logic_error{mongocxx::v_noabi::error_code::k_invalid_session, error.message};
        }

        // document::value takes ownership of the bson buffer.
        bsoncxx::v_noabi::document::value session_id{bsoncxx::helpers::value_from_bson_t(&bson)};
        extra.append(concatenate(session_id.view()));
    }

    if (auto const& collation = options.collation()) {
        extra.append(kvp("collation", *collation));
    }

    if (array_filters) {
        extra.append(kvp("arrayFilters", *array_filters));
    }

    if (auto const& hint = options.hint()) {
        extra.append(kvp("hint", hint->to_value()));
    }

    if (auto const& let = options.let()) {
        extra.append(kvp("let", *let));
    }

    if (auto const& comment = options.comment()) {
        extra.append(kvp("comment", *comment));
    }

    scoped_bson_t extra_bson{extra.view()};
    mongocxx::libmongoc::find_and_modify_opts_append(opts.get(), extra_bson.bson());

    if (update) {
        scoped_bson_t update_bson{update->view()};
        mongocxx::libmongoc::find_and_modify_opts_set_update(opts.get(), update_bson.bson());
    }

    if (bypass) {
        mongocxx::libmongoc::find_and_modify_opts_set_bypass_document_validation(opts.get(), true);
    }

    if (auto const& sort = options.sort()) {
        scoped_bson_t sort_bson{*sort};
        mongocxx::libmongoc::find_and_modify_opts_set_sort(opts.get(), sort_bson.bson());
    }

    if (auto const& projection = options.projection()) {
        scoped_bson_t projection_bson{*projection};
        mongocxx::libmongoc::find_and_modify_opts_set_fields(opts.get(), projection_bson.bson());
    }

    if (auto const& max_time = options.max_time()) {
        mongocxx::libmongoc::find_and_modify_opts_set_max_time_ms(opts.get(), static_cast<uint32_t>(max_time->count()));
    }

    // Upsert, remove, and new are passed in flags.
    mongocxx::libmongoc::find_and_modify_opts_set_flags(opts.get(), flags);

    // Call mongoc_collection_find_and_modify_with_opts.
    scoped_bson_t filter_bson{filter.view()};
    mongocxx::libbson::scoped_bson_t reply;

    bool result = mongocxx::libmongoc::collection_find_and_modify_with_opts(
        collection_t, filter_bson.bson(), opts.get(), reply.bson_for_init(), &error);

    if (!result) {
        if (!reply.view().empty()) {
            mongocxx::v_noabi::throw_exception<mongocxx::v_noabi::write_exception>(reply.steal(), error);
        }
        mongocxx::v_noabi::throw_exception<mongocxx::v_noabi::write_exception>(error);
    }

    bsoncxx::v_noabi::document::view reply_view = reply.view();

    auto const value = reply_view["value"];

    if (!value || value.type() == bsoncxx::v_noabi::type::k_null) {
        return bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>{};
    }

    return bsoncxx::v_noabi::document::value{reply_view["value"].get_document().view()};
}

} // namespace

namespace mongocxx {
namespace v_noabi {

using namespace libbson;

collection::collection() noexcept = default;
collection::collection(collection&&) noexcept = default;
collection& collection::operator=(collection&&) noexcept = default;
collection::~collection() = default;

collection::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

bsoncxx::v_noabi::stdx::string_view collection::name() const {
    return {get_collection_name(_get_impl().collection_t)};
}

void collection::_rename(
    client_session const* session,
    bsoncxx::v_noabi::string::view_or_value new_name,
    bool drop_target_before_rename,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc) {
    bson_error_t error;

    bsoncxx::v_noabi::builder::basic::document opts_doc;
    if (wc) {
        opts_doc.append(kvp("writeConcern", wc->to_document()));
    }

    if (session) {
        opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t opts_bson{opts_doc.view()};

    bool result = libmongoc::collection_rename_with_opts(
        _get_impl().collection_t,
        _get_impl().database_name.c_str(),
        new_name.terminated().data(),
        drop_target_before_rename,
        opts_bson.bson(),
        &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }
}

void collection::rename(
    bsoncxx::v_noabi::string::view_or_value new_name,
    bool drop_target_before_rename,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc) {
    return _rename(nullptr, new_name, drop_target_before_rename, wc);
}

void collection::rename(
    client_session const& session,
    bsoncxx::v_noabi::string::view_or_value new_name,
    bool drop_target_before_rename,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc) {
    return _rename(&session, new_name, drop_target_before_rename, wc);
}

collection::collection(database const& database, bsoncxx::v_noabi::string::view_or_value collection_name)
    : _impl(bsoncxx::make_unique<impl>(
          libmongoc::database_get_collection(database._get_impl().database_t, collection_name.terminated().data()),
          database.name(),
          database._get_impl().client_impl)) {}

collection::collection(database const& database, void* collection)
    : _impl(bsoncxx::make_unique<impl>(
          static_cast<mongoc_collection_t*>(collection),
          database.name(),
          database._get_impl().client_impl)) {}

collection::collection(collection const& c) {
    if (c) {
        _impl = bsoncxx::make_unique<impl>(c._get_impl());
    }
}

collection& collection::operator=(collection const& c) {
    if (!c) {
        _impl.reset();
    } else if (!*this) {
        _impl = bsoncxx::make_unique<impl>(c._get_impl());
    } else {
        *_impl = c._get_impl();
    }

    return *this;
}

mongocxx::v_noabi::bulk_write collection::create_bulk_write(options::bulk_write const& options) {
    return mongocxx::v_noabi::bulk_write{*this, options};
}

mongocxx::v_noabi::bulk_write collection::create_bulk_write(
    client_session const& session,
    options::bulk_write const& options) {
    return mongocxx::v_noabi::bulk_write{*this, options, &session};
}

namespace {

bsoncxx::v_noabi::builder::basic::document build_find_options_document(options::find const& options) {
    bsoncxx::v_noabi::builder::basic::document options_builder;

    if (auto const& adu = options.allow_disk_use()) {
        options_builder.append(kvp("allowDiskUse", *adu));
    }

    if (auto const& apr = options.allow_partial_results()) {
        options_builder.append(kvp("allowPartialResults", *apr));
    }

    if (auto const& batch_size = options.batch_size()) {
        options_builder.append(kvp("batchSize", *batch_size));
    }

    if (auto const& collation = options.collation()) {
        options_builder.append(kvp("collation", *collation));
    }

    // Prioritize new comment option over old $comment modifier.
    if (auto const& new_comment = options.comment_option()) {
        options_builder.append(kvp("comment", *new_comment));
    } else if (auto const& old_comment = options.comment()) {
        options_builder.append(kvp("comment", *old_comment));
    }

    if (auto const& cursor_type = options.cursor_type()) {
        if (*cursor_type == cursor::type::k_tailable) {
            options_builder.append(kvp("tailable", bsoncxx::v_noabi::types::b_bool{true}));
        } else if (*cursor_type == cursor::type::k_tailable_await) {
            options_builder.append(kvp("tailable", bsoncxx::v_noabi::types::b_bool{true}));
            options_builder.append(kvp("awaitData", bsoncxx::v_noabi::types::b_bool{true}));
        } else if (*cursor_type == cursor::type::k_non_tailable) {
        } else {
            throw logic_error{error_code::k_invalid_parameter};
        }
    }

    if (auto const& hint = options.hint()) {
        options_builder.append(kvp("hint", hint->to_value()));
    }

    if (auto const& let = options.let()) {
        options_builder.append(kvp("let", *let));
    }

    if (auto const& limit = options.limit()) {
        options_builder.append(kvp("limit", *limit));
    }

    if (auto const& max = options.max()) {
        options_builder.append(kvp("max", *max));
    }

    if (auto const& max_time = options.max_time()) {
        options_builder.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{max_time->count()}));
    }

    if (auto const& min = options.min()) {
        options_builder.append(kvp("min", *min));
    }

    if (auto const& nct = options.no_cursor_timeout()) {
        options_builder.append(kvp("noCursorTimeout", *nct));
    }

    if (auto const& projection = options.projection()) {
        options_builder.append(kvp("projection", bsoncxx::v_noabi::types::b_document{*projection}));
    }

    if (auto const& return_key = options.return_key()) {
        options_builder.append(kvp("returnKey", *return_key));
    }

    if (auto const& show_record_id = options.show_record_id()) {
        options_builder.append(kvp("showRecordId", *show_record_id));
    }

    if (auto const& skip = options.skip()) {
        options_builder.append(kvp("skip", *skip));
    }

    if (auto const& sort = options.sort()) {
        options_builder.append(kvp("sort", bsoncxx::v_noabi::types::b_document{*sort}));
    }

    return options_builder;
}

} // namespace

cursor collection::_find(client_session const* session, view_or_value filter, options::find const& options) {
    scoped_bson_t filter_bson{std::move(filter)};

    mongoc_read_prefs_t const* rp_ptr = nullptr;
    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    bsoncxx::v_noabi::builder::basic::document options_builder{build_find_options_document(options)};
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson{options_builder.extract()};

    cursor query_cursor{
        libmongoc::collection_find_with_opts(_get_impl().collection_t, filter_bson.bson(), options_bson.bson(), rp_ptr),
        options.cursor_type()};

    if (options.max_await_time()) {
        auto const count = options.max_await_time()->count();
        if ((count < 0) || (count >= std::numeric_limits<std::uint32_t>::max())) {
            throw logic_error{error_code::k_invalid_parameter};
        }
        libmongoc::cursor_set_max_await_time_ms(query_cursor._impl->cursor_t, static_cast<std::uint32_t>(count));
    }

    return query_cursor;
}

cursor collection::find(view_or_value filter, options::find const& options) {
    return _find(nullptr, std::move(filter), options);
}

cursor collection::find(client_session const& session, view_or_value filter, options::find const& options) {
    return _find(&session, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>
collection::_find_one(client_session const* session, view_or_value filter, options::find const& options) {
    options::find copy(options);
    copy.limit(1);
    cursor cursor = session ? find(*session, std::move(filter), copy) : find(std::move(filter), copy);
    cursor::iterator it = cursor.begin();
    if (it == cursor.end()) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }
    return bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>(bsoncxx::v_noabi::document::value{*it});
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one(
    view_or_value filter,
    options::find const& options) {
    return _find_one(nullptr, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>
collection::find_one(client_session const& session, view_or_value filter, options::find const& options) {
    return _find_one(&session, std::move(filter), options);
}

cursor
collection::_aggregate(client_session const* session, pipeline const& pipeline, options::aggregate const& options) {
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

    return cursor(libmongoc::collection_aggregate(
        _get_impl().collection_t, static_cast<::mongoc_query_flags_t>(0), stages.bson(), options_bson.bson(), rp_ptr));
}

cursor collection::aggregate(pipeline const& pipeline, options::aggregate const& options) {
    return _aggregate(nullptr, pipeline, options);
}

cursor
collection::aggregate(client_session const& session, pipeline const& pipeline, options::aggregate const& options) {
    return _aggregate(&session, pipeline, options);
}

bsoncxx::v_noabi::stdx::optional<result::insert_one>
collection::_insert_one(client_session const* session, view_or_value document, options::insert const& options) {
    // TODO: We should consider making it possible to convert from an options::insert into
    // an options::bulk_write at the type level, removing the need to re-iterate this code
    // many times here and below.
    //
    // See comments in: https://github.com/mongodb/mongo-cxx-driver/pull/409

    options::bulk_write bulk_opts;

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& bdv = options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*bdv);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    mongocxx::v_noabi::bulk_write bulk_op{*this, bulk_opts, session};
    bsoncxx::v_noabi::document::element oid{};
    bsoncxx::v_noabi::builder::basic::document new_document;

    if (!document.view()["_id"]) {
        new_document.append(kvp("_id", bsoncxx::v_noabi::oid()));
        new_document.append(concatenate(document));
        bulk_op.append(model::insert_one(new_document.view()));
        oid = new_document.view()["_id"];
    } else {
        bulk_op.append(model::insert_one(document));
        oid = document.view()["_id"];
    }

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::insert_one>(
        result::insert_one(std::move(result.value()), oid.get_value()));
}

bsoncxx::v_noabi::stdx::optional<result::insert_one> collection::insert_one(
    view_or_value document,
    options::insert const& options) {
    return _insert_one(nullptr, document, options);
}

bsoncxx::v_noabi::stdx::optional<result::insert_one>
collection::insert_one(client_session const& session, view_or_value document, options::insert const& options) {
    return _insert_one(&session, document, options);
}

bsoncxx::v_noabi::stdx::optional<result::replace_one> collection::_replace_one(
    client_session const* session,
    options::bulk_write const& bulk_opts,
    model::replace_one const& replace_op) {
    auto bulk_op = session ? create_bulk_write(*session, bulk_opts) : create_bulk_write(bulk_opts);
    bulk_op.append(replace_op);

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::replace_one>(result::replace_one(std::move(result.value())));
}

bsoncxx::v_noabi::stdx::optional<result::replace_one> collection::_replace_one(
    client_session const* session,
    view_or_value filter,
    view_or_value replacement,
    options::replace const& options) {
    options::bulk_write bulk_opts;

    if (auto const& bdv = options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*bdv);
    }

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& let = options.let()) {
        bulk_opts.let(*let);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    model::replace_one replace_op(std::move(filter), std::move(replacement));

    if (auto const& collation = options.collation()) {
        replace_op.collation(*collation);
    }

    if (auto const& hint = options.hint()) {
        replace_op.hint(*hint);
    }

    if (auto const& sort = options.sort()) {
        replace_op.sort(*sort);
    }

    if (auto const& upsert = options.upsert()) {
        replace_op.upsert(*upsert);
    }

    return _replace_one(session, bulk_opts, replace_op);
}

bsoncxx::v_noabi::stdx::optional<result::replace_one>
collection::replace_one(view_or_value filter, view_or_value replacement, options::replace const& options) {
    return _replace_one(nullptr, std::move(filter), replacement, options);
}

bsoncxx::v_noabi::stdx::optional<result::replace_one> collection::replace_one(
    client_session const& session,
    view_or_value filter,
    view_or_value replacement,
    options::replace const& options) {
    return _replace_one(&session, std::move(filter), replacement, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::_update_many(
    client_session const* session,
    view_or_value filter,
    view_or_value update,
    options::update const& options) {
    options::bulk_write bulk_opts;

    if (auto const& bdv = options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*bdv);
    }

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& let = options.let()) {
        bulk_opts.let(*let);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    auto bulk_op = session ? create_bulk_write(*session, bulk_opts) : create_bulk_write(bulk_opts);

    model::update_many update_op(std::move(filter), std::move(update));

    if (auto const& collation = options.collation()) {
        update_op.collation(*collation);
    }

    if (auto const& hint = options.hint()) {
        update_op.hint(*hint);
    }

    if (auto const& upsert = options.upsert()) {
        update_op.upsert(*upsert);
    }

    if (auto const& array_filters = options.array_filters()) {
        update_op.array_filters(*array_filters);
    }

    bulk_op.append(update_op);

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::update>(result::update(std::move(result.value())));
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_many(view_or_value filter, view_or_value update, options::update const& options) {
    return _update_many(nullptr, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_many(view_or_value filter, pipeline const& update, options::update const& options) {
    return _update_many(nullptr, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_many(view_or_value filter, std::initializer_list<_empty_doc_tag>, options::update const& options) {
    return _update_many(nullptr, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_many(
    client_session const& session,
    view_or_value filter,
    view_or_value update,
    options::update const& options) {
    return _update_many(&session, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_many(
    client_session const& session,
    view_or_value filter,
    pipeline const& update,
    options::update const& options) {
    return _update_many(&session, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_many(
    client_session const& session,
    view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    options::update const& options) {
    return _update_many(&session, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::_update_one(
    client_session const* session,
    view_or_value filter,
    view_or_value update,
    options::update const& options) {
    options::bulk_write bulk_opts;

    if (auto const& bdv = options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*bdv);
    }

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& let = options.let()) {
        bulk_opts.let(*let);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    auto bulk_op = session ? create_bulk_write(*session, bulk_opts) : create_bulk_write(bulk_opts);

    model::update_one update_op(std::move(filter), std::move(update));

    if (auto const& collation = options.collation()) {
        update_op.collation(*collation);
    }

    if (auto const& hint = options.hint()) {
        update_op.hint(*hint);
    }

    if (auto const& sort = options.sort()) {
        update_op.sort(*sort);
    }

    if (auto const& upsert = options.upsert()) {
        update_op.upsert(*upsert);
    }

    if (auto const& array_filters = options.array_filters()) {
        update_op.array_filters(*array_filters);
    }

    bulk_op.append(update_op);

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::update>(result::update(std::move(result.value())));
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_one(view_or_value filter, view_or_value update, options::update const& options) {
    return _update_one(nullptr, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_one(view_or_value filter, pipeline const& update, options::update const& options) {
    return _update_one(nullptr, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<result::update>
collection::update_one(view_or_value filter, std::initializer_list<_empty_doc_tag>, options::update const& options) {
    return _update_one(nullptr, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_one(
    client_session const& session,
    view_or_value filter,
    view_or_value update,
    options::update const& options) {
    return _update_one(&session, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_one(
    client_session const& session,
    view_or_value filter,
    pipeline const& update,
    options::update const& options) {
    return _update_one(&session, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<result::update> collection::update_one(
    client_session const& session,
    view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    options::update const& options) {
    return _update_one(&session, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<result::delete_result>
collection::_delete_many(client_session const* session, view_or_value filter, options::delete_options const& options) {
    options::bulk_write bulk_opts;

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& let = options.let()) {
        bulk_opts.let(*let);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    auto bulk_op = session ? create_bulk_write(*session, bulk_opts) : create_bulk_write(bulk_opts);

    model::delete_many delete_op(filter);

    if (auto const& collation = options.collation()) {
        delete_op.collation(*collation);
    }

    if (auto const& hint = options.hint()) {
        delete_op.hint(*hint);
    }

    bulk_op.append(delete_op);

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

bsoncxx::v_noabi::stdx::optional<result::delete_result> collection::delete_many(
    view_or_value filter,
    options::delete_options const& options) {
    return _delete_many(nullptr, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<result::delete_result>
collection::delete_many(client_session const& session, view_or_value filter, options::delete_options const& options) {
    return _delete_many(&session, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<result::delete_result>
collection::_delete_one(client_session const* session, view_or_value filter, options::delete_options const& options) {
    options::bulk_write bulk_opts;

    if (auto const& wc = options.write_concern()) {
        bulk_opts.write_concern(*wc);
    }

    if (auto const& let = options.let()) {
        bulk_opts.let(*let);
    }

    if (auto const& comment = options.comment()) {
        bulk_opts.comment(*comment);
    }

    auto bulk_op = session ? create_bulk_write(*session, bulk_opts) : create_bulk_write(bulk_opts);

    model::delete_one delete_op(filter);

    if (auto const collation = options.collation()) {
        delete_op.collation(*collation);
    }

    if (auto const hint = options.hint()) {
        delete_op.hint(*hint);
    }

    bulk_op.append(delete_op);

    auto result = bulk_op.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return bsoncxx::v_noabi::stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

bsoncxx::v_noabi::stdx::optional<result::delete_result> collection::delete_one(
    view_or_value filter,
    options::delete_options const& options) {
    return _delete_one(nullptr, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<result::delete_result>
collection::delete_one(client_session const& session, view_or_value filter, options::delete_options const& options) {
    return _delete_one(&session, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::_find_one_and_replace(
    client_session const* session,
    view_or_value filter,
    view_or_value replacement,
    options::find_one_and_replace const& options) {
    mongoc_find_and_modify_flags_t flags = MONGOC_FIND_AND_MODIFY_NONE;
    if (options.upsert().value_or(false)) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == options::return_document::k_after) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_RETURN_NEW);
    }

    return find_and_modify(
        _get_impl().collection_t,
        session ? session->_get_impl().get_session_t() : nullptr,
        filter,
        &replacement,
        flags,
        options.bypass_document_validation().value_or(false),
        bsoncxx::v_noabi::stdx::nullopt,
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_replace(
    view_or_value filter,
    view_or_value replacement,
    options::find_one_and_replace const& options) {
    return _find_one_and_replace(nullptr, std::move(filter), replacement, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_replace(
    client_session const& session,
    view_or_value filter,
    view_or_value replacement,
    options::find_one_and_replace const& options) {
    return _find_one_and_replace(&session, std::move(filter), replacement, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::_find_one_and_update(
    client_session const* session,
    view_or_value filter,
    view_or_value update,
    options::find_one_and_update const& options) {
    mongoc_find_and_modify_flags_t flags = MONGOC_FIND_AND_MODIFY_NONE;
    if (options.upsert().value_or(false)) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == options::return_document::k_after) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_RETURN_NEW);
    }

    return find_and_modify(
        _get_impl().collection_t,
        session ? session->_get_impl().get_session_t() : nullptr,
        filter,
        &update,
        flags,
        options.bypass_document_validation().value_or(false),
        options.array_filters(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    view_or_value filter,
    view_or_value update,
    options::find_one_and_update const& options) {
    return _find_one_and_update(nullptr, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    view_or_value filter,
    pipeline const& update,
    options::find_one_and_update const& options) {
    return _find_one_and_update(
        nullptr, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    options::find_one_and_update const& options) {
    return _find_one_and_update(nullptr, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    client_session const& session,
    view_or_value filter,
    view_or_value update,
    options::find_one_and_update const& options) {
    return _find_one_and_update(&session, std::move(filter), update, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    client_session const& session,
    view_or_value filter,
    pipeline const& update,
    options::find_one_and_update const& options) {
    return _find_one_and_update(
        &session, std::move(filter), bsoncxx::v_noabi::document::view(update.view_array()), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    client_session const& session,
    view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    options::find_one_and_update const& options) {
    return _find_one_and_update(&session, std::move(filter), bsoncxx::v_noabi::document::view{}, options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::_find_one_and_delete(
    client_session const* session,
    view_or_value filter,
    options::find_one_and_delete const& options) {
    return find_and_modify(
        _get_impl().collection_t,
        session ? session->_get_impl().get_session_t() : nullptr,
        filter,
        nullptr,
        MONGOC_FIND_AND_MODIFY_REMOVE,
        false,
        bsoncxx::v_noabi::stdx::nullopt,
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_delete(
    view_or_value filter,
    options::find_one_and_delete const& options) {
    return _find_one_and_delete(nullptr, std::move(filter), options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_delete(
    client_session const& session,
    view_or_value filter,
    options::find_one_and_delete const& options) {
    return _find_one_and_delete(&session, std::move(filter), options);
}

std::int64_t
collection::_count_documents(client_session const* session, view_or_value filter, options::count const& options) {
    scoped_bson_t bson_filter{filter};
    scoped_bson_t reply;
    bson_error_t error;
    mongoc_read_prefs_t const* read_prefs = nullptr;

    if (auto const& rp = options.read_preference()) {
        read_prefs = rp->_impl->read_preference_t;
    }

    bsoncxx::v_noabi::builder::basic::document opts_builder;

    if (auto const& collation = options.collation()) {
        opts_builder.append(kvp("collation", *collation));
    }

    if (auto const& max_time = options.max_time()) {
        opts_builder.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{max_time->count()}));
    }

    if (auto const& hint = options.hint()) {
        opts_builder.append(kvp("hint", hint->to_value()));
    }

    if (auto const& comment = options.comment()) {
        opts_builder.append(kvp("comment", *comment));
    }

    if (session) {
        opts_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    if (auto const& skip = options.skip()) {
        opts_builder.append(kvp("skip", *skip));
    }

    if (auto const& limit = options.limit()) {
        opts_builder.append(kvp("limit", *limit));
    }

    scoped_bson_t opts_bson{opts_builder.view()};
    auto result = libmongoc::collection_count_documents(
        _get_impl().collection_t, bson_filter.bson(), opts_bson.bson(), read_prefs, reply.bson_for_init(), &error);
    if (result < 0) {
        throw_exception<query_exception>(reply.steal(), error);
    }
    return result;
}

std::int64_t collection::count_documents(view_or_value filter, options::count const& options) {
    return _count_documents(nullptr, std::move(filter), options);
}

std::int64_t
collection::count_documents(client_session const& session, view_or_value filter, options::count const& options) {
    return _count_documents(&session, std::move(filter), options);
}

std::int64_t collection::estimated_document_count(options::estimated_document_count const& options) {
    scoped_bson_t reply;
    bson_error_t error;

    mongoc_read_prefs_t const* read_prefs = nullptr;

    if (auto const& rp = options.read_preference()) {
        read_prefs = rp->_impl->read_preference_t;
    }

    bsoncxx::v_noabi::builder::basic::document opts_builder;

    if (auto const& max_time = options.max_time()) {
        opts_builder.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{max_time->count()}));
    }

    if (auto const& comment = options.comment()) {
        opts_builder.append(kvp("comment", *comment));
    }

    scoped_bson_t opts_bson{opts_builder.view()};
    auto result = libmongoc::collection_estimated_document_count(
        _get_impl().collection_t, opts_bson.bson(), read_prefs, reply.bson_for_init(), &error);
    if (result < 0) {
        throw_exception<query_exception>(reply.steal(), error);
    }
    return result;
}

bsoncxx::v_noabi::document::value collection::_create_index(
    client_session const* session,
    bsoncxx::v_noabi::document::view_or_value keys,
    bsoncxx::v_noabi::document::view_or_value index_opts,
    options::index_view operation_options) {
    using namespace bsoncxx;

    auto name = session ? indexes().create_one(*session, keys, index_opts, operation_options)
                        : indexes().create_one(keys, index_opts, operation_options);

    if (name) {
        return make_document(kvp("name", *name));
    } else {
        return make_document();
    }
}

bsoncxx::v_noabi::document::value collection::create_index(
    bsoncxx::v_noabi::document::view_or_value keys,
    bsoncxx::v_noabi::document::view_or_value index_opts,
    options::index_view operation_options) {
    return _create_index(nullptr, keys, index_opts, operation_options);
}

bsoncxx::v_noabi::document::value collection::create_index(
    client_session const& session,
    bsoncxx::v_noabi::document::view_or_value keys,
    bsoncxx::v_noabi::document::view_or_value index_opts,
    options::index_view operation_options) {
    return _create_index(&session, keys, index_opts, operation_options);
}

cursor collection::_distinct(
    client_session const* session,
    bsoncxx::v_noabi::string::view_or_value field_name,
    view_or_value query,
    options::distinct const& options) {
    //
    // Construct the distinct command and options.
    //
    bsoncxx::v_noabi::builder::basic::document command_builder;
    command_builder.append(
        kvp("distinct", name()),
        kvp("key", field_name.view()),
        kvp("query", bsoncxx::v_noabi::types::b_document{query}));

    if (auto const& max_time = options.max_time()) {
        command_builder.append(kvp("maxTimeMS", bsoncxx::v_noabi::types::b_int64{max_time->count()}));
    }

    bsoncxx::v_noabi::builder::basic::document opts_builder{};
    if (auto const& collation = options.collation()) {
        opts_builder.append(kvp("collation", *collation));
    }

    if (auto const& comment = options.comment()) {
        opts_builder.append(kvp("comment", *comment));
    }

    if (session) {
        opts_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    mongoc_read_prefs_t const* rp_ptr = nullptr;
    if (auto const& rp = options.read_preference()) {
        rp_ptr = rp->_impl->read_preference_t;
    }

    //
    // Send the command and validate the reply.
    //
    scoped_bson_t reply;
    bson_error_t error;
    scoped_bson_t command_bson{command_builder.extract()};
    scoped_bson_t opts_bson{opts_builder.extract()};

    auto result = libmongoc::collection_read_command_with_opts(
        _get_impl().collection_t, command_bson.bson(), rp_ptr, opts_bson.bson(), reply.bson_for_init(), &error);

    if (!result) {
        throw_exception<operation_exception>(reply.steal(), error);
    }

    //
    // Fake a cursor with the reply document as a single result.
    //
    auto fake_db_reply =
        make_document(kvp("ok", 1), kvp("cursor", [&reply](sub_document sub_doc) {
                          sub_doc.append(kvp("ns", ""), kvp("id", 0), kvp("firstBatch", [&reply](sub_array sub_arr) {
                                             sub_arr.append(reply.view());
                                         }));
                      }));

    bson_t* reply_bson = bson_new_from_data(fake_db_reply.view().data(), fake_db_reply.view().length());
    if (!reply_bson) {
        throw bsoncxx::v_noabi::exception{bsoncxx::v_noabi::error_code::k_internal_error};
    }

    bson_t const* error_document;

    cursor fake_cursor{
        libmongoc::cursor_new_from_command_reply_with_opts(_get_impl().client_impl->client_t, reply_bson, nullptr)};
    if (libmongoc::cursor_error_document(fake_cursor._impl->cursor_t, &error, &error_document)) {
        if (error_document) {
            bsoncxx::v_noabi::document::value error_doc{
                bsoncxx::v_noabi::document::view{bson_get_data(error_document), error_document->len}};
            throw_exception<query_exception>(error_doc, error);
        } else {
            throw_exception<query_exception>(error);
        }
    }

    return fake_cursor;
}

cursor collection::distinct(
    bsoncxx::v_noabi::string::view_or_value field_name,
    view_or_value query,
    options::distinct const& options) {
    return _distinct(nullptr, field_name, query, options);
}

cursor collection::distinct(
    client_session const& session,
    bsoncxx::v_noabi::string::view_or_value field_name,
    view_or_value query,
    options::distinct const& options) {
    return _distinct(&session, field_name, query, options);
}

cursor collection::list_indexes() const {
    return libmongoc::collection_find_indexes_with_opts(_get_impl().collection_t, nullptr);
}

cursor collection::list_indexes(client_session const& session) const {
    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session._get_impl().to_document()});
    libbson::scoped_bson_t options_bson{options_builder.extract()};
    return libmongoc::collection_find_indexes_with_opts(_get_impl().collection_t, options_bson.bson());
}

void collection::_drop(
    client_session const* session,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc,
    bsoncxx::v_noabi::document::view_or_value collection_options) {
    bson_error_t error;

    bsoncxx::v_noabi::builder::basic::document opts_doc;
    if (wc) {
        opts_doc.append(kvp("writeConcern", wc->to_document()));
    }

    if (session) {
        opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    if (!collection_options.view().empty()) {
        opts_doc.append(bsoncxx::v_noabi::builder::concatenate_doc{collection_options});
    }

    scoped_bson_t opts_bson{opts_doc.view()};
    auto result = libmongoc::collection_drop_with_opts(_get_impl().collection_t, opts_bson.bson(), &error);

    // Throw an exception if the command failed, unless the failure was due to a non-existent
    // collection. We check for this failure using 'code', but we fall back to checking 'message'
    // for old server versions (3.0 and earlier) that do not send a code with the command response.
    if (!result &&
        !(error.code == ::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST ||
          bsoncxx::v_noabi::stdx::string_view{error.message} == bsoncxx::v_noabi::stdx::string_view{"ns not found"})) {
        throw_exception<operation_exception>(error);
    }
}

void collection::drop(
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc,
    bsoncxx::v_noabi::document::view_or_value collection_options) {
    return _drop(nullptr, wc, collection_options);
}

void collection::drop(
    client_session const& session,
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& wc,
    bsoncxx::v_noabi::document::view_or_value collection_options) {
    return _drop(&session, wc, collection_options);
}

void collection::read_concern(mongocxx::v_noabi::read_concern rc) {
    libmongoc::collection_set_read_concern(_get_impl().collection_t, rc._impl->read_concern_t);
}

mongocxx::v_noabi::read_concern collection::read_concern() const {
    auto rc = libmongoc::collection_get_read_concern(_get_impl().collection_t);
    return {bsoncxx::make_unique<read_concern::impl>(libmongoc::read_concern_copy(rc))};
}

void collection::read_preference(mongocxx::v_noabi::read_preference rp) {
    libmongoc::collection_set_read_prefs(_get_impl().collection_t, rp._impl->read_preference_t);
}

mongocxx::v_noabi::read_preference collection::read_preference() const {
    mongocxx::v_noabi::read_preference rp(bsoncxx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::collection_get_read_prefs(_get_impl().collection_t))));
    return rp;
}

void collection::write_concern(mongocxx::v_noabi::write_concern wc) {
    libmongoc::collection_set_write_concern(_get_impl().collection_t, wc._impl->write_concern_t);
}

mongocxx::v_noabi::write_concern collection::write_concern() const {
    mongocxx::v_noabi::write_concern wc(bsoncxx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::collection_get_write_concern(_get_impl().collection_t))));
    return wc;
}

change_stream collection::watch(options::change_stream const& options) {
    return watch(pipeline{}, options);
}

change_stream collection::watch(client_session const& session, options::change_stream const& options) {
    return _watch(&session, pipeline{}, options);
}

change_stream collection::watch(pipeline const& pipe, options::change_stream const& options) {
    return _watch(nullptr, pipe, options);
}

change_stream
collection::watch(client_session const& session, pipeline const& pipe, options::change_stream const& options) {
    return _watch(&session, pipe, options);
}

change_stream
collection::_watch(client_session const* session, pipeline const& pipe, options::change_stream const& options) {
    bsoncxx::v_noabi::builder::basic::document container;
    container.append(kvp("pipeline", pipe._impl->view_array()));
    scoped_bson_t pipeline_bson{container.view()};

    bsoncxx::v_noabi::builder::basic::document options_builder;
    options_builder.append(bsoncxx::v_noabi::builder::concatenate(options.as_bson()));
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t options_bson{options_builder.extract()};

    // NOTE: collection_watch copies what it needs so we're safe to destroy our copies.
    return change_stream{
        libmongoc::collection_watch(_get_impl().collection_t, pipeline_bson.bson(), options_bson.bson())};
}

index_view collection::indexes() {
    return index_view{_get_impl().collection_t, _get_impl().client_impl->client_t};
}

search_index_view collection::search_indexes() {
    return search_index_view{_get_impl().collection_t, _get_impl().client_impl->client_t};
}

mongocxx::v_noabi::bulk_write collection::_init_insert_many(
    options::insert const& options,
    client_session const* session) {
    options::bulk_write bulk_write_options;

    bulk_write_options.ordered(options.ordered().value_or(true));

    if (auto const& wc = options.write_concern()) {
        bulk_write_options.write_concern(*wc);
    }

    if (auto const& bdv = options.bypass_document_validation()) {
        bulk_write_options.bypass_document_validation(*bdv);
    }

    if (auto const& comment = options.comment()) {
        bulk_write_options.comment(*comment);
    }

    if (session) {
        return create_bulk_write(*session, bulk_write_options);
    }

    return create_bulk_write(bulk_write_options);
}

void collection::_insert_many_doc_handler(
    mongocxx::v_noabi::bulk_write& writes,
    bsoncxx::v_noabi::builder::basic::array& inserted_ids,
    bsoncxx::v_noabi::document::view doc) const {
    bsoncxx::v_noabi::builder::basic::document id_doc;

    if (!doc["_id"]) {
        id_doc.append(kvp("_id", bsoncxx::v_noabi::oid{}));
        writes.append(model::insert_one{make_document(concatenate(id_doc.view()), concatenate(doc))});
    } else {
        id_doc.append(kvp("_id", doc["_id"].get_value()));
        writes.append(model::insert_one{doc});
    }

    inserted_ids.append(id_doc.view());
}

bsoncxx::v_noabi::stdx::optional<result::insert_many> collection::_exec_insert_many(
    mongocxx::v_noabi::bulk_write& writes,
    bsoncxx::v_noabi::builder::basic::array& inserted_ids) {
    auto result = writes.execute();
    if (!result) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return result::insert_many{std::move(result.value()), inserted_ids.extract()};
}

collection::impl const& collection::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_collection_object};
    }
    return *_impl;
}

collection::impl& collection::_get_impl() {
    auto cthis = const_cast<collection const*>(this);
    return const_cast<collection::impl&>(cthis->_get_impl());
}

} // namespace v_noabi
} // namespace mongocxx
