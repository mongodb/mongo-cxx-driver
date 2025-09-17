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

#include <bsoncxx/document/element.hpp>

//

#include <bsoncxx/v1/detail/macros.hpp>

#include <bsoncxx/v1/element/view.hh>
#include <bsoncxx/v1/types/value.hh>

#include <cstdlib>
#include <string>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace document {

static_assert(is_implicitly_convertible<v1::element::view&&, element>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::element::view const&, element>::value, "v1 -> v_noabi must be implicit");
static_assert(is_explicitly_convertible<element&&, v1::element::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<element const&, v1::element::view>::value, "v_noabi -> v1 must be explicit");

v_noabi::type element::type() const try {
    return static_cast<v_noabi::type>(_view.type_id());
} catch (v1::exception const& ex) {
    throw v_noabi::exception{
        v_noabi::error_code::k_unset_element,
        std::string{"cannot return the type of uninitialized element: "} + ex.what()};
}

v1::stdx::string_view element::key() const try { return _view.key(); } catch (v1::exception const& ex) {
    throw v_noabi::exception{
        v_noabi::error_code::k_unset_element,
        std::string{"cannot return the key from an uninitialized element: "} + ex.what()};
}

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                       \
    v_noabi::types::b_##_name element::get_##_name() const try {                               \
        return from_v1(_view.get_##_name());                                                   \
    } catch (v1::exception const& ex) {                                                        \
        throw v_noabi::exception{                                                              \
            error_code::k_need_element_type_k_##_name,                                         \
            std::string{"cannot get " #_name " from an uninitialized element: "} + ex.what()}; \
    }

BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

v_noabi::types::bson_value::view element::get_value() const {
#pragma push_macro("X")
#undef X
#define X(_name, _value)           \
    case v_noabi::type::k_##_name: \
        return {_view.get_##_name()};

    switch (this->type()) {
        BSONCXX_V1_TYPES_XMACRO(X)

        default:
            BSONCXX_PRIVATE_UNREACHABLE;
    }
#pragma pop_macro("X")
}

v_noabi::types::bson_value::value element::get_owning_value() const {
    auto value_opt = v1::types::value::internal::make(this->raw(), this->length(), this->offset(), this->keylen());
    if (!value_opt) {
        return {};
    }
    return {std::move(*value_opt)};
}

v_noabi::types::bson_value::view element::type_view() const {
    return this->get_value();
}

v_noabi::types::bson_value::value element::type_value() const {
    return this->get_owning_value();
}

element element::operator[](stdx::string_view key) const try { return _view[key]; } catch (v1::exception const&) {
    // For backward compatibility, convert any exceptions into an invalid element.
    return {};
}

v_noabi::array::element element::operator[](std::uint32_t i) const try {
    return _view[i];
} catch (v1::exception const&) {
    // For backward compatibility, convert any exceptions into an invalid element.
    return {};
}

bool operator==(element const& lhs, types::bson_value::view const& rhs) {
    return lhs.get_value() == rhs;
}

} // namespace document
} // namespace v_noabi
} // namespace bsoncxx
