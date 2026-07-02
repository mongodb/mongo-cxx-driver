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

#include <mongocxx/v1/server_api.hpp> // IWYU pragma: export

//

#include <memory>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

class server_api::internal {
   public:
    struct mongoc_server_api_deleter {
        void operator()(mongoc_server_api_t* ptr) const noexcept {
            libmongoc::server_api_destroy(ptr);
        }
    };

    static std::unique_ptr<mongoc_server_api_t, mongoc_server_api_deleter> to_mongoc(v1::server_api const& self);
};

} // namespace v1
} // namespace mongocxx
