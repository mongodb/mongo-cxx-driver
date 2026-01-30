// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and limitations under the
// License.

#include <mongocxx/v1/collection.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/indexes.hpp>
#include <mongocxx/v1/pipeline.hpp>
#include <mongocxx/v1/return_document.hpp>
#include <mongocxx/v1/search_indexes.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/aggregate_options.hh>
#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/count_options.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/delete_many_options.hh>
#include <mongocxx/v1/delete_many_result.hh>
#include <mongocxx/v1/delete_one_options.hh>
#include <mongocxx/v1/delete_one_result.hh>
#include <mongocxx/v1/distinct_options.hh>
#include <mongocxx/v1/estimated_document_count_options.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/find_one_and_delete_options.hh>
#include <mongocxx/v1/find_one_and_replace_options.hh>
#include <mongocxx/v1/find_one_and_update_options.hh>
#include <mongocxx/v1/find_options.hh>
#include <mongocxx/v1/hint.hh>
#include <mongocxx/v1/insert_many_options.hh>
#include <mongocxx/v1/insert_many_result.hh>
#include <mongocxx/v1/insert_one_options.hh>
#include <mongocxx/v1/insert_one_result.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/replace_one_options.hh>
#include <mongocxx/v1/replace_one_result.hh>
#include <mongocxx/v1/update_many_options.hh>
#include <mongocxx/v1/update_many_result.hh>
#include <mongocxx/v1/update_one_options.hh>
#include <mongocxx/v1/update_one_result.hh>
#include <mongocxx/v1/write_concern.hh>

#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

using code = v1::collection::errc;

class collection::impl {
   public:
    mongoc_collection_t* _coll;
    mongoc_client_t* _client;

    ~impl() {
        libmongoc::collection_destroy(_coll);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;

    impl(impl const& other) : _coll{libmongoc::collection_copy(other._coll)}, _client{other._client} {}

    impl& operator=(impl const& other) = delete;

    impl(mongoc_collection_t* db, mongoc_client_t* client) : _coll{db}, _client{client} {}

    static impl const& with(collection const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(collection const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(collection& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(collection* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

collection::~collection() {
    delete impl::with(_impl);
}

collection::collection(collection&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

collection& collection::operator=(collection&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

collection::collection(collection const& other) : _impl{new impl{impl::with(other)}} {}

collection& collection::operator=(collection const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

collection::collection() : _impl{nullptr} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

collection::operator bool() const {
    return _impl != nullptr;
}

namespace {

template <typename Options>
mongoc_read_prefs_t const* get_read_prefs(Options const& options) {
    auto const& rp_opt = Options::internal::read_preference(options);
    return rp_opt ? v1::read_preference::internal::as_mongoc(*rp_opt) : nullptr;
}

void append_comment(bsoncxx::v1::types::value const& opt, scoped_bson& doc) {
    scoped_bson v;

    if (!BSON_APPEND_VALUE(v.inout_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(opt))) {
        throw std::logic_error{"mongocxx::v1::append_comment: BSON_APPEND_VALUE failed"};
    }

    doc += v;
}

void append_hint(v1::hint const& opt, scoped_bson& doc) {
    scoped_bson v;

    if (!BSON_APPEND_VALUE(
            v.inout_ptr(),
            "hint",
            &bsoncxx::v1::types::value::internal::get_bson_value(bsoncxx::v1::types::value{opt.to_value()}))) {
        throw std::logic_error{"mongocxx::v1::append_hint: BSON_APPEND_VALUE failed"};
    }

    doc += v;
}

void append_to(v1::bulk_write::options const& opts, scoped_bson& doc) {
    if (!opts.ordered()) {
        // ordered is true by default. Only append it if set to false.
        doc += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    if (auto const& opt = v1::bulk_write::options::internal::write_concern(opts)) {
        auto const v = opt->to_document();
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson_view{v}.bson()))};
    }

    if (auto const& opt = v1::bulk_write::options::internal::let(opts)) {
        auto const v = opt->view();
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{v}.bson()))};
    }

    if (auto const& opt = v1::bulk_write::options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::count_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::count_options::internal::collation(opts)) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = v1::count_options::internal::hint(opts)) {
        append_hint(*opt, doc);
    }

    if (auto const& opt = v1::count_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }

    if (auto const& opt = opts.skip()) {
        doc += scoped_bson{BCON_NEW("skip", BCON_INT64(*opt))};
    }

    if (auto const& opt = opts.limit()) {
        doc += scoped_bson{BCON_NEW("limit", BCON_INT64(*opt))};
    }
}

void append_to(v1::estimated_document_count_options const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson(BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()})));
    }

