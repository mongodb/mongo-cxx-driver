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

#include <mongocxx/options/auto_encryption.hpp> // IWYU pragma: export

//

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

class auto_encryption::internal {
   public:
    static auto_encryption from_v1(v1::auto_encryption_options v);
    static v1::auto_encryption_options to_v1(auto_encryption const& v);

    static mongoc_auto_encryption_opts_t* to_mongoc(auto_encryption const& opts);
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
