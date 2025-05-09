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

#include <mongocxx/exception/gridfs_exception-fwd.hpp>

#include <mongocxx/exception/exception.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// An exception thrown during a GridFS operation.
///
/// @see
/// - @ref mongocxx::v_noabi::exception
///
class gridfs_exception : public exception {
   public:
    ~gridfs_exception() override;

    gridfs_exception(gridfs_exception&&) = default;
    gridfs_exception& operator=(gridfs_exception&&) = default;
    gridfs_exception(gridfs_exception const&) = default;
    gridfs_exception& operator=(gridfs_exception const&) = default;

    using exception::exception;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::gridfs_exception.
///
