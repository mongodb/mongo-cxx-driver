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

#include <mongocxx/bulk_write.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/bulk_write.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/private/write_concern.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using namespace libbson;

bulk_write::bulk_write(bulk_write&&) noexcept = default;
bulk_write& bulk_write::operator=(bulk_write&&) noexcept = default;

bulk_write::~bulk_write() = default;

bulk_write::bulk_write(options::bulk_write options)
    : _impl(stdx::make_unique<impl>(libmongoc::bulk_operation_new(options.ordered()))) {
    auto options_wc = options.write_concern();
    if (options_wc)
        libmongoc::bulk_operation_set_write_concern(_impl->operation_t,
                                                    options_wc->_impl->write_concern_t);

    auto options_bdv = options.bypass_document_validation();
    if (options_bdv)
        libmongoc::bulk_operation_set_bypass_document_validation(_impl->operation_t, *options_bdv);
}

void bulk_write::append(const model::write& operation) {
    switch (operation.type()) {
        case write_type::k_insert_one: {
            scoped_bson_t doc(operation.get_insert_one().document());

            libmongoc::bulk_operation_insert(_impl->operation_t, doc.bson());
            break;
        }
        case write_type::k_update_one: {
            scoped_bson_t filter(operation.get_update_one().filter());
            scoped_bson_t update(operation.get_update_one().update());
            bool upsert = operation.get_update_one().upsert().value_or(false);

            libmongoc::bulk_operation_update_one(_impl->operation_t, filter.bson(), update.bson(),
                                                 upsert);
            break;
        }
        case write_type::k_update_many: {
            scoped_bson_t filter(operation.get_update_many().filter());
            scoped_bson_t update(operation.get_update_many().update());
            bool upsert = operation.get_update_many().upsert().value_or(false);

            libmongoc::bulk_operation_update(_impl->operation_t, filter.bson(), update.bson(),
                                             upsert);
            break;
        }
        case write_type::k_delete_one: {
            scoped_bson_t filter(operation.get_delete_one().filter());
            libmongoc::bulk_operation_remove_one(_impl->operation_t, filter.bson());
            break;
        }
        case write_type::k_delete_many: {
            scoped_bson_t filter(operation.get_delete_many().filter());
            libmongoc::bulk_operation_remove(_impl->operation_t, filter.bson());
            break;
        }
        case write_type::k_replace_one: {
            scoped_bson_t filter(operation.get_replace_one().filter());
            scoped_bson_t replace(operation.get_replace_one().replacement());
            bool upsert = operation.get_replace_one().upsert().value_or(false);

            libmongoc::bulk_operation_replace_one(_impl->operation_t, filter.bson(), replace.bson(),
                                                  upsert);
            break;
        }
    }
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
