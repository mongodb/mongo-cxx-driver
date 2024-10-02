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

#include <mongocxx/exception/query_exception-fwd.hpp>

#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Class representing an exception during a query operation.
///
/// @see
/// - @ref mongocxx::v_noabi::operation_exception
///
class query_exception : public operation_exception {
   public:
    ~query_exception() override;

    query_exception(query_exception&&) = default;
    query_exception& operator=(query_exception&&) = default;
    query_exception(const query_exception&) = default;
    query_exception& operator=(const query_exception&) = default;

    using operation_exception::operation_exception;
};

}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::query_exception.
///
