// Copyright 2020 MongoDB Inc.
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

#include <mongocxx/client_encryption.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/client_encryption.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using mongocxx::libbson::scoped_bson_t;

client_encryption::client_encryption(options::client_encryption opts)
    : _impl(stdx::make_unique<impl>(std::move(opts))) {}

client_encryption::~client_encryption() noexcept = default;

bsoncxx::types::bson_value::value client_encryption::create_data_key(
    std::string kms_provider, const options::data_key& opts) {
    return _impl->create_data_key(kms_provider, opts);
}

bsoncxx::types::bson_value::value client_encryption::encrypt(bsoncxx::types::bson_value::view value,
                                                             const options::encrypt& opts) {
    return _impl->encrypt(value, opts);
}

bsoncxx::types::bson_value::value client_encryption::decrypt(
    bsoncxx::types::bson_value::view value) {
    return _impl->decrypt(value);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
