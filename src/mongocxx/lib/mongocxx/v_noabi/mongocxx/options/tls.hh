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

#include <mongocxx/options/tls.hpp> // IWYU pragma: export

//

#include <mongocxx/private/ssl.hh>

#if MONGOCXX_SSL_IS_ENABLED()
#include <list>

#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/private/mongoc.hh>
#endif

namespace mongocxx {
namespace v_noabi {
namespace options {

class tls::internal {
   public:
#if MONGOCXX_SSL_IS_ENABLED()
    struct to_mongoc_type {
        std::list<bsoncxx::v_noabi::string::view_or_value> string_owner;

        mongoc_ssl_opt_t opt;
    };

    static to_mongoc_type to_mongoc(tls const& opts);
#endif
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