    if (auto const& opt = v1::estimated_document_count_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::delete_many_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::delete_many_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::delete_many_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::delete_many_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::delete_one_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::delete_one_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::delete_one_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::delete_one_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::distinct_options const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = v1::distinct_options::internal::collation(opts)) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::distinct_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::find_options const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.allow_disk_use()) {
        doc += scoped_bson{BCON_NEW("allowDiskUse", BCON_BOOL(*opt))};
    }

    if (auto const& opt = opts.allow_partial_results()) {
        doc += scoped_bson{BCON_NEW("allowPartialResults", BCON_BOOL(*opt))};
    }

    if (auto const opt = opts.batch_size()) {
        doc += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*opt))};
    }

    if (auto const& opt = v1::find_options::internal::collation(opts)) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::find_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }

    if (auto const& opt = opts.cursor_type()) {
        switch (*opt) {
            case cursor::type::k_non_tailable: {
                // Do nothing.
            } break;

            case cursor::type::k_tailable: {
                doc += scoped_bson{BCON_NEW("tailable", BCON_BOOL(true))};
            } break;

            case cursor::type::k_tailable_await: {
                doc += scoped_bson{BCON_NEW("tailable", BCON_BOOL(true), "awaitData", BCON_BOOL(true))};
            } break;

            default:
                MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }

    if (auto const& opt = v1::find_options::internal::hint(opts)) {
        append_hint(*opt, doc);
    }

    if (auto const& opt = v1::find_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = opts.limit()) {
        doc += scoped_bson{BCON_NEW("limit", BCON_INT64(*opt))};
    }

    if (auto const& opt = v1::find_options::internal::max(opts)) {
        doc += scoped_bson{BCON_NEW("max", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = opts.max_await_time()) {
        doc += scoped_bson{BCON_NEW("maxAwaitTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = v1::find_options::internal::min(opts)) {
        doc += scoped_bson{BCON_NEW("min", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = opts.no_cursor_timeout()) {
        doc += scoped_bson{BCON_NEW("noCursorTimeout", BCON_BOOL(*opt))};
    }

    if (auto const& opt = v1::find_options::internal::projection(opts)) {
        doc += scoped_bson{BCON_NEW("projection", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = opts.return_key()) {
        doc += scoped_bson{BCON_NEW("returnKey", BCON_BOOL(*opt))};
    }

    if (auto const& opt = opts.show_record_id()) {
        doc += scoped_bson{BCON_NEW("showRecordId", BCON_BOOL(*opt))};
    }

    if (auto const& opt = opts.skip()) {
        doc += scoped_bson{BCON_NEW("skip", BCON_INT64(*opt))};
    }

    if (auto const& opt = v1::find_options::internal::sort(opts)) {
        doc += scoped_bson{BCON_NEW("sort", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }
}

void append_to(v1::insert_many_options const& opts, scoped_bson& doc) {
    // Only include "ordered" when `false` (not the default behavior).
    if (opts.ordered().value_or(true) == false) {
        doc += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    if (auto const& opt = v1::insert_many_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::insert_many_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::insert_one_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::insert_one_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::insert_one_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::replace_one_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::replace_one_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::replace_one_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::replace_one_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::update_many_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::update_many_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::update_many_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::update_many_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

void append_to(v1::update_one_options const& opts, scoped_bson& doc) {
    if (auto const& opt = v1::update_one_options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = v1::update_one_options::internal::let(opts)) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::update_one_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

v1::cursor aggregate_impl(
    mongoc_collection_t* coll,
    bson_t const* pipeline,
    bson_t const* opts,
    mongoc_read_prefs_t const* read_prefs) {
    return v1::cursor::internal::make(
        libmongoc::collection_aggregate(coll, MONGOC_QUERY_NONE, pipeline, opts, read_prefs));
}

v1::bulk_write
create_bulk_write_impl(mongoc_collection_t* coll, bson_t const* opts, bsoncxx::v1::stdx::optional<bool> bdv_opt) {
    auto ret = v1::bulk_write::internal::make(libmongoc::collection_create_bulk_operation_with_opts(coll, opts));

    // CDRIVER-4304: mongoc_bulk_opts_t doesn't support "bypassDocumentValidation" yet.
    if (bdv_opt) {
        libmongoc::bulk_operation_set_bypass_document_validation(v1::bulk_write::internal::as_mongoc(ret), *bdv_opt);
    }

    return ret;
}

std::int64_t count_documents_impl(
    mongoc_collection_t* coll,
    bson_t const* filter,
    bson_t const* opts,
    mongoc_read_prefs_t const* read_prefs) {
    bson_error_t error = {};

    scoped_bson reply;
    auto const ret = libmongoc::collection_count_documents(coll, filter, opts, read_prefs, reply.out_ptr(), &error);

    if (ret < 0) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return ret;
}

std::int64_t
estimated_document_count_impl(mongoc_collection_t* coll, bson_t const* opts, mongoc_read_prefs_t const* read_prefs) {
    bson_error_t error = {};

    scoped_bson reply;
    auto const ret = libmongoc::collection_estimated_document_count(coll, opts, read_prefs, reply.out_ptr(), &error);

    if (ret < 0) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return ret;
}

bsoncxx::v1::stdx::optional<v1::delete_many_result>
delete_many_impl(v1::bulk_write bulk, bsoncxx::v1::document::view q, v1::delete_many_options const& delete_opts) {
    v1::bulk_write::delete_many op{bsoncxx::v1::document::value{q}};

    if (auto const& opt = v1::delete_many_options::internal::collation(delete_opts)) {
        op.collation(*opt);
    }

    if (auto const& opt = v1::delete_many_options::internal::hint(delete_opts)) {
        op.hint(*opt);
    }

    if (auto ret = bulk.append(op).execute()) {
        return v1::delete_many_result::internal::make(std::move(*ret));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::delete_one_result>
delete_one_impl(v1::bulk_write bulk, bsoncxx::v1::document::view q, v1::delete_one_options const& delete_opts) {
    v1::bulk_write::delete_one op{bsoncxx::v1::document::value{q}};

    if (auto const& opt = v1::delete_one_options::internal::collation(delete_opts)) {
        op.collation(*opt);
    }

    if (auto const& opt = v1::delete_one_options::internal::hint(delete_opts)) {
        op.hint(*opt);
    }

    if (auto ret = bulk.append(op).execute()) {
        return v1::delete_one_result::internal::make(std::move(*ret));
    }

    return {};
}

v1::cursor distinct_impl(
    mongoc_collection_t* coll,
    mongoc_client_t* client,
    bsoncxx::v1::stdx::string_view key,
    bson_t const* query,
    bson_t const* opts,
    mongoc_read_prefs_t const* read_prefs) {
    scoped_bson const command{BCON_NEW(
        "distinct",
        libmongoc::collection_get_name(coll),
        "key",
        BCON_UTF8(std::string{key}.c_str()),
        "query",
        BCON_DOCUMENT(scoped_bson_view{query}))};

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_read_command_with_opts(
            coll, scoped_bson_view{command}.bson(), read_prefs, scoped_bson_view{opts}, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    // {
    //   "ok": 1,
    //   "cursor": {
    //     "ns": "",
    //     "id": 0,
    //     "firstBatch": [ <reply> ]
    // }
    scoped_bson fake_reply{BCON_NEW(
        "ok",
        BCON_INT32(1),
        "cursor",
        "{",
        "ns",
        "",
        "id",
        BCON_INT32(0),
        "firstBatch",
        "[",
        BCON_DOCUMENT(reply.bson()),
        "]",
        "}")};

    auto ret = v1::cursor::internal::make(
        libmongoc::cursor_new_from_command_reply_with_opts(client, fake_reply.inout_ptr(), nullptr));

    bson_t const* error_document = nullptr;
    if (libmongoc::cursor_error_document(v1::cursor::internal::as_mongoc(ret), &error, &error_document)) {
        if (error_document) {
            v1::throw_exception(error, scoped_bson_view{error_document}.view());
        } else {
            v1::throw_exception(error);
        }
    }

    return ret;
}

void drop_impl(mongoc_collection_t* coll, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::collection_drop_with_opts(coll, opts, &error)) {
        // Only throw when the error is not due to a non-existent collection.
        if (error.code != MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST) {
            v1::throw_exception(error);
        }
    }
}

v1::cursor
find_impl(mongoc_collection_t* coll, bson_t const* filter, bson_t const* opts, v1::find_options const& find_opts) {
    return v1::cursor::internal::make(
        libmongoc::collection_find_with_opts(coll, filter, opts, get_read_prefs(find_opts)), find_opts.cursor_type());
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> find_one_impl(v1::cursor cursor) {
    // Use a for loop for convenience.
    for (auto const e : cursor) {
        return bsoncxx::v1::document::value{e};
    }

    return {};
}

template <typename Options>
bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> find_and_modify_impl(
    mongoc_collection_t* coll,
    v1::client_session const* session_ptr,
    bson_t const* filter,
    bson_t const* update,
    mongoc_find_and_modify_flags_t flags,
    bool bypass,
    bson_t const* array_filters,
    Options const& options) {
    struct deleter_type {
        void operator()(mongoc_find_and_modify_opts_t* ptr) const noexcept {
            libmongoc::find_and_modify_opts_destroy(ptr);
        }
    };

    using opts_ptr_type = std::unique_ptr<mongoc_find_and_modify_opts_t, deleter_type>;

    auto const opts_owner = opts_ptr_type{libmongoc::find_and_modify_opts_new()};
    auto const opts = opts_owner.get();

    // Some options must be passed via an `extra` BSON document.
    {
        scoped_bson extra;

        if (auto const& opt = Options::internal::write_concern(options)) {
            extra += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
        }

        if (session_ptr) {
            v1::client_session::internal::append_to(*session_ptr, extra);
        }

        if (auto const& opt = Options::internal::collation(options)) {
            extra += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
        }

        if (array_filters) {
            extra += scoped_bson{BCON_NEW("arrayFilters", BCON_ARRAY(array_filters))};
        }

        if (auto const& opt = Options::internal::hint(options)) {
            append_hint(*opt, extra);
        }

        if (auto const& opt = Options::internal::let(options)) {
            extra += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
        }

        if (auto const& opt = Options::internal::comment(options)) {
            append_comment(*opt, extra);
        }

        libmongoc::find_and_modify_opts_append(opts, extra.bson());
    }

    // Some options must be passed via explicit setters.
    {
        if (update) {
            libmongoc::find_and_modify_opts_set_update(opts, update);
        }

        if (bypass) {
            libmongoc::find_and_modify_opts_set_bypass_document_validation(opts, true);
        }

        if (auto const& opt = Options::internal::sort(options)) {
            libmongoc::find_and_modify_opts_set_sort(opts, scoped_bson_view{*opt}.bson());
        }

        if (auto const& opt = Options::internal::projection(options)) {
            libmongoc::find_and_modify_opts_set_fields(opts, scoped_bson_view{*opt}.bson());
        }

        if (auto const& opt = options.max_time()) {
            auto const count = opt->count();

            if (count < 0 || count > std::numeric_limits<std::uint32_t>::max()) {
                throw v1::exception::internal::make(code::max_time_u32);
            }

            libmongoc::find_and_modify_opts_set_max_time_ms(opts, static_cast<std::uint32_t>(count));
        }

        libmongoc::find_and_modify_opts_set_flags(opts, flags);
    }

    scoped_bson reply;
    bson_error_t error = {};
    if (!libmongoc::collection_find_and_modify_with_opts(coll, filter, opts, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    auto const value = reply.view()["value"];

    if (value && value.type_id() == bsoncxx::v1::types::id::k_document) {
        return bsoncxx::v1::document::value{value.get_document().value};
    }

    return {};
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> find_one_and_delete_impl(
    mongoc_collection_t* coll,
    v1::client_session const* session_ptr,
    bson_t const* query,
    v1::find_one_and_delete_options const& opts) {
    return find_and_modify_impl(coll, session_ptr, query, nullptr, MONGOC_FIND_AND_MODIFY_REMOVE, false, nullptr, opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> find_one_and_replace_impl(
    mongoc_collection_t* coll,
    v1::client_session const* session_ptr,
    bson_t const* query,
    bson_t const* replacement,
    v1::find_one_and_replace_options const& options) {
    auto flags = MONGOC_FIND_AND_MODIFY_NONE;

    if (options.upsert()) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == v1::return_document::k_after) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_RETURN_NEW);
    }

    return find_and_modify_impl(
        coll,
        session_ptr,
        query,
        replacement,
        flags,
        options.bypass_document_validation().value_or(false),
        nullptr,
        options);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> find_one_and_update_impl(
    mongoc_collection_t* coll,
    v1::client_session const* session_ptr,
    bson_t const* query,
    bson_t const* update,
    v1::find_one_and_update_options const& options) {
    auto flags = MONGOC_FIND_AND_MODIFY_NONE;

    if (options.upsert()) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == v1::return_document::k_after) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_RETURN_NEW);
    }

    auto const& array_filters_ptr = [&]() -> bsoncxx::v1::array::value const* {
        if (auto const& opt = v1::find_one_and_update_options::internal::array_filters(options)) {
            return &*opt;
        }

        return nullptr;
    }();

    return find_and_modify_impl(
        coll,
        session_ptr,
        query,
        update,
        flags,
        options.bypass_document_validation().value_or(false),
        array_filters_ptr ? scoped_bson_view{array_filters_ptr->view()}.bson() : nullptr,
        options);
}

bsoncxx::v1::document::value with_id(bsoncxx::v1::document::view doc) {
    if (auto const id = doc["_id"]) {
        return bsoncxx::v1::document::value{doc};
    }

    bsoncxx::v1::oid oid;
    bson_oid_t bson_oid = {};
    std::memcpy(bson_oid.bytes, oid.bytes(), oid.size());

    scoped_bson new_doc{BCON_NEW("_id", BCON_OID(&bson_oid))};
    new_doc += doc;
    return std::move(new_doc).value();
}

bsoncxx::v1::stdx::optional<v1::insert_one_result> insert_one_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view document) {
    auto insert_doc = with_id(document);
    auto id = insert_doc["_id"].type_value();

    if (auto res = bulk.append(v1::bulk_write::insert_one{std::move(insert_doc)}).execute()) {
        return v1::insert_one_result::internal::make(std::move(*res), std::move(id));
    }

    return {};
}

void rename_impl(mongoc_collection_t* coll, char const* new_name, bool drop_target, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::collection_rename_with_opts(coll, nullptr, new_name, drop_target, opts, &error)) {
        v1::throw_exception(error);
    }
}

bsoncxx::v1::stdx::optional<v1::replace_one_result> replace_one_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view replacement,
    v1::replace_one_options const& opts) {
    v1::bulk_write::replace_one op{bsoncxx::v1::document::value{filter}, bsoncxx::v1::document::value{replacement}};

    if (auto const& opt = v1::replace_one_options::internal::collation(opts)) {
        op.collation(*opt);
    }

    if (auto const& opt = v1::replace_one_options::internal::hint(opts)) {
        op.hint(*opt);
    }

    if (auto const& opt = v1::replace_one_options::internal::sort(opts)) {
        op.sort(*opt);
    }

    if (auto const opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::replace_one_result::internal::make(std::move(*res));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_many_result>
update_many_impl(v1::bulk_write bulk, v1::bulk_write::update_many op, v1::update_many_options const& opts) {
    if (auto const& opt = v1::update_many_options::internal::collation(opts)) {
        op.collation(*opt);
    }

    if (auto const& opt = v1::update_many_options::internal::hint(opts)) {
        op.hint(*opt);
    }

    if (auto const& opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto const& opt = v1::update_many_options::internal::array_filters(opts)) {
        op.array_filters(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::update_many_result::internal::make(std::move(*res));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_many_result> update_many_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_many_options const& opts) {
    return update_many_impl(
        std::move(bulk),
        v1::bulk_write::update_many{bsoncxx::v1::document::value{filter}, bsoncxx::v1::document::value{update}},
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_many_result> update_many_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view filter,
    v1::pipeline const& update,
    v1::update_many_options const& opts) {
    return update_many_impl(
        std::move(bulk), v1::bulk_write::update_many{bsoncxx::v1::document::value{filter}, update}, opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result>
update_one_impl(v1::bulk_write bulk, v1::bulk_write::update_one op, v1::update_one_options const& opts) {
    if (auto const& opt = v1::update_one_options::internal::collation(opts)) {
        op.collation(*opt);
    }

    if (auto const& opt = v1::update_one_options::internal::hint(opts)) {
        op.hint(*opt);
    }

    if (auto const& opt = v1::update_one_options::internal::sort(opts)) {
        op.sort(*opt);
    }

    if (auto const opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto const& opt = v1::update_one_options::internal::array_filters(opts)) {
        op.array_filters(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::update_one_result::internal::make(std::move(*res));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_one_result> update_one_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_one_options const& opts) {
    return update_one_impl(
        std::move(bulk),
        v1::bulk_write::update_one{bsoncxx::v1::document::value{filter}, bsoncxx::v1::document::value{update}},
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result> update_one_impl(
    v1::bulk_write bulk,
    bsoncxx::v1::document::view filter,
    v1::pipeline const& update,
    v1::update_one_options const& opts) {
    return update_one_impl(
        std::move(bulk), v1::bulk_write::update_one{bsoncxx::v1::document::value{filter}, update}, opts);
}

v1::change_stream watch_impl(mongoc_collection_t* coll, bsoncxx::v1::array::view pipeline, bson_t const* opts) {
    return v1::change_stream::internal::make(
        libmongoc::collection_watch(coll, scoped_bson_view{pipeline}.bson(), opts));
}

} // namespace

v1::cursor collection::aggregate(v1::pipeline const& pipeline, v1::aggregate_options const& opts) {
    scoped_bson doc;

    v1::aggregate_options::internal::append_to(opts, doc);

    return aggregate_impl(
        impl::with(this)->_coll, scoped_bson_view{pipeline.view_array()}.bson(), doc.bson(), get_read_prefs(opts));
}

v1::cursor collection::aggregate(
    v1::client_session const& session,
    v1::pipeline const& pipeline,
    v1::aggregate_options const& opts) {
    scoped_bson doc;

    v1::aggregate_options::internal::append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return aggregate_impl(
        impl::with(this)->_coll, scoped_bson_view{pipeline.view_array()}.bson(), doc.bson(), get_read_prefs(opts));
}

v1::bulk_write collection::create_bulk_write(v1::bulk_write::options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation());
}

v1::bulk_write collection::create_bulk_write(v1::client_session const& session, v1::bulk_write::options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation());
}

std::int64_t collection::count_documents(bsoncxx::v1::document::view filter, v1::count_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return count_documents_impl(
        impl::with(this)->_coll, scoped_bson_view{filter}.bson(), doc.bson(), get_read_prefs(opts));
}

std::int64_t collection::count_documents(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    v1::count_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return count_documents_impl(
        impl::with(this)->_coll, scoped_bson_view{filter}.bson(), doc.bson(), get_read_prefs(opts));
}

std::int64_t collection::estimated_document_count(v1::estimated_document_count_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return estimated_document_count_impl(impl::with(this)->_coll, doc.bson(), get_read_prefs(opts));
}

bsoncxx::v1::stdx::optional<v1::delete_many_result> collection::delete_many(
    bsoncxx::v1::document::view q,
    v1::delete_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return delete_many_impl(create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), {}), q, opts);
}

bsoncxx::v1::stdx::optional<v1::delete_many_result> collection::delete_many(
    v1::client_session const& session,
    bsoncxx::v1::document::view q,
    v1::delete_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return delete_many_impl(create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), {}), q, opts);
}

bsoncxx::v1::stdx::optional<v1::delete_one_result> collection::delete_one(
    bsoncxx::v1::document::view q,
    v1::delete_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return delete_one_impl(create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), {}), q, opts);
}

bsoncxx::v1::stdx::optional<v1::delete_one_result> collection::delete_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view q,
    v1::delete_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return delete_one_impl(create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), {}), q, opts);
}

v1::cursor collection::distinct(
    bsoncxx::v1::stdx::string_view key,
    bsoncxx::v1::document::view query,
    v1::distinct_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return distinct_impl(
        impl::with(this)->_coll,
        impl::with(this)->_client,
        key,
        scoped_bson_view{query}.bson(),
        doc.bson(),
        get_read_prefs(opts));
}

v1::cursor collection::distinct(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view key,
    bsoncxx::v1::document::view query,
    v1::distinct_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return distinct_impl(
        impl::with(this)->_coll,
        impl::with(this)->_client,
        key,
        scoped_bson_view{query}.bson(),
        doc.bson(),
        get_read_prefs(opts));
}

void collection::drop(bsoncxx::v1::stdx::optional<v1::write_concern> const& wc, bsoncxx::v1::document::view opts) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))};
    }

    doc += opts;

    drop_impl(impl::with(this)->_coll, doc.bson());
}

void collection::drop(
    v1::client_session const& session,
    bsoncxx::v1::stdx::optional<v1::write_concern> const& wc,
    bsoncxx::v1::document::view opts) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{wc->to_document()}.bson()))};
    }

    doc += opts;

    v1::client_session::internal::append_to(session, doc);

    drop_impl(impl::with(this)->_coll, doc.bson());
}

v1::cursor collection::find(bsoncxx::v1::document::view filter, v1::find_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return find_impl(impl::with(this)->_coll, scoped_bson_view{filter}.bson(), doc.bson(), opts);
}

v1::cursor
collection::find(v1::client_session const& session, bsoncxx::v1::document::view filter, v1::find_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return find_impl(impl::with(this)->_coll, scoped_bson_view{filter}.bson(), doc.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one(
    bsoncxx::v1::document::view filter,
    v1::find_options const& opts) {
    return find_one_impl(this->find(filter, std::move(v1::find_options{opts}.limit(1))));
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    v1::find_options const& opts) {
    return find_one_impl(this->find(session, filter, std::move(v1::find_options{opts}.limit(1))));
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_delete(
    bsoncxx::v1::document::view query,
    v1::find_one_and_delete_options const& opts) {
    return find_one_and_delete_impl(impl::with(this)->_coll, nullptr, scoped_bson_view{query}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_delete(
    v1::client_session const& session,
    bsoncxx::v1::document::view query,
    v1::find_one_and_delete_options const& opts) {
    return find_one_and_delete_impl(impl::with(this)->_coll, &session, scoped_bson_view{query}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_replace(
    bsoncxx::v1::document::view query,
    bsoncxx::v1::document::view replacement,
    v1::find_one_and_replace_options const& opts) {
    return find_one_and_replace_impl(
        impl::with(this)->_coll, nullptr, scoped_bson_view{query}.bson(), scoped_bson_view{replacement}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_replace(
    v1::client_session const& session,
    bsoncxx::v1::document::view query,
    bsoncxx::v1::document::view replacement,
    v1::find_one_and_replace_options const& opts) {
    return find_one_and_replace_impl(
        impl::with(this)->_coll, &session, scoped_bson_view{query}.bson(), scoped_bson_view{replacement}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_update(
    bsoncxx::v1::document::view query,
    bsoncxx::v1::document::view update,
    v1::find_one_and_update_options const& opts) {
    return find_one_and_update_impl(
        impl::with(this)->_coll, nullptr, scoped_bson_view{query}.bson(), scoped_bson_view{update}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_update(
    bsoncxx::v1::document::view query,
    v1::pipeline const& update,
    v1::find_one_and_update_options const& opts) {
    return find_one_and_update_impl(
        impl::with(this)->_coll,
        nullptr,
        scoped_bson_view{query}.bson(),
        scoped_bson_view{update.view_array()}.bson(),
        opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_update(
    v1::client_session const& session,
    bsoncxx::v1::document::view query,
    bsoncxx::v1::document::view update,
    v1::find_one_and_update_options const& opts) {
    return find_one_and_update_impl(
        impl::with(this)->_coll, &session, scoped_bson_view{query}.bson(), scoped_bson_view{update}.bson(), opts);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collection::find_one_and_update(
    v1::client_session const& session,
    bsoncxx::v1::document::view query,
    v1::pipeline const& update,
    v1::find_one_and_update_options const& opts) {
    return find_one_and_update_impl(
        impl::with(this)->_coll,
        &session,
        scoped_bson_view{query}.bson(),
        scoped_bson_view{update.view_array()}.bson(),
        opts);
}

v1::indexes collection::indexes() {
    // TODO: v1::indexes (CXX-3237)
    MONGOCXX_PRIVATE_UNREACHABLE;
}

bsoncxx::v1::stdx::optional<v1::insert_one_result> collection::insert_one(
    bsoncxx::v1::document::view document,
    v1::insert_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return insert_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()), document);
}

bsoncxx::v1::stdx::optional<v1::insert_one_result> collection::insert_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view document,
    v1::insert_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return insert_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()), document);
}

bsoncxx::v1::stdx::string_view collection::name() const {
    return libmongoc::collection_get_name(impl::with(this)->_coll);
}

void collection::rename(
    bsoncxx::v1::stdx::string_view new_name,
    bool drop_target,
    bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern) {
    scoped_bson doc;

    if (write_concern) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{write_concern->to_document()}.bson()))};
    }

    return rename_impl(impl::with(this)->_coll, std::string{new_name}.c_str(), drop_target, doc.bson());
}

void collection::rename(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view new_name,
    bool drop_target,
    bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern) {
    scoped_bson doc;

    if (write_concern) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{write_concern->to_document()}.bson()))};
    }

    v1::client_session::internal::append_to(session, doc);

    return rename_impl(impl::with(this)->_coll, std::string{new_name}.c_str(), drop_target, doc.bson());
}

void collection::read_concern(v1::read_concern const& rc) {
    libmongoc::collection_set_read_concern(impl::with(this)->_coll, v1::read_concern::internal::as_mongoc(rc));
}

v1::read_concern collection::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(libmongoc::collection_get_read_concern(impl::with(this)->_coll)));
}

void collection::read_preference(v1::read_preference const& rp) {
    libmongoc::collection_set_read_prefs(impl::with(this)->_coll, v1::read_preference::internal::as_mongoc(rp));
}

v1::read_preference collection::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(libmongoc::collection_get_read_prefs(impl::with(this)->_coll)));
}

bsoncxx::v1::stdx::optional<v1::replace_one_result> collection::replace_one(
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::value replacement,
    v1::replace_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return replace_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        replacement,
        opts);
}

bsoncxx::v1::stdx::optional<v1::replace_one_result> collection::replace_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::value replacement,
    v1::replace_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return replace_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        replacement,
        opts);
}

