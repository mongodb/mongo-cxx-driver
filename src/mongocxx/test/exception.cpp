// Copyright 2021-Present MongoDB Inc.
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

#include "helpers.hpp"

#include <mongocxx/exception/exception.hpp>

namespace {

TEST_CASE("exceptions/basic", "[exceptions]") {

/* Although std::logic_error is not thrown by the standard library, we have a history of
doing it (using error_code for more specific behavior) with our implementation: */
mongocxx::logic_error le("this is a mongocxx::logic_error");

}

}  // namespace
