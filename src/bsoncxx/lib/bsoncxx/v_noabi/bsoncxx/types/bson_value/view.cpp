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

#include <bsoncxx/types/bson_value/view.hpp>

//

#include <bsoncxx/v1/detail/macros.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

static_assert(is_explicitly_convertible<view&&, v1::types::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<view const&, v1::types::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::types::view&&, view>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::types::view const&, view>::value, "v1 -> v_noabi must be implicit");

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                         \
    v_noabi::types::b_##_name view::get_##_name() const try {                                    \
        return from_v1(_view.get_##_name());                                                     \
    } catch (v1::exception const& ex) {                                                          \
        throw v_noabi::exception{v_noabi::error_code::k_need_element_type_k_##_name, ex.what()}; \
    }

///
/// Return the BSON type value of this element.
///
/// @throws bsoncxx::v_noabi::exception if this element is not the requested type.
///
/// @{
BSONCXX_V1_TYPES_XMACRO(X)
/// @}
///
#pragma pop_macro("X")

bool operator==(view const& lhs, view const& rhs) {
    if (lhs.type() != rhs.type()) {
        return false;
    }

    switch (static_cast<int>(lhs.type())) {
#define BSONCXX_ENUM(type, val) \
    case val:                   \
        return lhs.get_##type() == rhs.get_##type();
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    // Silence compiler warnings about failing to return a value.
    BSONCXX_PRIVATE_UNREACHABLE;
}

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
