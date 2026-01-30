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

#include <mongocxx/collection.hh>

//

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/collection.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/delete_many_result.hh>
#include <mongocxx/v1/delete_one_result.hh>
#include <mongocxx/v1/insert_many_result.hh>
#include <mongocxx/v1/insert_one_result.hh>
#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/replace_one_result.hh>
#include <mongocxx/v1/update_many_result.hh>
#include <mongocxx/v1/update_one_result.hh>
#include <mongocxx/v1/write_concern.hh>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>

#include <mongocxx/options/find_one_common_options-fwd.hpp>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/hint.hpp>
#include <mongocxx/index_view.hpp>
#include <mongocxx/model/delete_many.hpp>
#include <mongocxx/model/delete_one.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/model/replace_one.hpp>
#include <mongocxx/model/update_many.hpp>
#include <mongocxx/model/update_one.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/estimated_document_count.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/search_index_view.hpp>

#include <mongocxx/bulk_write.hh>
#include <mongocxx/client_session.hh>
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

using bsoncxx::v_noabi::builder::concatenate;
using bsoncxx::v_noabi::builder::basic::kvp;
using bsoncxx::v_noabi::builder::basic::make_array;
using bsoncxx::v_noabi::builder::basic::make_document;
using bsoncxx::v_noabi::builder::basic::sub_array;
using bsoncxx::v_noabi::builder::basic::sub_document;

namespace mongocxx {
namespace v_noabi {

namespace {

template <typename Collection>
Collection& check_moved_from(Collection& coll) {
    if (!coll) {
        throw v_noabi::logic_error{v_noabi::error_code::k_invalid_collection_object};
    }
    return coll;
}

} // namespace

collection::collection(collection const& other) {
    if (other) {
        _coll = check_moved_from(other)._coll;
    }
}

collection& collection::operator=(collection const& other) {
    if (this != &other) {
        if (!other) {
            _coll = v1::collection{};
        } else {
            _coll = check_moved_from(other)._coll;
        }
    }
    return *this;
}

collection::operator bool() const noexcept {
    return _coll.operator bool();
}

namespace {

template <typename Options>
mongoc_read_prefs_t const* get_read_prefs(Options const& options) {
    auto const& rp_opt = options.read_preference();
    return rp_opt ? v_noabi::read_preference::internal::as_mongoc(*rp_opt) : nullptr;
}

void append_comment(bsoncxx::v_noabi::types::bson_value::view_or_value const& opt, scoped_bson& doc) {
    scoped_bson v;

    if (!BSON_APPEND_VALUE(
            v.inout_ptr(),
            "comment",
            &bsoncxx::v1::types::value::internal::get_bson_value(
                bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(opt.view())}))) {
        throw std::logic_error{"mongocxx::v_noabi::append_comment: BSON_APPEND_VALUE failed"};
    }

    doc += v;
}

void append_hint(v_noabi::hint const& opt, scoped_bson& doc) {
    scoped_bson v;

    if (!BSON_APPEND_VALUE(
            v.inout_ptr(),
            "hint",
            &bsoncxx::v1::types::value::internal::get_bson_value(
                bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(opt.to_value())}))) {
        throw std::logic_error{"mongocxx::v_noabi::append_hint: BSON_APPEND_VALUE failed"};
    }

    doc += v;
}

void append_to(v_noabi::options::bulk_write const& opts, scoped_bson& doc) {
    if (!opts.ordered()) {
        // ordered is true by default. Only append it if set to false.
        doc += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    if (auto const& opt = opts.write_concern()) {
        auto const v = opt->to_document();
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson_view(v).bson()))};
    }

    if (auto const& opt = opts.let()) {
        auto const v = opt->view();
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(v).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::count const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.collation()) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.hint()) {
        append_hint(*opt, doc);
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }

    if (auto const& opt = opts.skip()) {
        doc += scoped_bson{BCON_NEW("skip", BCON_INT64(*opt))};
    }

    if (auto const& opt = opts.limit()) {
        doc += scoped_bson{BCON_NEW("limit", BCON_INT64(*opt))};
    }
}

