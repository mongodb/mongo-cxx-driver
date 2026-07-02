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

#include <mongocxx/v1/auto_encryption_options.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <memory>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class auto_encryption_options::internal {
   public:
    static bsoncxx::v1::stdx::optional<ns_pair>& key_vault_namespace(auto_encryption_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& kms_providers(auto_encryption_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& tls_opts(auto_encryption_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& schema_map(auto_encryption_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& encrypted_fields_map(
        auto_encryption_options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& extra_options(auto_encryption_options& self);

    struct mongoc_auto_encryption_opts_deleter {
        void operator()(mongoc_auto_encryption_opts_t* ptr) const noexcept {
            libmongoc::auto_encryption_opts_destroy(ptr);
        }
    };

    static std::unique_ptr<mongoc_auto_encryption_opts_t, mongoc_auto_encryption_opts_deleter> to_mongoc(
        v1::auto_encryption_options const& self);
};

} // namespace v1
} // namespace mongocxx
