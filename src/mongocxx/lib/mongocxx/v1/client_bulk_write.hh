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
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/export.hh>

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
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) ordered(options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bool> const&) verbose_results(
        options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<v1::write_concern> const&) write_concern(
        options const& self);
};

} // namespace v1
} // namespace mongocxx
