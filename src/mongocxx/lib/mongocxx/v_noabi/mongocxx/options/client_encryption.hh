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

#include <mongocxx/options/client_encryption.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/client_encryption.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

class client_encryption::internal {
   public:
    static client_encryption from_v1(v1::client_encryption::options v);
    static v1::client_encryption::options to_v1(client_encryption const& v);

    using opts_deleter = v1::client_encryption::options::internal::opts_deleter;
    using opts_ptr_type = v1::client_encryption::options::internal::opts_ptr_type;

    static opts_ptr_type to_mongoc(client_encryption const& opts);
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
