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

#include <mongocxx/v1/encrypt_options.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/value-fwd.hpp>

#include <mongocxx/v1/range_options-fwd.hpp>
#include <mongocxx/v1/text_options-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {

class encrypt_options::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const&) key_id(
        encrypt_options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<std::string> const&) key_alt_name(
        encrypt_options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<encryption_algorithm> const&) algorithm(
        encrypt_options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<v1::range_options> const&) range_opts(
        encrypt_options const& self);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(bsoncxx::v1::stdx::optional<v1::text_options> const&) text_opts(
        encrypt_options const& self);

    static bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& key_id(encrypt_options& self);
    static bsoncxx::v1::stdx::optional<std::string>& key_alt_name(encrypt_options& self);
    static bsoncxx::v1::stdx::optional<encryption_algorithm>& algorithm(encrypt_options& self);
    static bsoncxx::v1::stdx::optional<v1::range_options>& range_opts(encrypt_options& self);
    static bsoncxx::v1::stdx::optional<v1::text_options>& text_opts(encrypt_options& self);
};

} // namespace v1
} // namespace mongocxx
