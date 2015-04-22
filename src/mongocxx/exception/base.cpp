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

#include <mongocxx/exception/base.hpp>

#include <utility>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

base::base(bsoncxx::document::value raw_server_error)
    : _raw_server_error(std::move(raw_server_error))
{}

base::base(error_and_code_type error_and_code)
    : _error_and_code(std::move(error_and_code))
{}

base::base(bsoncxx::document::value raw_server_error,
           error_and_code_type error_and_code)
    : _raw_server_error(std::move(raw_server_error))
    , _error_and_code(std::move(error_and_code))
{}

const stdx::optional<bsoncxx::document::value>& base::raw_server_error() const {
    return _raw_server_error;
}

stdx::optional<bsoncxx::document::value>& base::raw_server_error() {
    return _raw_server_error;
}

const stdx::optional<error_and_code_type>& base::error_and_code() const {
    return _error_and_code;
}

stdx::optional<error_and_code_type>& base::error_and_code() {
    return _error_and_code;
}

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
