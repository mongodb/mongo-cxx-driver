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

#include <mongocxx/collection.hpp>

#include <chrono>
#include <cstdint>
#include <limits>
#include <tuple>
#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/private/helpers.hh>
#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/error_category.hh>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/options/private/rewriter.hh>
#include <mongocxx/private/bulk_write.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/collection.hh>
#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/database.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/private/pipeline.hh>
#include <mongocxx/private/read_concern.hh>
#include <mongocxx/private/read_preference.hh>
#include <mongocxx/private/write_concern.hh>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::sub_array;
using bsoncxx::builder::basic::sub_document;
using bsoncxx::builder::concatenate;
using bsoncxx::document::view_or_value;

namespace {

const char* get_collection_name(mongoc_collection_t* collection) {
    return mongocxx::libmongoc::collection_get_name(collection);
}

mongocxx::stdx::optional<bsoncxx::document::value> find_and_modify(
    mongoc_collection_t* collection,
    bsoncxx::document::view command,
    bsoncxx::document::view opts,
    const bsoncxx::stdx::optional<mongocxx::write_concern>& wc) {
    bsoncxx::builder::basic::document opts_builder;
    opts_builder.append(concatenate(opts));

    if (wc) {
        if (!wc->is_acknowledged() && opts["collation"]) {
            throw mongocxx::logic_error{mongocxx::error_code::k_invalid_parameter};
        }
        opts_builder.append(concatenate(wc->to_document()));
    }

    mongocxx::libbson::scoped_bson_t command_bson{command};
    mongocxx::libbson::scoped_bson_t opts_bson{opts_builder.extract()};
    mongocxx::libbson::scoped_bson_t reply;
    ::bson_error_t error;

    bool result = mongocxx::libmongoc::collection_write_command_with_opts(
        collection, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

    if (!result) {
        if (!reply.view().empty()) {
            mongocxx::throw_exception<mongocxx::write_exception>(reply.steal(), error);
        }
        mongocxx::throw_exception<mongocxx::write_exception>(error);
    }

    bsoncxx::document::view reply_view = reply.view();

    if (reply_view["value"].type() == bsoncxx::type::k_null) {
        return mongocxx::stdx::optional<bsoncxx::document::value>{};
    }

    return bsoncxx::document::value{reply_view["value"].get_document().view()};
}

// TODO move these to a private header
template <typename T>
class guard {
   public:
    guard(T&& t) : _t{std::move(t)} {}

    ~guard() {
        _t();
    }

   private:
    T _t;
};

template <typename T>
guard<T> make_guard(T&& t) {
    return guard<T>{std::forward<T>(t)};
}

}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using namespace libbson;

collection::collection() noexcept = default;
collection::collection(collection&&) noexcept = default;
collection& collection::operator=(collection&&) noexcept = default;
collection::~collection() = default;

collection::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

stdx::string_view collection::name() const {
    return {get_collection_name(_get_impl().collection_t)};
}

void collection::rename(bsoncxx::string::view_or_value new_name,
                        bool drop_target_before_rename,
                        const bsoncxx::stdx::optional<mongocxx::write_concern>& wc) {
    bson_error_t error;

    bsoncxx::builder::basic::document opts_doc;
    if (wc) {
        opts_doc.append(kvp("writeConcern", wc->to_document()));
    }

    scoped_bson_t opts_bson{opts_doc.view()};

    bool result = libmongoc::collection_rename_with_opts(_get_impl().collection_t,
                                                         _get_impl().database_name.c_str(),
                                                         new_name.terminated().data(),
                                                         drop_target_before_rename,
                                                         opts_bson.bson(),
                                                         &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }
}

collection::collection(const database& database, bsoncxx::string::view_or_value collection_name)
    : _impl(stdx::make_unique<impl>(
          libmongoc::database_get_collection(database._get_impl().database_t,
                                             collection_name.terminated().data()),
          database.name(),
          database._get_impl().client_impl)) {}

collection::collection(const database& database, void* collection)
    : _impl(stdx::make_unique<impl>(static_cast<mongoc_collection_t*>(collection),
                                    database.name(),
                                    database._get_impl().client_impl)) {}

collection::collection(const collection& c) {
    if (c) {
        _impl = stdx::make_unique<impl>(c._get_impl());
    }
}

collection& collection::operator=(const collection& c) {
    if (!c) {
        _impl.reset();
    } else if (!*this) {
        _impl = stdx::make_unique<impl>(c._get_impl());
    } else {
        *_impl = c._get_impl();
    }

    return *this;
}

bulk_write collection::create_bulk_write(const options::bulk_write& options) {
    class bulk_write writes {
        *this, options
    };

    return writes;
}

stdx::optional<result::bulk_write> collection::bulk_write(const class bulk_write& bulk_write) {
    mongoc_bulk_operation_t* b = bulk_write._impl->operation_t;

    // Before collection::create_bulk_write was added, bulk_writes were created from their
    // constructor without a reference to a collection, and the collection was set on the bulk_write
    // object here. However, since libmongoc::bulk_operation_set_collection takes the name of a
    // collection rather than a reference to the collection itself, it doesn't inherit any write
    // concern that may be set on the collection. The legacy bulk_write constructor is deprecated
    // but remains for backwards compatibility, so we need to set the client, database, and
    // collection here for bulk writes created from that constructor.
    if (!bulk_write._created_from_collection) {
        libmongoc::bulk_operation_set_client(b, _get_impl().client_impl->client_t);
        libmongoc::bulk_operation_set_database(b, _get_impl().database_name.c_str());
        libmongoc::bulk_operation_set_collection(b, get_collection_name(_get_impl().collection_t));
    }

    scoped_bson_t reply;

    bson_error_t error;

    if (!libmongoc::bulk_operation_execute(b, reply.bson_for_init(), &error)) {
        throw_exception<bulk_write_exception>(reply.steal(), error);
    }

    // Reply is empty for unacknowledged writes, so return disengaged optional.
    if (reply.view().empty()) {
        return stdx::nullopt;
    }

    result::bulk_write result(reply.steal());

    return stdx::optional<result::bulk_write>(std::move(result));
}

namespace {

bsoncxx::document::value build_find_options_document(const options::find& options) {
    bsoncxx::builder::basic::document options_builder;

    if (options.allow_partial_results()) {
        options_builder.append(kvp("allowPartialResults", *options.allow_partial_results()));
    }

    if (options.batch_size()) {
        options_builder.append(kvp("batchSize", *options.batch_size()));
    }

    if (options.collation()) {
        options_builder.append(kvp("collation", *options.collation()));
    }

    if (options.comment()) {
        options_builder.append(kvp("comment", *options.comment()));
    }

    if (options.cursor_type()) {
        if (*options.cursor_type() == cursor::type::k_tailable) {
            options_builder.append(kvp("tailable", bsoncxx::types::b_bool{true}));
        } else if (*options.cursor_type() == cursor::type::k_tailable_await) {
            options_builder.append(kvp("tailable", bsoncxx::types::b_bool{true}));
            options_builder.append(kvp("awaitData", bsoncxx::types::b_bool{true}));
        } else if (*options.cursor_type() == cursor::type::k_non_tailable) {
        } else {
            throw logic_error{error_code::k_invalid_parameter};
        }
    }

    if (options.hint()) {
        options_builder.append(kvp("hint", options.hint()->to_value()));
    }

    if (options.limit()) {
        options_builder.append(kvp("limit", *options.limit()));
    }

    if (options.max()) {
        options_builder.append(kvp("max", *options.max()));
    }

    if (options.max_scan()) {
        options_builder.append(kvp("maxScan", *options.max_scan()));
    }

    if (options.max_time()) {
        options_builder.append(
            kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    if (options.min()) {
        options_builder.append(kvp("min", *options.min()));
    }

    if (options.no_cursor_timeout()) {
        options_builder.append(kvp("noCursorTimeout", *options.no_cursor_timeout()));
    }

    if (options.projection()) {
        options_builder.append(
            kvp("projection", bsoncxx::types::b_document{*options.projection()}));
    }

    if (options.return_key()) {
        options_builder.append(kvp("returnKey", *options.return_key()));
    }

    if (options.show_record_id()) {
        options_builder.append(kvp("showRecordId", *options.show_record_id()));
    }

    if (options.skip()) {
        options_builder.append(kvp("skip", *options.skip()));
    }

    if (options.snapshot()) {
        options_builder.append(kvp("snapshot", *options.snapshot()));
    }

    if (options.sort()) {
        options_builder.append(kvp("sort", bsoncxx::types::b_document{*options.sort()}));
    }

    return options_builder.extract();
}

}  // namespace

cursor collection::find(view_or_value filter, const options::find& options) {
    // libmongoc::collection_find_with_opts does not support the legacy "modifiers" options, so we
    // must copy the options struct and convert all of the modifiers options to their modern
    // equivalents.
    auto options_converted = options::rewriter::rewrite_find_modifiers(options);

    scoped_bson_t filter_bson{std::move(filter)};
    scoped_bson_t options_bson{build_find_options_document(options_converted)};

    const mongoc_read_prefs_t* rp_ptr = NULL;
    if (options_converted.read_preference()) {
        rp_ptr = options_converted.read_preference()->_impl->read_preference_t;
    }

    cursor query_cursor{
        libmongoc::collection_find_with_opts(
            _get_impl().collection_t, filter_bson.bson(), options_bson.bson(), rp_ptr),
        options_converted.cursor_type()};

    if (options_converted.max_await_time()) {
        const auto count = options_converted.max_await_time()->count();
        if ((count < 0) || (count >= std::numeric_limits<std::uint32_t>::max())) {
            throw logic_error{error_code::k_invalid_parameter};
        }
        libmongoc::cursor_set_max_await_time_ms(query_cursor._impl->cursor_t,
                                                static_cast<std::uint32_t>(count));
    }

    return query_cursor;
}

stdx::optional<bsoncxx::document::value> collection::find_one(view_or_value filter,
                                                              const options::find& options) {
    options::find copy(options);
    copy.limit(1);
    cursor cursor = find(filter, copy);
    cursor::iterator it = cursor.begin();
    if (it == cursor.end()) {
        return stdx::nullopt;
    }
    return stdx::optional<bsoncxx::document::value>(bsoncxx::document::value{*it});
}

cursor collection::aggregate(const pipeline& pipeline, const options::aggregate& options) {
    scoped_bson_t stages(bsoncxx::document::view(pipeline._impl->view_array()));

    bsoncxx::builder::basic::document b;

    if (options.allow_disk_use()) {
        b.append(kvp("allowDiskUse", *options.allow_disk_use()));
    }

    if (options.collation()) {
        b.append(kvp("collation", *options.collation()));
    }

    if (options.use_cursor()) {
        b.append(kvp("cursor", [&options](sub_document sub_doc) {
            if (options.batch_size()) {
                sub_doc.append(kvp("batchSize", *options.batch_size()));
            }
        }));
    }

    if (options.max_time()) {
        b.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    if (options.bypass_document_validation()) {
        b.append(kvp("bypassDocumentValidation", *options.bypass_document_validation()));
    }

    if (options.hint()) {
        b.append(kvp("hint", options.hint()->to_value()));
    }

    if (options.write_concern()) {
        b.append(kvp("writeConcern", options.write_concern()->to_document()));
    }

    scoped_bson_t options_bson(b.view());

    const ::mongoc_read_prefs_t* rp_ptr = NULL;

    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    return cursor(libmongoc::collection_aggregate(_get_impl().collection_t,
                                                  static_cast<::mongoc_query_flags_t>(0),
                                                  stages.bson(),
                                                  options_bson.bson(),
                                                  rp_ptr));
}

stdx::optional<result::insert_one> collection::insert_one(view_or_value document,
                                                          const options::insert& options) {
    // TODO: We should consider making it possible to convert from an options::insert into
    // an options::bulk_write at the type level, removing the need to re-iterate this code
    // many times here and below.
    //
    // See comments in: https://github.com/mongodb/mongo-cxx-driver/pull/409

    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    if (options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*options.bypass_document_validation());
    }

    auto bulk_op = create_bulk_write(bulk_opts);
    bsoncxx::document::element oid{};

    bsoncxx::builder::basic::document new_document;
    if (!document.view()["_id"]) {
        new_document.append(kvp("_id", bsoncxx::oid()));
        new_document.append(concatenate(document));
        bulk_op.append(model::insert_one(new_document.view()));
        oid = new_document.view()["_id"];
    } else {
        bulk_op.append(model::insert_one(document));
        oid = document.view()["_id"];
    }

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::insert_one>(
        result::insert_one(std::move(result.value()), std::move(oid.get_value())));
}

stdx::optional<result::replace_one> collection::replace_one(view_or_value filter,
                                                            view_or_value replacement,
                                                            const options::update& options) {
    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*options.bypass_document_validation());
    }
    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    auto bulk_op = create_bulk_write(bulk_opts);

