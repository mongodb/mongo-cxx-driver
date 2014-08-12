/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <exception>

/**
 * MongoDB C++ Driver exception hierarchy
 *
 * std::exception
 *     L mongo::driver::Exception
 *         L mongo::driver::OperationException
 *             L mongo::driver::QueryException
 *             L mongo::driver::WriteException
 *         L mongo::driver::SocketException
 */

namespace mongo {
namespace driver {

class exception : std::exception {};

class operation_exception : exception {};

class query_exception : operation_exception {};

class write_exception : operation_exception {};

class authentication_exception : operation_exception {};

}  // namespace driver
}  // namespace mongo
