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

#pragma once
#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/hint-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {

class bulk_write::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bulk_write) make(mongoc_bulk_operation_t* bulk);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_bulk_operation_t*) as_mongoc(bulk_write& self);

    static bool& is_empty(bulk_write& self);
};

class bulk_write::update_one::internal {
   public:
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& hint(update_one const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) filter(update_one& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) update(update_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(update_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value>& array_filters(update_one& self);
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint>& hint(update_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& sort(update_one& self);
};

class bulk_write::update_many::internal {
   public:
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& hint(update_many const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) filter(update_many& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) update(update_many& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(update_many& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value>& array_filters(update_many& self);
    static bsoncxx::v1::stdx::optional<bool>& upsert(update_many& self);
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint>& hint(update_many& self);
};

class bulk_write::replace_one::internal {
   public:
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& hint(replace_one const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) filter(replace_one& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) replacement(replace_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(replace_one& self);
    static bsoncxx::v1::stdx::optional<v1::hint>& hint(replace_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& sort(replace_one& self);
};

class bulk_write::delete_one::internal {
   public:
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& hint(delete_one const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) filter(delete_one& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(delete_one& self);
    static bsoncxx::v1::stdx::optional<v1::hint>& hint(delete_one& self);
};

class bulk_write::delete_many::internal {
   public:
    static bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& hint(delete_many const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) filter(delete_many& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(delete_many& self);
    static bsoncxx::v1::stdx::optional<v1::hint>& hint(delete_many& self);
};

class bulk_write::single::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(insert_one&) get_insert_one(single& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(update_one&) get_update_one(single& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(update_many&) get_update_many(single& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(delete_one&) get_delete_one(single& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(delete_many&) get_delete_many(single& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(replace_one&) get_replace_one(single& self);
};

class bulk_write::options::internal {
   public:
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& comment(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& let(options const& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern(options const& self);

    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& comment(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& let(options& self);
    static bsoncxx::v1::stdx::optional<v1::write_concern>& write_concern(options& self);

    static void append_to(options const& self, scoped_bson& doc);
};

class bulk_write::result::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(result) make(bsoncxx::v1::document::value reply);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::view) reply(result const& self);
};

} // namespace v1
} // namespace mongocxx
