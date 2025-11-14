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
#include <type_traits>

#include <catch2/catch_tostring.hpp> // IWYU pragma: export

namespace bsoncxx {
namespace test {

// Equivalent to `Catch::Detail::stringify`.
template <typename Param>
std::string stringify(Param const& param) {
    using T = typename std::remove_cv<typename std::remove_reference<Param>::type>::type;
    return Catch::StringMaker<T>::convert(param);
}

} // namespace test
} // namespace bsoncxx