void append_to(v_noabi::options::estimated_document_count const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson(BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()})));
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::delete_options const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = opts.let()) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::distinct const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.collation()) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::find const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.allow_disk_use()) {
        doc += scoped_bson{BCON_NEW("allowDiskUse", BCON_BOOL(*opt))};
    }

    if (auto const& opt = opts.allow_partial_results()) {
        doc += scoped_bson{BCON_NEW("allowPartialResults", BCON_BOOL(*opt))};
    }

    if (auto const opt = opts.batch_size()) {
        doc += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*opt))};
    }

    if (auto const& opt = opts.collation()) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.comment_option()) {
        append_comment(*opt, doc);
    } else if (auto const& opt = opts.comment()) {
        append_comment(bsoncxx::v_noabi::types::view{bsoncxx::v_noabi::types::b_string{opt->view()}}, doc);
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
                throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
        }
    }

    if (auto const& opt = opts.hint()) {
        append_hint(*opt, doc);
    }

    if (auto const& opt = opts.let()) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.limit()) {
        doc += scoped_bson{BCON_NEW("limit", BCON_INT64(*opt))};
    }

    if (auto const& opt = opts.max()) {
        doc += scoped_bson{BCON_NEW("max", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.max_await_time()) {
        doc += scoped_bson{BCON_NEW("maxAwaitTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = opts.min()) {
        doc += scoped_bson{BCON_NEW("min", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.no_cursor_timeout()) {
        doc += scoped_bson{BCON_NEW("noCursorTimeout", BCON_BOOL(*opt))};
    }

    if (auto const& opt = opts.projection()) {
        doc += scoped_bson{BCON_NEW("projection", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
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

    if (auto const& opt = opts.sort()) {
        doc += scoped_bson{BCON_NEW("sort", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }
}

void append_to(v_noabi::options::insert const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::replace const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = opts.let()) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        append_comment(*opt, doc);
    }
}

void append_to(v_noabi::options::update const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = opts.let()) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = opts.comment()) {
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
        v_noabi::throw_exception<v_noabi::query_exception>(from_v1(std::move(reply)), error);
    }

    return ret;
}

std::int64_t
estimated_document_count_impl(mongoc_collection_t* coll, bson_t const* opts, mongoc_read_prefs_t const* read_prefs) {
    bson_error_t error = {};

    scoped_bson reply;
    auto const ret = libmongoc::collection_estimated_document_count(coll, opts, read_prefs, reply.out_ptr(), &error);

    if (ret < 0) {
        v_noabi::throw_exception<v_noabi::query_exception>(from_v1(std::move(reply)), error);
    }

    return ret;
}

bsoncxx::v1::stdx::optional<v1::delete_many_result> delete_many_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& delete_opts) {
    v_noabi::model::delete_many op{std::move(filter)};

    if (auto const& opt = delete_opts.collation()) {
        op.collation(*opt);
    }

    if (auto const& opt = delete_opts.hint()) {
        op.hint(*opt);
    }

    if (auto ret = bulk.append(op).execute()) {
        return v1::delete_many_result::internal::make(v_noabi::to_v1(std::move(*ret)));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::delete_one_result> delete_one_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& delete_opts) {
    v_noabi::model::delete_one op{std::move(filter)};

    if (auto const& opt = delete_opts.collation()) {
        op.collation(*opt);
    }

    if (auto const& opt = delete_opts.hint()) {
        op.hint(*opt);
    }

    bulk.append(op);

    if (auto ret = bulk.execute()) {
        return v1::delete_one_result::internal::make(v_noabi::to_v1(std::move(*ret)));
    }

    return {};
}

v1::cursor distinct_impl(
    mongoc_collection_t* coll,
    mongoc_client_t* client,
    bsoncxx::v_noabi::string::view_or_value field_name,
    bson_t const* query,
    bson_t const* opts,
    mongoc_read_prefs_t const* read_prefs) {
    scoped_bson const command{BCON_NEW(
        "distinct",
        libmongoc::collection_get_name(coll),
        "key",
        BCON_UTF8(field_name.terminated().data()),
        "query",
        BCON_DOCUMENT(scoped_bson_view{query}))};

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_read_command_with_opts(
            coll, scoped_bson_view{command}.bson(), read_prefs, scoped_bson_view{opts}, reply.out_ptr(), &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(from_v1(std::move(reply)), error);
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
        BCON_UTF8(""),
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
            v_noabi::throw_exception<v_noabi::query_exception>(
                bsoncxx::v_noabi::from_v1(scoped_bson_view{error_document}.value()), error);
        } else {
            v_noabi::throw_exception<v_noabi::query_exception>(error);
        }
    }

    return ret;
}

void drop_impl(mongoc_collection_t* coll, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::collection_drop_with_opts(coll, opts, &error)) {
        // Only throw when the error is not due to a non-existent collection.
        if (error.code != MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST) {
            v_noabi::throw_exception<v_noabi::operation_exception>(error);
        }
    }
}

v1::cursor find_impl(
    mongoc_collection_t* coll,
    bson_t const* filter,
    bson_t const* opts,
    v_noabi::options::find const& find_opts) {
    return v1::cursor::internal::make(
        libmongoc::collection_find_with_opts(coll, filter, opts, get_read_prefs(find_opts)), find_opts.cursor_type());
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_one_impl(v_noabi::cursor cursor) {
    auto const iter = cursor.begin();

    if (iter != cursor.end()) {
        return bsoncxx::v_noabi::document::value{*iter};
    }

    return {};
}

template <typename Options>
bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_and_modify_impl(
    mongoc_collection_t* coll,
    v_noabi::client_session const* session_ptr,
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

    bson_error_t error = {};

    // Some options must be passed via an `extra` BSON document.
    {
        scoped_bson extra;

        if (auto const& opt = options.write_concern()) {
            if (!opt->is_acknowledged() && options.collation()) {
                throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
            }

            extra += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
        }

        if (session_ptr) {
            v_noabi::client_session::internal::append_to(*session_ptr, extra);
        }

        if (auto const& opt = options.collation()) {
            extra += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
        }

        if (array_filters) {
            extra += scoped_bson{BCON_NEW("arrayFilters", BCON_ARRAY(array_filters))};
        }

        if (auto const& opt = options.hint()) {
            append_hint(*opt, extra);
        }

        if (auto const& opt = options.let()) {
            extra += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
        }

        if (auto const& opt = options.comment()) {
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

        if (auto const& opt = options.sort()) {
            libmongoc::find_and_modify_opts_set_sort(opts, to_scoped_bson_view(*opt).bson());
        }

        if (auto const& opt = options.projection()) {
            libmongoc::find_and_modify_opts_set_fields(opts, to_scoped_bson_view(*opt).bson());
        }

        if (auto const& opt = options.max_time()) {
            // Leave unchecked for backward compatibility.
            libmongoc::find_and_modify_opts_set_max_time_ms(opts, static_cast<std::uint32_t>(opt->count()));
        }

        libmongoc::find_and_modify_opts_set_flags(opts, flags);
    }

    scoped_bson reply;
    if (!libmongoc::collection_find_and_modify_with_opts(coll, filter, opts, reply.out_ptr(), &error)) {
        if (reply.view().empty()) {
            v_noabi::throw_exception<v_noabi::write_exception>(error);
        } else {
            v_noabi::throw_exception<v_noabi::write_exception>(mongocxx::from_v1(std::move(reply)), error);
        }
    }

    auto const value = reply.view()["value"];

    // Leave check against k_null instead of k_document for backward compatibility.
    if (value && value.type_id() != bsoncxx::v1::types::id::k_null) {
        return bsoncxx::v_noabi::document::value{bsoncxx::v_noabi::from_v1(value.get_document().value)};
    }

    return {};
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_one_and_delete_impl(
    mongoc_collection_t* coll,
    v_noabi::client_session const* session_ptr,
    bson_t const* query,
    v_noabi::options::find_one_and_delete const& opts) {
    return find_and_modify_impl(coll, session_ptr, query, nullptr, MONGOC_FIND_AND_MODIFY_REMOVE, false, nullptr, opts);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_one_and_replace_impl(
    mongoc_collection_t* coll,
    v_noabi::client_session const* session_ptr,
    bson_t const* query,
    bson_t const* replacement,
    v_noabi::options::find_one_and_replace const& options) {
    auto flags = MONGOC_FIND_AND_MODIFY_NONE;

    if (options.upsert()) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == v_noabi::options::return_document::k_after) {
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

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> find_one_and_update_impl(
    mongoc_collection_t* coll,
    v_noabi::client_session const* session_ptr,
    bson_t const* query,
    bson_t const* update,
    v_noabi::options::find_one_and_update const& options) {
    auto flags = MONGOC_FIND_AND_MODIFY_NONE;

    if (options.upsert()) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_UPSERT);
    }

    if (options.return_document() == v_noabi::options::return_document::k_after) {
        flags = static_cast<mongoc_find_and_modify_flags_t>(flags | MONGOC_FIND_AND_MODIFY_RETURN_NEW);
    }

    auto const& array_filters_ptr = [&]() -> bsoncxx::v_noabi::array::view_or_value const* {
        if (auto const& opt = options.array_filters()) {
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
        array_filters_ptr ? to_scoped_bson_view(array_filters_ptr->view()).bson() : nullptr,
        options);
}

void with_id(bsoncxx::v_noabi::document::view_or_value& doc) {
    if (doc.view()["_id"]) {
        return;
    }

    bsoncxx::v1::oid oid;
    bson_oid_t bson_oid = {};
    std::memcpy(bson_oid.bytes, oid.bytes(), oid.size());

    scoped_bson new_doc{BCON_NEW("_id", BCON_OID(&bson_oid))};
    new_doc += to_scoped_bson_view(doc);
    doc = bsoncxx::v_noabi::from_v1(std::move(new_doc).value());
}

bsoncxx::v1::stdx::optional<v1::insert_one_result> insert_one_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value document) {
    with_id(document);

    auto id = document.view()["_id"].type_value();

    if (auto res = bulk.append(v_noabi::model::insert_one{document}).execute()) {
        return v1::insert_one_result::internal::make(
            v_noabi::to_v1(std::move(*res)), bsoncxx::v_noabi::to_v1(std::move(id)));
    }

    return {};
}

void rename_impl(mongoc_collection_t* coll, char const* new_name, bool drop_target, bson_t const* opts) {
    bson_error_t error = {};

    if (!libmongoc::collection_rename_with_opts(
            coll, nullptr, std::string{new_name}.c_str(), drop_target, opts, &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }
}
bsoncxx::v1::stdx::optional<v1::replace_one_result> replace_one_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value replacement,
    v_noabi::options::replace const& opts) {
    v_noabi::model::replace_one op{std::move(filter), std::move(replacement)};

    if (auto const& opt = opts.collation()) {
        op.collation(*opt);
    }

    if (auto const& opt = opts.hint()) {
        op.hint(*opt);
    }

    if (auto const& opt = opts.sort()) {
        op.sort(*opt);
    }

    if (auto const opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::replace_one_result::internal::make(v_noabi::to_v1(std::move(*res)));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_many_result>
update_many_impl(v_noabi::bulk_write bulk, v_noabi::model::update_many op, v_noabi::options::update const& opts) {
    if (auto const& opt = opts.collation()) {
        op.collation(*opt);
    }

    if (auto const& opt = opts.hint()) {
        op.hint(*opt);
    }

    if (auto const& opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto const& opt = opts.array_filters()) {
        op.array_filters(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::update_many_result::internal::make(v_noabi::to_v1(std::move(*res)));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_many_result> update_many_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& opts) {
    return update_many_impl(std::move(bulk), v_noabi::model::update_many{std::move(filter), std::move(update)}, opts);
}

bsoncxx::v1::stdx::optional<v1::update_many_result> update_many_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& opts) {
    return update_many_impl(std::move(bulk), v_noabi::model::update_many{std::move(filter), update}, opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result>
update_one_impl(v_noabi::bulk_write bulk, v_noabi::model::update_one op, v_noabi::options::update const& opts) {
    if (auto const& opt = opts.collation()) {
        op.collation(*opt);
    }

    if (auto const& opt = opts.hint()) {
        op.hint(*opt);
    }

    if (auto const& opt = opts.sort()) {
        op.sort(*opt);
    }

    if (auto const opt = opts.upsert()) {
        op.upsert(*opt);
    }

    if (auto const& opt = opts.array_filters()) {
        op.array_filters(*opt);
    }

    if (auto res = bulk.append(op).execute()) {
        return v1::update_one_result::internal::make(v_noabi::to_v1(std::move(*res)));
    }

    return {};
}

bsoncxx::v1::stdx::optional<v1::update_one_result> update_one_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& opts) {
    return update_one_impl(std::move(bulk), v_noabi::model::update_one{std::move(filter), std::move(update)}, opts);
}

bsoncxx::v1::stdx::optional<v1::update_one_result> update_one_impl(
    v_noabi::bulk_write bulk,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& opts) {
    return update_one_impl(std::move(bulk), v_noabi::model::update_one{std::move(filter), update}, opts);
}

v1::change_stream watch_impl(mongoc_collection_t* coll, bsoncxx::v_noabi::array::view pipeline, bson_t const* opts) {
    return v1::change_stream::internal::make(
        libmongoc::collection_watch(
            coll, scoped_bson{BCON_NEW("pipeline", BCON_ARRAY(to_scoped_bson_view(pipeline).bson()))}.bson(), opts));
}

} // namespace

v_noabi::cursor collection::aggregate(v_noabi::pipeline const& pipeline, v_noabi::options::aggregate const& options) {
    scoped_bson doc;

    v_noabi::options::aggregate::internal::append_to(options, doc);

    return aggregate_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(pipeline.view_array()).bson(),
        doc.bson(),
        get_read_prefs(options));
}

v_noabi::cursor collection::aggregate(
    v_noabi::client_session const& session,
    v_noabi::pipeline const& pipeline,
    v_noabi::options::aggregate const& options) {
    scoped_bson doc;

    v_noabi::options::aggregate::internal::append_to(options, doc);
    v_noabi::client_session::internal::append_to(session, doc);

    return aggregate_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(pipeline.view_array()).bson(),
        doc.bson(),
        get_read_prefs(options));
}

v_noabi::bulk_write collection::create_bulk_write(v_noabi::options::bulk_write const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return create_bulk_write_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), options.bypass_document_validation());
}

v_noabi::bulk_write collection::create_bulk_write(
    v_noabi::client_session const& session,
    v_noabi::options::bulk_write const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return create_bulk_write_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), options.bypass_document_validation());
}

std::int64_t collection::count_documents(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::count const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return count_documents_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(filter).bson(),
        doc.bson(),
        get_read_prefs(options));
}

std::int64_t collection::count_documents(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::count const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return count_documents_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(filter).bson(),
        doc.bson(),
        get_read_prefs(options));
}

std::int64_t collection::estimated_document_count(v_noabi::options::estimated_document_count const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return estimated_document_count_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), get_read_prefs(options));
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result> collection::delete_many(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return delete_many_impl(
        create_bulk_write_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), {}),
        filter.view(),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result> collection::delete_many(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return delete_many_impl(
        create_bulk_write_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), {}),
        filter.view(),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result> collection::delete_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return delete_one_impl(
        create_bulk_write_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), {}),
        filter.view(),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::delete_result> collection::delete_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::delete_options const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return delete_one_impl(
        create_bulk_write_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), {}),
        filter.view(),
        options);
}

