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

#include <mongocxx/v1/search_indexes.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/cursor-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

#include <bson/bson_t.h>

namespace mongocxx {
namespace v1 {

class search_indexes::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(search_indexes) make(mongoc_collection_t* coll);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) is_moved_from(search_indexes const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_collection_t const*) get_collection(search_indexes const& self);
    static mongoc_collection_t* get_collection(search_indexes& self);

    static v1::cursor
    list_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts, mongoc_read_prefs_t const* read_prefs);

    static std::string create_one_impl(mongoc_collection_t* coll, bson_t const* index, bson_t const* opts);

    static void drop_one_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts);

    static void update_one_impl(
        mongoc_collection_t* coll,
        char const* name,
        bsoncxx::v1::document::view definition,
        bson_t const* opts);
};

class search_indexes::model::internal {
   public:
    static bsoncxx::v1::stdx::optional<std::string> const& name(model const& self);
    static bsoncxx::v1::document::value const& definition(model const& self);
    static bsoncxx::v1::stdx::optional<std::string> const& type(model const& self);

    static bsoncxx::v1::stdx::optional<std::string>& name(model& self);
    static bsoncxx::v1::document::value& definition(model& self);
    static bsoncxx::v1::stdx::optional<std::string>& type(model& self);
};

} // namespace v1
} // namespace mongocxx
