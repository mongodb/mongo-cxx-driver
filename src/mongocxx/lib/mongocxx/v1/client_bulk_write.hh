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
#include <mongocxx/v1/client_bulk_write.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class client_bulk_write::options::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bool> const&) bypass_document_validation(
        options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const&) comment(
        options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const&) let(
        options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bool> const&) ordered(options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bool> const&) verbose_results(
        options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<v1::write_concern> const&) write_concern(
        options const& self);
};

class client_bulk_write::result::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(result) make(mongoc_bulkwriteresult_t* res);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(result) make();

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int64_t&) inserted_count(result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int64_t&) upserted_count(result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int64_t&) matched_count(result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int64_t&) modified_count(result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(std::int64_t&) deleted_count(result& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>&) insert_results(
        result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>&) update_results(
        result& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>&) delete_results(
        result& self);
};

class client_bulk_write::exception::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(exception) make(
        mongoc_bulkwriteexception_t* exc,
        bsoncxx::v1::stdx::optional<result> partial_result);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(exception) make();

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) write_errors(exception& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::array::value&) write_concern_errors(exception& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::value&) error_reply(exception& self);
};

class client_bulk_write::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(client_bulk_write) make(mongoc_bulkwrite_t* bulk);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_bulkwrite_t*) as_mongoc(client_bulk_write& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) set_session(client_bulk_write& self, v1::client_session& session);
};

} // namespace v1
} // namespace mongocxx
