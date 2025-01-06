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

#include <mongocxx/exception/authentication_exception-fwd.hpp>

#include <bsoncxx/document/value.hpp>

#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Unused. To be removed in an upcoming major release.
///
/// @deprecated To be removed in an upcoming major release.
///
class MONGOCXX_DEPRECATED authentication_exception : public operation_exception {
   public:
    ~authentication_exception() override;

    authentication_exception(authentication_exception&&) = default;
    authentication_exception& operator=(authentication_exception&&) = default;
    authentication_exception(authentication_exception const&) = default;
    authentication_exception& operator=(authentication_exception const&) = default;

    using operation_exception::operation_exception;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::authentication_exception.
///
