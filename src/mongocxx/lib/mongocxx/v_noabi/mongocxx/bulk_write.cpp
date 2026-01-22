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

#include <mongocxx/bulk_write.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/bulk_write.hh>

#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/model/delete_many.hpp>
#include <mongocxx/model/delete_one.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/model/replace_one.hpp>
#include <mongocxx/model/update_many.hpp>
#include <mongocxx/model/update_one.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/write_type.hpp>

#include <mongocxx/client_session.hh>
#include <mongocxx/collection.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

template <typename Op>
void append_collation(scoped_bson& options, Op const& op) {
    if (auto const opt = op.collation()) {
        auto const doc = to_scoped_bson_view(opt->view());
        options += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(doc.bson()))};
    }
}

template <typename Op>
void append_hint(scoped_bson& options, Op const& op) {
    namespace builder = bsoncxx::v_noabi::builder::basic;

    if (auto const& opt = op.hint()) {
        builder::document doc;
        doc.append(builder::kvp("hint", opt->to_value()));
        options += to_scoped_bson_view(doc.view());
    }
}

template <typename Op>
void append_sort(scoped_bson& options, Op const& op) {
    if (auto const opt = op.sort()) {
        auto const doc = to_scoped_bson_view(*opt);
        options += scoped_bson{BCON_NEW("sort", BCON_DOCUMENT(doc.bson()))};
    }
}

template <typename Op>
void append_upsert(scoped_bson& options, Op const& op) {
    if (auto const opt = op.upsert()) {
        options += scoped_bson{BCON_NEW("upsert", BCON_BOOL(*opt))};
    }
}

template <typename Op>
void append_array_filters(scoped_bson& options, Op const& op) {
    if (auto const opt = op.array_filters()) {
        auto const doc = to_scoped_bson_view(*opt);
        options += scoped_bson{BCON_NEW("arrayFilters", BCON_ARRAY(doc.bson()))};
    }
}

void append_insert_one(mongoc_bulk_operation_t* bulk, v_noabi::model::insert_one const& op) {
    bson_error_t error = {};

    if (!libmongoc::bulk_operation_insert_with_opts(bulk, to_scoped_bson_view(op.document()).bson(), nullptr, &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

void append_delete_one(mongoc_bulk_operation_t* bulk, v_noabi::model::delete_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_remove_one_with_opts(
            bulk, to_scoped_bson_view(op.filter()).bson(), options.bson(), &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

void append_delete_many(mongoc_bulk_operation_t* bulk, v_noabi::model::delete_many const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_remove_many_with_opts(
            bulk, to_scoped_bson_view(op.filter()).bson(), options.bson(), &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

void append_update_one(mongoc_bulk_operation_t* bulk, v_noabi::model::update_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_sort(options, op);
    append_upsert(options, op);
    append_array_filters(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_update_one_with_opts(
            bulk,
            to_scoped_bson_view(op.filter()).bson(),
            to_scoped_bson_view(op.update()).bson(),
            options.bson(),
            &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

void append_update_many(mongoc_bulk_operation_t* bulk, v_noabi::model::update_many const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_upsert(options, op);
    append_array_filters(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_update_many_with_opts(
            bulk,
            to_scoped_bson_view(op.filter()).bson(),
            to_scoped_bson_view(op.update()).bson(),
            options.bson(),
            &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

void append_replace_one(mongoc_bulk_operation_t* bulk, v_noabi::model::replace_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_sort(options, op);
    append_upsert(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_replace_one_with_opts(
            bulk,
            to_scoped_bson_view(op.filter()).bson(),
            to_scoped_bson_view(op.replacement()).bson(),
            options.bson(),
            &error)) {
        throw_exception<v_noabi::logic_error>(error);
    }
}

} // namespace

bulk_write& bulk_write::append(v_noabi::model::write const& op) {
    auto const bulk = v1::bulk_write::internal::as_mongoc(_bulk);

    switch (op.type()) {
        case v_noabi::write_type::k_insert_one:
            append_insert_one(bulk, op.get_insert_one());
            break;
        case v_noabi::write_type::k_delete_one:
            append_delete_one(bulk, op.get_delete_one());
            break;
        case v_noabi::write_type::k_delete_many:
            append_delete_many(bulk, op.get_delete_many());
            break;
        case v_noabi::write_type::k_update_one:
            append_update_one(bulk, op.get_update_one());
            break;
        case v_noabi::write_type::k_update_many:
            append_update_many(bulk, op.get_update_many());
            break;
        case v_noabi::write_type::k_replace_one:
            append_replace_one(bulk, op.get_replace_one());
            break;
        default:
            // Ignore.
            break;
    }

    v1::bulk_write::internal::is_empty(_bulk) = false;

    return *this;
}

bsoncxx::v_noabi::stdx::optional<result::bulk_write> bulk_write::execute() const try {
    // Backward compatibility: `execute()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<v1::bulk_write&>(_bulk).execute();
} catch (v1::exception const& ex) {
    throw_exception<v_noabi::bulk_write_exception>(ex);
}

bulk_write
bulk_write::internal::make(collection const& coll, options::bulk_write const& opts, client_session const* session) {
    scoped_bson options;

    if (!opts.ordered()) {
        // ordered is true by default. Only append it if set to false.
        options += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    if (auto const& opt = opts.write_concern()) {
        auto const v = opt->to_document();
        options += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson_view(v).bson()))};
    }

    if (auto const& opt = opts.let()) {
        auto const v = opt->view();
        options += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(v).bson()))};
    }

    if (auto const& opt = opts.comment()) {
        namespace builder = bsoncxx::v_noabi::builder::basic;
        builder::document doc;
        doc.append(builder::kvp("comment", opt->view()));
        options += to_scoped_bson_view(doc.view());
    }

    if (session) {
        v_noabi::client_session::internal::append_to(*session, options);
    }

    auto ret = v1::bulk_write::internal::make(
        libmongoc::collection_create_bulk_operation_with_opts(coll._get_impl().collection_t, options.bson()));

    if (auto const validation = opts.bypass_document_validation()) {
        libmongoc::bulk_operation_set_bypass_document_validation(v1::bulk_write::internal::as_mongoc(ret), *validation);
    }

    return bulk_write{std::move(ret)};
}

} // namespace v_noabi
} // namespace mongocxx