v1::search_indexes collection::search_indexes() {
    // TODO: v1::search_indexes (CXX-3237)
    MONGOCXX_PRIVATE_UNREACHABLE;
}

bsoncxx::v1::stdx::optional<v1::update_many_result> collection::update_many(
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return update_many_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_many_result> collection::update_many(
    bsoncxx::v1::document::view filter,
    v1::pipeline const& update,
    update_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return update_many_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_many_result> collection::update_many(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return update_many_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_many_result> collection::update_many(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    v1::pipeline const& update,
    v1::update_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return update_many_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result> collection::update_one(
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return update_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result>
collection::update_one(bsoncxx::v1::document::view filter, v1::pipeline const& update, update_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return update_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result> collection::update_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    bsoncxx::v1::document::view update,
    v1::update_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return update_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result> collection::update_one(
    v1::client_session const& session,
    bsoncxx::v1::document::view filter,
    v1::pipeline const& update,
    v1::update_one_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return update_one_impl(
        create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation()),
        filter,
        update,
        opts);
}

v1::change_stream collection::watch(v1::change_stream::options const& opts) {
    return watch_impl(
        impl::with(this)->_coll,
        bsoncxx::v1::array::view{},
        scoped_bson{v1::change_stream::options::internal::to_document(opts)}.bson());
}

v1::change_stream collection::watch(v1::client_session const& session, v1::change_stream::options const& opts) {
    scoped_bson doc{v1::change_stream::options::internal::to_document(opts)};

    v1::client_session::internal::append_to(session, doc);

    return watch_impl(impl::with(this)->_coll, bsoncxx::v1::array::view{}, doc.bson());
}

v1::change_stream collection::watch(v1::pipeline const& pipeline, v1::change_stream::options const& opts) {
    return watch_impl(
        impl::with(this)->_coll,
        pipeline.view_array(),
        scoped_bson{v1::change_stream::options::internal::to_document(opts)}.bson());
}

v1::change_stream collection::watch(
    v1::client_session const& session,
    v1::pipeline const& pipeline,
    v1::change_stream::options const& opts) {
    scoped_bson doc{v1::change_stream::options::internal::to_document(opts)};

    v1::client_session::internal::append_to(session, doc);

    return watch_impl(impl::with(this)->_coll, pipeline.view_array(), doc.bson());
}

void collection::write_concern(v1::write_concern const& wc) {
    libmongoc::collection_set_write_concern(impl::with(this)->_coll, v1::write_concern::internal::as_mongoc(wc));
}

v1::write_concern collection::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(libmongoc::collection_get_write_concern(impl::with(this)->_coll)));
}

std::error_category const& collection::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::collection";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::max_time_u32:
                    return "the \"maxTimeMS\" field must be representable as a `std::uint32_t`";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::max_time_u32:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::max_time_u32:
                        return type == condition::invalid_argument;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

collection::collection(void* impl) : _impl{impl} {}

v1::bulk_write collection::_create_insert_many(v1::client_session const* session, v1::insert_many_options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    if (session) {
        v1::client_session::internal::append_to(*session, doc);
    }

    return create_bulk_write_impl(impl::with(this)->_coll, doc.bson(), opts.bypass_document_validation());
}

void collection::_append_insert_many(
    v1::bulk_write& bulk,
    std::vector<bsoncxx::v1::types::value>& inserted_ids,
    bsoncxx::v1::document::view doc) {
    auto insert_doc = with_id(doc);
    auto id = insert_doc["_id"].type_value();

    bulk.append(v1::bulk_write::insert_one{std::move(insert_doc)});
    inserted_ids.push_back(std::move(id));
}

bsoncxx::v1::stdx::optional<v1::insert_many_result> collection::_execute_insert_many(
    v1::bulk_write& bulk,
    std::vector<bsoncxx::v1::types::value>& inserted_ids) {
    struct deleter_type {
        void operator()(bson_array_builder_t* ptr) const noexcept {
            bson_array_builder_destroy(ptr);
        }
    };

    using builder_ptr_type = std::unique_ptr<bson_array_builder_t, deleter_type>;

    auto const builder_owner = builder_ptr_type{bson_array_builder_new(), deleter_type{}};
    auto const builder = builder_owner.get();

    for (auto const& e : inserted_ids) {
        if (!bson_array_builder_append_value(builder, &bsoncxx::v1::types::value::internal::get_bson_value(e))) {
            throw std::logic_error{
                "mongocxx::v1::collection::_execute_insert_many: bson_array_builder_append_value failed"};
        }
    }

    scoped_bson arr;
    if (!bson_array_builder_build(builder, arr.out_ptr())) {
        throw std::logic_error{"mongocxx::v1::collection::_execute_insert_many: bson_array_builder_build failed"};
    }

    if (auto res = bulk.execute()) {
        return v1::insert_many_result::internal::make(
            std::move(*res), bsoncxx::v1::array::value{std::move(arr).value().release()});
    }

    return {};
}

collection collection::internal::make(mongoc_collection_t* coll, mongoc_client_t* client) {
    return {new impl{coll, client}};
}

mongoc_client_t* collection::internal::get_client(collection& self) {
    return impl::with(self)._client;
}

mongoc_collection_t const* collection::internal::as_mongoc(collection const& self) {
    return impl::with(self)._coll;
}

mongoc_collection_t* collection::internal::as_mongoc(collection& self) {
    return impl::with(self)._coll;
}

} // namespace v1
} // namespace mongocxx