v_noabi::cursor collection::distinct(
    bsoncxx::v_noabi::string::view_or_value field_name,
    bsoncxx::v_noabi::document::view_or_value query,
    v_noabi::options::distinct const& options) {
    scoped_bson doc;

    append_to(options, doc);

    auto& c = check_moved_from(_coll);
    return distinct_impl(
        v1::collection::internal::as_mongoc(c),
        v1::collection::internal::get_client(c),
        field_name,
        to_scoped_bson_view(query).bson(),
        doc.bson(),
        get_read_prefs(options));
}

v_noabi::cursor collection::distinct(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::string::view_or_value field_name,
    bsoncxx::v_noabi::document::view_or_value query,
    v_noabi::options::distinct const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    auto& c = check_moved_from(_coll);
    return distinct_impl(
        v1::collection::internal::as_mongoc(c),
        v1::collection::internal::get_client(c),
        field_name,
        to_scoped_bson_view(query).bson(),
        doc.bson(),
        get_read_prefs(options));
}

void collection::drop(
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& wc,
    bsoncxx::v_noabi::document::view_or_value collection_options) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(wc->to_document()).bson()))};
    }

    doc += to_scoped_bson_view(collection_options);

    drop_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson());
}

void collection::drop(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& wc,
    bsoncxx::v_noabi::document::view_or_value collection_options) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(wc->to_document()).bson()))};
    }

    doc += to_scoped_bson_view(collection_options);

    v_noabi::client_session::internal::append_to(session, doc);

    drop_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson());
}

