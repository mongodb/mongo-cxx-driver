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

#include <mongocxx/v1/change_stream.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class change_stream::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(change_stream) make(mongoc_change_stream_t* stream);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::document::view) doc(change_stream const& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) has_doc(change_stream const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) can_get_more(change_stream const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) is_dead(change_stream const& self);

    static void advance_iterator(change_stream& self);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_change_stream_t const*) as_mongoc(change_stream const& self);
};

class change_stream::options::internal {
   public:
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& collation(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& comment(options const& self);
    static bsoncxx::v1::stdx::optional<std::string> const& full_document(options const& self);
    static bsoncxx::v1::stdx::optional<std::string> const& full_document_before_change(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& resume_after(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& start_after(options const& self);

    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& collation(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& comment(options& self);
    static bsoncxx::v1::stdx::optional<std::string>& full_document(options& self);
    static bsoncxx::v1::stdx::optional<std::string>& full_document_before_change(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& resume_after(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& start_after(options& self);

    static bsoncxx::v1::document::value to_document(options const& self);
};

class change_stream::iterator::internal {
   public:
    static iterator make(change_stream* self);

    static change_stream::impl* with(iterator* self);
};

} // namespace v1
} // namespace mongocxx
