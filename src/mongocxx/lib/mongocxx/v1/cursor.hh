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

#include <mongocxx/v1/cursor.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class cursor::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(cursor) make(mongoc_cursor_t* cursor, type type);

    static cursor make(mongoc_cursor_t* cursor) {
        return make(cursor, type::k_non_tailable);
    }

    static cursor make(mongoc_cursor_t* cursor, bsoncxx::v1::stdx::optional<type> type_opt) {
        return make(cursor, type_opt ? *type_opt : type::k_non_tailable);
    }

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::view) doc(cursor const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) has_doc(cursor const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) can_get_more(cursor const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) is_dead(cursor const& self);

    static void advance_iterator(cursor& self);

    static mongoc_cursor_t* as_mongoc(cursor& self);
};

class cursor::iterator::internal {
   public:
    static iterator make(cursor* self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(cursor const*) get_cursor(iterator const& self);
};

} // namespace v1
} // namespace mongocxx