v_noabi::cursor collection::find(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return find_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(filter).bson(),
        doc.bson(),
        options);
}

v_noabi::cursor collection::find(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return find_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        to_scoped_bson_view(filter).bson(),
        doc.bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find const& options) {
    return find_one_impl(this->find(filter, std::move(v_noabi::options::find{options}.limit(1))));
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find const& options) {
    return find_one_impl(this->find(session, filter, std::move(v_noabi::options::find{options}.limit(1))));
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_delete(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find_one_and_delete const& options) {
    return find_one_and_delete_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        nullptr,
        to_scoped_bson_view(filter).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_delete(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::find_one_and_delete const& options) {
    return find_one_and_delete_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        &session,
        to_scoped_bson_view(filter).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_replace(
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value replacement,
    v_noabi::options::find_one_and_replace const& options) {
    return find_one_and_replace_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        nullptr,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(replacement).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_replace(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value replacement,
    v_noabi::options::find_one_and_replace const& options) {
    return find_one_and_replace_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        &session,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(replacement).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        nullptr,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(update).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        nullptr,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(update.view_array()).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        nullptr,
        to_scoped_bson_view(filter).bson(),
        scoped_bson{}.bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        &session,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(update).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        &session,
        to_scoped_bson_view(filter).bson(),
        to_scoped_bson_view(update.view_array()).bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> collection::find_one_and_update(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::find_one_and_update const& options) {
    return find_one_and_update_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        &session,
        to_scoped_bson_view(filter).bson(),
        scoped_bson{}.bson(),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_one> collection::insert_one(
    bsoncxx::v_noabi::document::view_or_value document,
    v_noabi::options::insert const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return insert_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(document));
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_one> collection::insert_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value document,
    v_noabi::options::insert const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return insert_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(document));
}

v_noabi::index_view collection::indexes() {
    auto& c = check_moved_from(_coll);

    return index_view{v1::collection::internal::as_mongoc(c), v1::collection::internal::get_client(c)};
}

v_noabi::cursor collection::list_indexes() const {
    // Backward compatibility: `list_indexes()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v_noabi::collection&>(check_moved_from(*this));

    return c.indexes().list();
}

v_noabi::cursor collection::list_indexes(v_noabi::client_session const& session) const {
    // Backward compatibility: `list_indexes()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& c = const_cast<v_noabi::collection&>(check_moved_from(*this));

    return c.indexes().list(session);
}

bsoncxx::v_noabi::stdx::string_view collection::name() const {
    return check_moved_from(_coll).name();
}

void collection::rename(
    bsoncxx::v_noabi::string::view_or_value new_name,
    bool drop_target_before_rename,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& wc) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(wc->to_document()).bson()))};
    }

    return rename_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        new_name.terminated().data(),
        drop_target_before_rename,
        doc.bson());
}

