// Copyright 2017-present MongoDB Inc.
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

#include <mongocxx/options/client_session.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

client_session& client_session::causal_consistency(bool causal_consistency) noexcept {
    _causal_consistency = causal_consistency;
    return *this;
}

bool client_session::causal_consistency() const noexcept {
    return _causal_consistency;
}

client_session& client_session::default_transaction_opts(transaction default_transaction_opts) {
    _default_transaction_opts = std::move(default_transaction_opts);
    return *this;
}

const stdx::optional<transaction>& client_session::default_transaction_opts() const {
    return _default_transaction_opts;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
