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

#include <mongocxx/v1/indexes.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class indexes::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(indexes) make(mongoc_collection_t* coll, mongoc_client_t* client);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_collection_t const*) get_collection(indexes const& self);
    static mongoc_client_t const* get_client(indexes const& self);

    static mongoc_collection_t* get_collection(indexes& self);
    static mongoc_client_t* get_client(indexes& self);
};

class indexes::options::internal {
   public:
    static bsoncxx::v1::stdx::optional<v1::write_concern> const& write_concern(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& commit_quorum(options const& self);

    static bsoncxx::v1::stdx::optional<v1::write_concern>& write_concern(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& commit_quorum(options& self);

    static bsoncxx::v1::stdx::optional<std::string>& name(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& storage_engine(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& weights(options& self);
    static bsoncxx::v1::stdx::optional<std::string>& default_language(options& self);
    static bsoncxx::v1::stdx::optional<std::string>& language_override(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& partial_filter_expression(options& self);
};

} // namespace v1
} // namespace mongocxx
