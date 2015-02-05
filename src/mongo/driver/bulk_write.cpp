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

#include <mongo/driver/bulk_write.hpp>

#include <mongo/bson/libbson.hpp>
#include <mongo/driver/private/bulk_write.hpp>
#include <mongo/driver/private/write_concern.hpp>
#include <mongo/driver/private/libmongoc.hpp>
#include <mongo/bson/stdx/make_unique.hpp>

namespace mongo {
namespace driver {

using namespace bson::libbson;

bulk_write::bulk_write(bulk_write&&) noexcept = default;
bulk_write& bulk_write::operator=(bulk_write&&) noexcept = default;

bulk_write::~bulk_write() = default;

bulk_write::bulk_write(bool ordered)
    : _impl(stdx::make_unique<impl>(libmongoc::bulk_operation_new(ordered))) {
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
        case write_type::k_uninitialized:
            break;  // TODO: something exceptiony
    }
}

void bulk_write::write_concern(class write_concern wc) {
    libmongoc::bulk_operation_set_write_concern(_impl->operation_t, wc._impl->write_concern_t);
}

// class write_concern bulk_write::write_concern() const {
// class write_concern wc(stdx::make_unique<write_concern::impl>(
// libmongoc::write_concern_copy(
// libmongoc::bulk_operation_get_write_concern(_impl->operation_t)
//)
//));
// return wc;
//}

}  // namespace driver
}  // namespace mongo