    model::replace_one replace_op(filter, replacement);
    if (options.collation()) {
        replace_op.collation(*options.collation());
    }
    if (options.upsert()) {
        replace_op.upsert(*options.upsert());
    }

    bulk_op.append(replace_op);

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::replace_one>(result::replace_one(std::move(result.value())));
}

stdx::optional<result::update> collection::update_many(view_or_value filter,
                                                       view_or_value update,
                                                       const options::update& options) {
    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*options.bypass_document_validation());
    }
    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    auto bulk_op = create_bulk_write(bulk_opts);

    model::update_many update_op(filter, update);
    if (options.collation()) {
        update_op.collation(*options.collation());
    }
    if (options.upsert()) {
        update_op.upsert(*options.upsert());
    }

    bulk_op.append(update_op);

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::update>(result::update(std::move(result.value())));
}

stdx::optional<result::delete_result> collection::delete_many(
    view_or_value filter, const options::delete_options& options) {
    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    auto bulk_op = create_bulk_write(bulk_opts);

    model::delete_many delete_op(filter);
    if (options.collation()) {
        delete_op.collation(*options.collation());
    }
    bulk_op.append(delete_op);

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

stdx::optional<result::update> collection::update_one(view_or_value filter,
                                                      view_or_value update,
                                                      const options::update& options) {
    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.bypass_document_validation()) {
        bulk_opts.bypass_document_validation(*options.bypass_document_validation());
    }
    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    auto bulk_op = create_bulk_write(bulk_opts);

    model::update_one update_op(filter, update);
    if (options.collation()) {
        update_op.collation(*options.collation());
    }
    if (options.upsert()) {
        update_op.upsert(*options.upsert());
    }

    bulk_op.append(update_op);

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::update>(result::update(std::move(result.value())));
}