void collection::rename(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::string::view_or_value new_name,
    bool drop_target_before_rename,
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& wc) {
    scoped_bson doc;

    if (wc) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(wc->to_document()).bson()))};
    }

    v_noabi::client_session::internal::append_to(session, doc);

    return rename_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        new_name.terminated().data(),
        drop_target_before_rename,
        doc.bson());
}

void collection::read_concern(v_noabi::read_concern rc) {
    libmongoc::collection_set_read_concern(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), v_noabi::read_concern::internal::as_mongoc(rc));
}

v_noabi::read_concern collection::read_concern() const {
    return v1::read_concern::internal::make(
        libmongoc::read_concern_copy(
            libmongoc::collection_get_read_concern(v1::collection::internal::as_mongoc(check_moved_from(_coll)))));
}

void collection::read_preference(v_noabi::read_preference rp) {
    libmongoc::collection_set_read_prefs(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        v_noabi::read_preference::internal::as_mongoc(rp));
}

v_noabi::read_preference collection::read_preference() const {
    return v1::read_preference::internal::make(
        libmongoc::read_prefs_copy(
            libmongoc::collection_get_read_prefs(v1::collection::internal::as_mongoc(check_moved_from(_coll)))));
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::replace_one> collection::replace_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value replacement,
    v_noabi::options::replace const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return replace_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(replacement),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::replace_one> collection::replace_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value replacement,
    v_noabi::options::replace const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return replace_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(replacement),
        options);
}

