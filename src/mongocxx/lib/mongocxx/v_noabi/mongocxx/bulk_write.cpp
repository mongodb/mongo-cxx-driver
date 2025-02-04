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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/bulk_write.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/collection.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/mongoc_error.hh>
#include <mongocxx/private/write_concern.hh>

namespace mongocxx {
namespace v_noabi {

using namespace libbson;
using bsoncxx::v_noabi::builder::basic::kvp;

bulk_write::bulk_write(bulk_write&&) noexcept = default;
bulk_write& bulk_write::operator=(bulk_write&&) noexcept = default;

bulk_write::~bulk_write() = default;

bool bulk_write::empty() const noexcept {
    return _impl->is_empty;
}

bulk_write& bulk_write::append(model::write const& operation) {
    switch (operation.type()) {
        case write_type::k_insert_one: {
            scoped_bson_t doc(operation.get_insert_one().document());
            bson_error_t error;
            auto result = libmongoc::bulk_operation_insert_with_opts(_impl->operation_t, doc.bson(), nullptr, &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
        case write_type::k_update_one: {
            scoped_bson_t filter(operation.get_update_one().filter());
            scoped_bson_t update(operation.get_update_one().update());

            bsoncxx::v_noabi::builder::basic::document options_builder;
            if (auto const collation = operation.get_update_one().collation()) {
                options_builder.append(kvp("collation", *collation));
            }
            if (auto const hint = operation.get_update_one().hint()) {
                options_builder.append(kvp("hint", *hint));
            }
            if (auto const sort = operation.get_update_one().sort()) {
                options_builder.append(kvp("sort", *sort));
            }
            if (auto const upsert = operation.get_update_one().upsert()) {
                options_builder.append(kvp("upsert", *upsert));
            }
            if (auto const array_filters = operation.get_update_one().array_filters()) {
                options_builder.append(kvp("arrayFilters", *array_filters));
            }
            scoped_bson_t options(options_builder.extract());

            bson_error_t error;
            auto result = libmongoc::bulk_operation_update_one_with_opts(
                _impl->operation_t, filter.bson(), update.bson(), options.bson(), &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
        case write_type::k_update_many: {
            scoped_bson_t filter(operation.get_update_many().filter());
            scoped_bson_t update(operation.get_update_many().update());

            bsoncxx::v_noabi::builder::basic::document options_builder;
            if (auto const collation = operation.get_update_many().collation()) {
                options_builder.append(kvp("collation", *collation));
            }
            if (auto const hint = operation.get_update_many().hint()) {
                options_builder.append(kvp("hint", *hint));
            }
            if (auto const upsert = operation.get_update_many().upsert()) {
                options_builder.append(kvp("upsert", *upsert));
            }
            if (auto const array_filters = operation.get_update_many().array_filters()) {
                options_builder.append(kvp("arrayFilters", *array_filters));
            }
            scoped_bson_t options(options_builder.extract());

            bson_error_t error;
            auto result = libmongoc::bulk_operation_update_many_with_opts(
                _impl->operation_t, filter.bson(), update.bson(), options.bson(), &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
        case write_type::k_delete_one: {
            scoped_bson_t filter(operation.get_delete_one().filter());

            bsoncxx::v_noabi::builder::basic::document options_builder;
            if (auto const collation = operation.get_delete_one().collation()) {
                options_builder.append(kvp("collation", *collation));
            }
            if (auto const hint = operation.get_delete_one().hint()) {
                options_builder.append(kvp("hint", *hint));
            }
            scoped_bson_t options(options_builder.extract());

            bson_error_t error;
            auto result = libmongoc::bulk_operation_remove_one_with_opts(
                _impl->operation_t, filter.bson(), options.bson(), &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
        case write_type::k_delete_many: {
            scoped_bson_t filter(operation.get_delete_many().filter());

            bsoncxx::v_noabi::builder::basic::document options_builder;
            if (auto const collation = operation.get_delete_many().collation()) {
                options_builder.append(kvp("collation", *collation));
            }
            if (auto const hint = operation.get_delete_many().hint()) {
                options_builder.append(kvp("hint", *hint));
            }
            scoped_bson_t options(options_builder.extract());

            bson_error_t error;
            auto result = libmongoc::bulk_operation_remove_many_with_opts(
                _impl->operation_t, filter.bson(), options.bson(), &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
        case write_type::k_replace_one: {
            scoped_bson_t filter(operation.get_replace_one().filter());
            scoped_bson_t replace(operation.get_replace_one().replacement());

            bsoncxx::v_noabi::builder::basic::document options_builder;
            if (auto const collation = operation.get_replace_one().collation()) {
                options_builder.append(kvp("collation", *collation));
            }
            if (auto const hint = operation.get_replace_one().hint()) {
                options_builder.append(kvp("hint", *hint));
            }
            if (auto const sort = operation.get_replace_one().sort()) {
                options_builder.append(kvp("sort", *sort));
            }
            if (auto const upsert = operation.get_replace_one().upsert()) {
                options_builder.append(kvp("upsert", *upsert));
            }
            scoped_bson_t options(options_builder.extract());

            bson_error_t error;
            auto result = libmongoc::bulk_operation_replace_one_with_opts(
                _impl->operation_t, filter.bson(), replace.bson(), options.bson(), &error);
            if (!result) {
                throw_exception<logic_error>(error);
            }
            break;
        }
    }

    _impl->is_empty = false;

    return *this;
}

bsoncxx::v_noabi::stdx::optional<result::bulk_write> bulk_write::execute() const {
    mongoc_bulk_operation_t* b = _impl->operation_t;
    scoped_bson_t reply;
    bson_error_t error;

    if (!libmongoc::bulk_operation_execute(b, reply.bson_for_init(), &error)) {
        throw_exception<bulk_write_exception>(reply.steal(), error);
    }

    // Reply is empty for unacknowledged writes, so return disengaged optional.
    if (reply.view().empty()) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    result::bulk_write result(reply.steal());

    return bsoncxx::v_noabi::stdx::optional<result::bulk_write>(std::move(result));
}

bulk_write::bulk_write(collection const& coll, options::bulk_write const& options, client_session const* session)
    : _created_from_collection{true} {
    bsoncxx::v_noabi::builder::basic::document options_builder;
    if (!options.ordered()) {
        // ordered is true by default. Only append it if set to false.
        options_builder.append(kvp("ordered", false));
    }
    if (auto const& wc = options.write_concern()) {
        options_builder.append(kvp("writeConcern", wc->to_document()));
    }
    if (auto const& let = options.let()) {
        options_builder.append(kvp("let", *let));
    }
    if (auto const& comment = options.comment()) {
        options_builder.append(kvp("comment", *comment));
    }
    if (session) {
        options_builder.append(bsoncxx::v_noabi::builder::concatenate_doc{session->_get_impl().to_document()});
    }

    scoped_bson_t bson_options(options_builder.extract());
    _impl = bsoncxx::make_unique<bulk_write::impl>(
        libmongoc::collection_create_bulk_operation_with_opts(coll._get_impl().collection_t, bson_options.bson()));

    if (auto validation = options.bypass_document_validation()) {
        libmongoc::bulk_operation_set_bypass_document_validation(_impl->operation_t, *validation);
    }
}

} // namespace v_noabi
} // namespace mongocxx
