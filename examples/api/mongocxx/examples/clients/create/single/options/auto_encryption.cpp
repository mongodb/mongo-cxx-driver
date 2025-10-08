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

#include <cstdint>
#include <cstring>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/auto_encryption.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(bsoncxx::document::view kms_providers) {
    mongocxx::options::auto_encryption auto_encryption_opts;

    auto_encryption_opts.key_vault_namespace({"keyvault", "datakeys"});
    auto_encryption_opts.kms_providers(kms_providers);
    auto_encryption_opts.extra_options(bsoncxx::from_json(
        R"({"mongocryptdURI": "mongodb://localhost:27027", "mongocryptdSpawnArgs": ["--port", "27027"]})"));
    // ... other automatic encryption options.

    mongocxx::options::client client_opts;
    client_opts.auto_encryption_opts(auto_encryption_opts);

    mongocxx::uri uri;
    mongocxx::client client{uri, client_opts};

    EXPECT(client);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    try {
        std::uint8_t local_key[96]{};

        example(make_document(kvp(
            "local",
            make_document(kvp("key", bsoncxx::types::b_binary{bsoncxx::binary_sub_type::k_binary, 96, local_key})))));
    } catch (mongocxx::exception const& ex) {
        if (std::strstr(ex.what(), "ENABLE_CLIENT_SIDE_ENCRYPTION") != nullptr) {
            // Library may not be configured with TLS/SSL support enabled.
        } else if (std::strstr(ex.what(), "mongocryptd") != nullptr) {
            // Environment may not support spawning mongocryptd.
        } else {
            throw;
        }
    }
}
