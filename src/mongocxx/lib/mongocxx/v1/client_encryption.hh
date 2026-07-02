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

#include <mongocxx/v1/client_encryption.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <memory>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class client_encryption::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(client_encryption) make(mongoc_client_encryption_t* ptr);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(mongoc_client_encryption_t*) as_mongoc(client_encryption const& self);
};

class client_encryption::options::internal {
   public:
    struct opts_deleter {
        void operator()(mongoc_client_encryption_opts_t* ptr) noexcept {
            libmongoc::client_encryption_opts_destroy(ptr);
        }
    };

    using opts_ptr_type = std::unique_ptr<mongoc_client_encryption_opts_t, opts_deleter>;

    static bsoncxx::v1::stdx::optional<ns_pair> const& key_vault_namespace(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& kms_providers(options const& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& tls_opts(options const& self);

    static bsoncxx::v1::stdx::optional<ns_pair>& key_vault_namespace(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& kms_providers(options& self);
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& tls_opts(options& self);

    static opts_ptr_type to_mongoc(client_encryption::options const& opts);
};

} // namespace v1
} // namespace mongocxx
