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
// limitations under the License./**

#include "driver/config/prelude.hpp"

#include <cstdint>
#include <exception>

#include "bson/document.hpp"

/// MongoDB C++ Driver exception hierarchy
///
/// std::exception
///     L mongo::driver::exception
///         L mongo::driver::operation_exception
///             L mongo::driver::query_exception
///             L mongo::driver::write_exception
///                 L mongo::driver::duplicate_key_exception
///         L mongo::driver::socket_exception
///

namespace mongo {
namespace driver {

class LIBMONGOCXX_EXPORT exception : public std::exception {};

class LIBMONGOCXX_EXPORT operation_exception : public exception {};

class LIBMONGOCXX_EXPORT query_exception : public operation_exception {};

class LIBMONGOCXX_EXPORT write_exception : public operation_exception {};

class LIBMONGOCXX_EXPORT duplicate_key_exception : public write_exception {};

class write_concern_error {
  private:
    std::int32_t code;
    bson::document::view details;
    std::string message;
};

class write_error {
    std::int32_t code;
    std::string message;
}

class LIBMONGOCXX_EXPORT authentication_exception : public operation_exception {};

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
