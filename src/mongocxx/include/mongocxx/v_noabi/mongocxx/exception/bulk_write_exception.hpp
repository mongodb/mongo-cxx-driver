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

#include <mongocxx/exception/bulk_write_exception-fwd.hpp>

#include <bsoncxx/document/value.hpp>

#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// An exception thrown during a bulk write operation.
///
/// @see
/// - @ref mongocxx::v_noabi::operation_exception
///
class bulk_write_exception : public operation_exception {
   public:
    ~bulk_write_exception() override;

    bulk_write_exception(bulk_write_exception&&) = default;
    bulk_write_exception& operator=(bulk_write_exception&&) = default;
    bulk_write_exception(bulk_write_exception const&) = default;
    bulk_write_exception& operator=(bulk_write_exception const&) = default;

    using operation_exception::operation_exception;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::bulk_write_exception.
///
