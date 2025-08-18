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

#include <bsoncxx/array/element.hpp>

//

#include <bsoncxx/v1/element/view.hh>

#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace array {

// MSVC: `std::is_constructible<T, Args...>` does not work with using-declared conversion functions to class type...?
#if !defined(_MSC_VER)
static_assert(is_explicitly_convertible<element&&, v1::element::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<element const&, v1::element::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::element::view&&, element>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::element::view const&, element>::value, "v1 -> v_noabi must be implicit");
#endif // !defined(_MSC_VER)

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx
