// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

inline void make_bson_error(bson_error_t* error, const mongocxx::operation_exception& e) {
    // No way to get the domain back out of the exception, so zero out.
    error->code = static_cast<uint32_t>(e.code().value());
    error->domain = 0;
    set_bson_error_message(error, e.what());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>