stdx::optional<result::delete_result> collection::delete_one(
    view_or_value filter, const options::delete_options& options) {
    options::bulk_write bulk_opts;
    bulk_opts.ordered(false);

    if (options.write_concern()) {
        bulk_opts.write_concern(*options.write_concern());
    }

    auto bulk_op = create_bulk_write(bulk_opts);

    model::delete_one delete_op(filter);
    if (options.collation()) {
        delete_op.collation(*options.collation());
    }
    bulk_op.append(delete_op);

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::nullopt;
    }

    return stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

stdx::optional<bsoncxx::document::value> collection::find_one_and_replace(
    view_or_value filter, view_or_value replacement, const options::find_one_and_replace& options) {
    bsoncxx::builder::basic::document command_doc;
    bsoncxx::builder::basic::document options_doc;

    command_doc.append(
        kvp("findAndModify", libmongoc::collection_get_name(_get_impl().collection_t)));

    options_doc.append(kvp("query", filter));

    options_doc.append(kvp("update", replacement.view()));

    if (options.sort()) {
        options_doc.append(kvp("sort", *options.sort()));
    }

    if (options.bypass_document_validation()) {
        options_doc.append(kvp("bypassDocumentValidation", *options.bypass_document_validation()));
    }

    if (options.collation()) {
        options_doc.append(kvp("collation", *options.collation()));
    }

    if (options.projection()) {
        options_doc.append(kvp("fields", *options.projection()));
    }

    if (options.upsert().value_or(false)) {
        options_doc.append(kvp("upsert", *options.upsert()));
    }

    if (options.return_document() == options::return_document::k_after) {
        options_doc.append(kvp("new", true));
    }

    if (options.max_time()) {
        options_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    return find_and_modify(
        _get_impl().collection_t, command_doc.view(), options_doc.view(), options.write_concern());
}

stdx::optional<bsoncxx::document::value> collection::find_one_and_update(
    view_or_value filter, view_or_value update, const options::find_one_and_update& options) {
    bsoncxx::builder::basic::document command_doc;
    bsoncxx::builder::basic::document options_doc;

    command_doc.append(
        kvp("findAndModify", libmongoc::collection_get_name(_get_impl().collection_t)));

    options_doc.append(kvp("query", filter));

    options_doc.append(kvp("update", update));

    if (options.sort()) {
        options_doc.append(kvp("sort", *options.sort()));
    }

    if (options.return_document() == options::return_document::k_after) {
        options_doc.append(kvp("new", true));
    }

    if (options.bypass_document_validation()) {
        options_doc.append(kvp("bypassDocumentValidation", *options.bypass_document_validation()));
    }

    if (options.collation()) {
        options_doc.append(kvp("collation", *options.collation()));
    }

    if (options.projection()) {
        options_doc.append(kvp("fields", *options.projection()));
    }

    if (options.upsert().value_or(false)) {
        options_doc.append(kvp("upsert", *options.upsert()));
    }

    if (options.max_time()) {
        options_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    return find_and_modify(
        _get_impl().collection_t, command_doc.view(), options_doc.view(), options.write_concern());
}

stdx::optional<bsoncxx::document::value> collection::find_one_and_delete(
    view_or_value filter, const options::find_one_and_delete& options) {
    bsoncxx::builder::basic::document command_doc;
    bsoncxx::builder::basic::document options_doc;

    command_doc.append(
        kvp("findAndModify", libmongoc::collection_get_name(_get_impl().collection_t)));

    options_doc.append(kvp("query", filter));

    options_doc.append(kvp("remove", true));

    if (options.sort()) {
        options_doc.append(kvp("sort", *options.sort()));
    }

    if (options.collation()) {
        options_doc.append(kvp("collation", *options.collation()));
    }

    if (options.projection()) {
        options_doc.append(kvp("fields", *options.projection()));
    }

    if (options.max_time()) {
        options_doc.append(kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    if (options.write_concern()) {
        options_doc.append(kvp("writeConcern", options.write_concern()->to_document()));
    }

    return find_and_modify(
        _get_impl().collection_t, command_doc.view(), options_doc.view(), options.write_concern());
}

std::int64_t collection::count(view_or_value filter, const options::count& options) {
    scoped_bson_t bson_filter{filter};
    bson_error_t error;

    const mongoc_read_prefs_t* rp_ptr = NULL;

    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    // Some options must be added via the options struct
    bsoncxx::builder::basic::document cmd_opts_builder;

    if (options.collation()) {
        cmd_opts_builder.append(kvp("collation", *options.collation()));
    }

    if (options.max_time()) {
        cmd_opts_builder.append(
            kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    if (options.hint()) {
        cmd_opts_builder.append(kvp("hint", options.hint()->to_value()));
    }

    scoped_bson_t cmd_opts_bson{cmd_opts_builder.view()};

    auto result = libmongoc::collection_count_with_opts(_get_impl().collection_t,
                                                        static_cast<mongoc_query_flags_t>(0),
                                                        bson_filter.bson(),
                                                        options.skip().value_or(0),
                                                        options.limit().value_or(0),
                                                        cmd_opts_bson.bson(),
                                                        rp_ptr,
                                                        &error);

    if (result < 0) {
        throw_exception<query_exception>(error);
    }

    return result;
}

bsoncxx::document::value collection::create_index(bsoncxx::document::view_or_value keys,
                                                  bsoncxx::document::view_or_value index_opts,
                                                  options::index_view operation_options) {
    using namespace bsoncxx;

    auto name = indexes().create_one(keys, index_opts, operation_options);

    if (name) {
        return make_document(kvp("name", *name));
    } else {
        return make_document();
    }
}

cursor collection::distinct(bsoncxx::string::view_or_value field_name,
                            view_or_value query,
                            const options::distinct& options) {
    //
    // Construct the distinct command and options.
    //
    bsoncxx::builder::basic::document command_builder;
    command_builder.append(kvp("distinct", name()),
                           kvp("key", field_name.view()),
                           kvp("query", bsoncxx::types::b_document{query}));

    if (options.max_time()) {
        command_builder.append(
            kvp("maxTimeMS", bsoncxx::types::b_int64{options.max_time()->count()}));
    }

    bsoncxx::builder::basic::document opts_builder{};
    if (options.collation()) {
        opts_builder.append(kvp("collation", *options.collation()));
    }

    const mongoc_read_prefs_t* rp_ptr = NULL;
    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    //
    // Send the command and validate the reply.
    //
    scoped_bson_t reply;
    bson_error_t error;
    scoped_bson_t command_bson{command_builder.extract()};
    scoped_bson_t opts_bson{opts_builder.extract()};

    auto result = libmongoc::collection_read_command_with_opts(_get_impl().collection_t,
                                                               command_bson.bson(),
                                                               rp_ptr,
                                                               opts_bson.bson(),
                                                               reply.bson_for_init(),
                                                               &error);

    if (!result) {
        throw_exception<operation_exception>(error);
    }

    //
    // Fake a cursor with the reply document as a single result.
    //
    auto fake_db_reply = make_document(
        kvp("ok", 1), kvp("cursor", [&reply](sub_document sub_doc) {
            sub_doc.append(
                kvp("ns", ""), kvp("id", 0), kvp("firstBatch", [&reply](sub_array sub_arr) {
                    sub_arr.append(reply.view());
                }));
        }));

    bson_t* reply_bson =
        bson_new_from_data(fake_db_reply.view().data(), fake_db_reply.view().length());
    if (!reply_bson) {
        throw bsoncxx::exception{bsoncxx::error_code::k_internal_error};
    }

    cursor fake_cursor{
        libmongoc::cursor_new_from_command_reply(_get_impl().client_impl->client_t, reply_bson, 0)};
    if (libmongoc::cursor_error(fake_cursor._impl->cursor_t, &error)) {
        throw_exception<operation_exception>(error);
    }

    return fake_cursor;
}

cursor collection::list_indexes() const {
    return libmongoc::collection_find_indexes_with_opts(_get_impl().collection_t, nullptr);
}

void collection::drop(const stdx::optional<mongocxx::write_concern>& wc) {
    bson_error_t error;

    bsoncxx::builder::basic::document opts_doc;
    if (wc) {
        opts_doc.append(kvp("writeConcern", wc->to_document()));
    }

    scoped_bson_t opts_bson{opts_doc.view()};
    auto result =
        libmongoc::collection_drop_with_opts(_get_impl().collection_t, opts_bson.bson(), &error);

    // Throw an exception if the command failed, unless the failure was due to a non-existent
    // collection. We check for this failure using 'code', but we fall back to checking 'message'
    // for old server versions (3.0 and earlier) that do not send a code with the command response.
    if (!result &&
        !(error.code == ::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST ||
          stdx::string_view{error.message} == stdx::string_view{"ns not found"})) {
        throw_exception<operation_exception>(error);
    }
}

void collection::read_concern(class read_concern rc) {
    libmongoc::collection_set_read_concern(_get_impl().collection_t, rc._impl->read_concern_t);
}

class read_concern collection::read_concern() const {
    auto rc = libmongoc::collection_get_read_concern(_get_impl().collection_t);
    return {stdx::make_unique<read_concern::impl>(libmongoc::read_concern_copy(rc))};
}

void collection::read_preference(class read_preference rp) {
    libmongoc::collection_set_read_prefs(_get_impl().collection_t, rp._impl->read_preference_t);
}

class read_preference collection::read_preference() const {
    class read_preference rp(stdx::make_unique<read_preference::impl>(libmongoc::read_prefs_copy(
        libmongoc::collection_get_read_prefs(_get_impl().collection_t))));
    return rp;
}

void collection::write_concern(class write_concern wc) {
    libmongoc::collection_set_write_concern(_get_impl().collection_t, wc._impl->write_concern_t);
}

class write_concern collection::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(libmongoc::write_concern_copy(
        libmongoc::collection_get_write_concern(_get_impl().collection_t))));
    return wc;
}

template <typename T>
inline void append_if(bsoncxx::builder::basic::document& doc,
                      const std::string& key,
                      const std::optional<T>& opt) {
    if (opt) {
        doc.append(bsoncxx::builder::basic::kvp(key, opt.value()));
    }
}

bsoncxx::document::value as_bson(const options::change_stream& cs) {
    // Construct new bson rep each time since values may change after this is called.
    bsoncxx::builder::basic::document out{};

    append_if(out, "fullDocument", cs.full_document());
    append_if(out, "resumeAfter", cs.resume_after());
    append_if(out, "batchSize", cs.batch_size());

    if (cs.max_await_time()) {
        auto count = cs.max_await_time().value().count();
        if ((count < 0) || (count >= std::numeric_limits<std::uint32_t>::max())) {
            throw logic_error{error_code::k_invalid_parameter};
        }
        out.append(bsoncxx::builder::basic::kvp("maxAwaitTimeMS", count));
    }

    return std::move(out.extract());
}

class change_stream collection::watch(const options::change_stream& options) {
    return watch(pipeline{}, options);
}

class change_stream collection::watch(const pipeline& pipe, const options::change_stream& options) {
    scoped_bson_t pipeline_bson{bsoncxx::document::view(pipe._impl->view_array())};

    scoped_bson_t options_bson{};
    options_bson.init_from_static(as_bson(options));

    // NB: collection_watch copies what it needs so we're safe to destroy our copies.
    return change_stream{libmongoc::collection_watch(
        _get_impl().collection_t, pipeline_bson.bson(), options_bson.bson())};
}

class index_view collection::indexes() {
    return index_view{_get_impl().collection_t};
}

const collection::impl& collection::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_collection_object};
    }
    return *_impl;
}

collection::impl& collection::_get_impl() {
    auto cthis = const_cast<const collection*>(this);
    return const_cast<collection::impl&>(cthis->_get_impl());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