v_noabi::search_index_view collection::search_indexes() {
    auto& c = check_moved_from(_coll);

    return search_index_view{v1::collection::internal::as_mongoc(c), v1::collection::internal::get_client(c)};
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(update),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        update,
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        bsoncxx::v_noabi::document::view{},
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(update),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        update,
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_many(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_many_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        bsoncxx::v_noabi::document::view{},
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(update),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        update,
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        bsoncxx::v_noabi::document::view{},
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    bsoncxx::v_noabi::document::view_or_value update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        std::move(update),
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::pipeline const& update,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        update,
        options);
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::update> collection::update_one(
    v_noabi::client_session const& session,
    bsoncxx::v_noabi::document::view_or_value filter,
    std::initializer_list<_empty_doc_tag>,
    v_noabi::options::update const& options) {
    scoped_bson doc;

    append_to(options, doc);

    v_noabi::client_session::internal::append_to(session, doc);

    return update_one_impl(
        create_bulk_write_impl(
            v1::collection::internal::as_mongoc(check_moved_from(_coll)),
            doc.bson(),
            options.bypass_document_validation()),
        std::move(filter),
        bsoncxx::v_noabi::document::view{},
        options);
}

v_noabi::change_stream collection::watch(v_noabi::options::change_stream const& options) {
    return watch_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        bsoncxx::v1::array::view{},
        to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options)).bson());
}

