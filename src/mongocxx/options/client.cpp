// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/options/client.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

client& client::ssl_opts(ssl ssl_opts) {
    _ssl_opts = std::move(ssl_opts);
    return *this;
}

const stdx::optional<ssl>& client::ssl_opts() const {
    return _ssl_opts;
}

client& client::apm_opts(apm apm_opts) {
    _apm_opts = std::move(apm_opts);
    return *this;
}

const stdx::optional<apm>& client::apm_opts() const {
    return _apm_opts;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
