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

#include <mongocxx/v1/exception.hpp>

//

#include <catch2/catch_tostring.hpp>

CATCH_REGISTER_ENUM(
    mongocxx::v1::source_errc,
    mongocxx::v1::source_errc::zero,
    mongocxx::v1::source_errc::mongocxx,
    mongocxx::v1::source_errc::mongoc,
    mongocxx::v1::source_errc::mongocrypt)

CATCH_REGISTER_ENUM(
    mongocxx::v1::type_errc,
    mongocxx::v1::type_errc::zero,
    mongocxx::v1::type_errc::invalid_argument,
    mongocxx::v1::type_errc::runtime_error)
