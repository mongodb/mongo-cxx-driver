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

#include <mongocxx/v1/data_key_options.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <memory>
#include <string>
#include <vector>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class data_key_options::internal {
   public:
    struct deleter_type {
        void operator()(mongoc_client_encryption_datakey_opts_t* opts) const noexcept {
            libmongoc::client_encryption_datakey_opts_destroy(opts);
        }
    };

    using unique_ptr_type = std::unique_ptr<mongoc_client_encryption_datakey_opts_t, deleter_type>;

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(unique_ptr_type) to_mongoc(data_key_options const& self);

    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& master_key(data_key_options& self);
    static std::vector<std::string>& key_alt_names(data_key_options& self);
    static bsoncxx::v1::stdx::optional<key_material_type>& key_material(data_key_options& self);
};

} // namespace v1
} // namespace mongocxx

///
/// @file
/// Provides @ref mongocxx::v1::data_key_options.
///