v_noabi::change_stream collection::watch(
    v_noabi::client_session const& session,
    v_noabi::options::change_stream const& options) {
    auto doc = to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));

    v_noabi::client_session::internal::append_to(session, doc);

    return watch_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), bsoncxx::v1::array::view{}, doc.bson());
}

v_noabi::change_stream collection::watch(pipeline const& pipe, v_noabi::options::change_stream const& options) {
    return watch_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)),
        pipe.view_array(),
        to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options)).bson());
}

v_noabi::change_stream collection::watch(
    v_noabi::client_session const& session,
    v_noabi::pipeline const& pipe,
    v_noabi::options::change_stream const& options) {
    auto doc = to_scoped_bson(v_noabi::options::change_stream::internal::to_document(options));

    v_noabi::client_session::internal::append_to(session, doc);

    return watch_impl(v1::collection::internal::as_mongoc(check_moved_from(_coll)), pipe.view_array(), doc.bson());
}

void collection::write_concern(v_noabi::write_concern wc) {
    libmongoc::collection_set_write_concern(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), v_noabi::write_concern::internal::as_mongoc(wc));
}

v_noabi::write_concern collection::write_concern() const {
    return v1::write_concern::internal::make(
        libmongoc::write_concern_copy(
            libmongoc::collection_get_write_concern(v1::collection::internal::as_mongoc(check_moved_from(_coll)))));
}

v_noabi::bulk_write collection::_init_insert_many(
    v_noabi::options::insert const& options,
    v_noabi::client_session const* session) {
    scoped_bson doc;

    // Only include "ordered" when `false` (not the default behavior).
    if (options.ordered().value_or(true) == false) {
        doc += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    append_to(options, doc);

    if (session) {
        v_noabi::client_session::internal::append_to(*session, doc);
    }

    return create_bulk_write_impl(
        v1::collection::internal::as_mongoc(check_moved_from(_coll)), doc.bson(), options.bypass_document_validation());
}

void collection::_insert_many_doc_handler(
    v_noabi::bulk_write& writes,
    bsoncxx::v_noabi::builder::basic::array& inserted_ids,
    bsoncxx::v_noabi::document::view doc) const {
    if (auto const id = doc["_id"]) {
        writes.append(v_noabi::model::insert_one{doc});
        inserted_ids.append(id.type_value());
    } else {
        bsoncxx::v_noabi::oid const oid;
        bson_oid_t bson_oid = {};
        std::memcpy(bson_oid.bytes, oid.bytes(), oid.size());

        scoped_bson new_doc{BCON_NEW("_id", BCON_OID(&bson_oid))};
        new_doc += to_scoped_bson_view(doc);

        writes.append(v_noabi::model::insert_one{bsoncxx::v_noabi::from_v1(new_doc.view())});
        inserted_ids.append(oid);
    }
}

bsoncxx::v_noabi::stdx::optional<v_noabi::result::insert_many> collection::_exec_insert_many(
    v_noabi::bulk_write& writes,
    bsoncxx::v_noabi::builder::basic::array& inserted_ids) {
    if (auto res = writes.execute()) {
        return v1::insert_many_result::internal::make(
            v_noabi::to_v1(std::move(*res)), bsoncxx::v_noabi::to_v1(inserted_ids.extract()));
    }

    return {};
}

mongoc_collection_t* collection::internal::as_mongoc(collection& self) {
    return v1::collection::internal::as_mongoc(check_moved_from(self._coll));
}

} // namespace v_noabi
} // namespace mongocxx
