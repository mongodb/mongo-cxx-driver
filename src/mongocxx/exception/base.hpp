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

#include <cstdint>
#include <exception>
#include <tuple>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

using error_and_code_type = std::tuple<std::string, std::int32_t>;

class MONGOCXX_API base : virtual public std::exception {
   public:
    base(bsoncxx::document::value raw_server_error);
    base(error_and_code_type error_and_code);

    base(
      bsoncxx::document::value raw_server_error,
      error_and_code_type error_and_code
    );

    ///
    /// @returns The raw server error, if it is available.
    ///
    const stdx::optional<bsoncxx::document::value>& raw_server_error() const;
    stdx::optional<bsoncxx::document::value>& raw_server_error();

    ///
    /// @returns The error message and code, if it is available.
    ///
    const stdx::optional<error_and_code_type>& error_and_code() const;
    stdx::optional<error_and_code_type>& error_and_code();

   private:
    stdx::optional<bsoncxx::document::value> _raw_server_error{};
    stdx::optional<error_and_code_type> _error_and_code{};
};

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
