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

#include <mongocxx/exception/operation_exception.hpp>

#include <string>
#include <utility>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

operation_exception::operation_exception(std::error_code ec,
                                         bsoncxx::document::value&& raw_server_error,
                                         std::string what_arg)
    : exception(ec, what_arg), _raw_server_error{std::move(raw_server_error)} {
}

const stdx::optional<bsoncxx::document::value>& operation_exception::raw_server_error() const {
    return _raw_server_error;
}

stdx::optional<bsoncxx::document::value>& operation_exception::raw_server_error() {
    return _raw_server_error;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
