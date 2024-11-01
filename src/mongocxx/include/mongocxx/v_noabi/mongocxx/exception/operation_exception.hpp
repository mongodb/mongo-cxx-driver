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

#include <string>

#include <mongocxx/exception/operation_exception-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/exception/exception.hpp>

#include <bsoncxx/config/prelude.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

BSONCXX_PUSH_WARNINGS();
BSONCXX_DISABLE_WARNING(MSVC(4251));
BSONCXX_DISABLE_WARNING(MSVC(4275));

///
/// Class representing an exception received from a MongoDB server.  It includes the server-provided
/// error code, if one was available.
///
/// @see
/// - @ref mongocxx::v_noabi::exception
///
class operation_exception : public exception {
   public:
    ~operation_exception() override;

    operation_exception(operation_exception&&) = default;
    operation_exception& operator=(operation_exception&&) = default;
    operation_exception(const operation_exception&) = default;
    operation_exception& operator=(const operation_exception&) = default;

    using exception::exception;

    ///
    /// Constructs a new operation exception.
    ///
    /// @param ec
    ///   The error code associated with this exception.
    /// @param raw_server_error
    ///   The optional raw bson error document to be associated with this exception.
    /// @param what_arg
    ///   An optional message to be returned by `what`.
    ///
    operation_exception(std::error_code ec,
                        bsoncxx::v_noabi::document::value&& raw_server_error,
                        std::string what_arg = "");

    ///
    /// The optional raw bson error document from the server.
    ///
    /// @returns The raw server error, if it is available.
    ///
    /// @{
    const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>& raw_server_error()
        const;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>& raw_server_error();
    /// @}
    ///

    ///
    /// Checks for the specified label in a operation exception.
    ///
    /// @param label
    ///   A string of the label to search for
    ///
    /// @returns True if the label is found in the operation exception, and false otherwise.
    ///
    bool has_error_label(bsoncxx::v_noabi::stdx::string_view label) const;

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> _raw_server_error;
};

BSONCXX_POP_WARNINGS();

}  // namespace v_noabi
}  // namespace mongocxx

#include <bsoncxx/config/postlude.hpp>

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::operation_exception.
///
