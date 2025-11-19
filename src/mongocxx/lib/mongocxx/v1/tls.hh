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

#include <mongocxx/v1/tls.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

class tls::internal {
   public:
    static bsoncxx::v1::stdx::optional<std::string>& pem_file(tls& self);
    static bsoncxx::v1::stdx::optional<std::string>& pem_password(tls& self);
    static bsoncxx::v1::stdx::optional<std::string>& ca_file(tls& self);
    static bsoncxx::v1::stdx::optional<std::string>& ca_dir(tls& self);
    static bsoncxx::v1::stdx::optional<std::string>& crl_file(tls& self);
};

} // namespace v1
} // namespace mongocxx
