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

#include <bsoncxx/v1/types/id-fwd.hpp>

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
#define X(_name, _value)                                                                  \
    v_noabi::types::b_##_name const& view::get_##_name() const {                          \
        if (_id != type::k_##_name) {                                                     \
            throw v_noabi::exception{v_noabi::error_code::k_need_element_type_k_##_name}; \
        }                                                                                 \
        return _b_##_name;                                                                \
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
    if (lhs._id != rhs._id) {
        return false;
    }

#pragma push_macro("X")
#undef X
#define X(_name, _value)           \
    case v_noabi::type::k_##_name: \
        return lhs._b_##_name == rhs._b_##_name;

    switch (lhs._id) {
        BSONCXX_V1_TYPES_XMACRO(X)
        default:
            // Silence compiler warnings about failing to return a value.
            BSONCXX_PRIVATE_UNREACHABLE;
    }
#pragma pop_macro("X")
}

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
