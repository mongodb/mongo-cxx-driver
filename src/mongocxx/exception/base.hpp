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

#pragma once

#include <mongocxx/config/prelude.hpp>

#include <exception>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

class MONGOCXX_API base : virtual public std::exception {
   public:
    explicit base(std::error_code ec, std::string what_arg = "");

    explicit base(std::error_code ec, bsoncxx::document::value raw_server_error,
                  std::string what_arg = "");

    ///
    /// @returns The raw server error, if it is available.
    ///
    const stdx::optional<bsoncxx::document::value>& raw_server_error() const;
    stdx::optional<bsoncxx::document::value>& raw_server_error();

    const std::error_code& code() const;

    const char* what() const noexcept override;

   private:
    stdx::optional<bsoncxx::document::value> _raw_server_error;
    std::error_code _ec;
    stdx::optional<std::string> _what;
};

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
