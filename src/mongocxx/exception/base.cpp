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

#include <mongocxx/config/prelude.hpp>

#include <string>
#include <utility>

#include <mongocxx/exception/base.hpp>
#include <mongocxx/exception/error_category.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

base::base(std::error_code ec) : _ec{ec} {
}

base::base(std::error_code ec, std::string what_arg) : _ec{ec}, _what{std::move(what_arg)} {
}

base::base(bsoncxx::document::value raw_server_error, std::error_code ec)
    : _raw_server_error{std::move(raw_server_error)}, _ec{ec} {
}

base::base(bsoncxx::document::value raw_server_error, std::error_code ec, std::string what_arg)
    : _raw_server_error{std::move(raw_server_error)}, _ec{ec}, _what{std::move(what_arg)} {
}

const stdx::optional<bsoncxx::document::value>& base::raw_server_error() const {
    return _raw_server_error;
}

stdx::optional<bsoncxx::document::value>& base::raw_server_error() {
    return _raw_server_error;
}

const std::error_code& base::code() const {
    return _ec;
}

const char* base::what() const noexcept {
    return _what ? _what->c_str() : "";
}

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
