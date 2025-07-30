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

#include <bsoncxx/v1/types/view.hh>

//

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/decimal128.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/oid.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/id.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace types {

using code = v1::types::view::errc;

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                                                             \
    static_assert(is_regular<b_##_name>::value, "bsoncxx::v1::types::b_" #_name " must be regular"); \
    static_assert(is_semitrivial<b_##_name>::value, "bsoncxx::v1::types::b_" #_name " must be semitrivial");

BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

static_assert(is_regular<view>::value, "bsoncxx::v1::types::view must be regular");
static_assert(is_semitrivial<view>::value, "bsoncxx::v1::types::view must be semitrivial");

#pragma push_macro("X")
#undef X
#define X(_name, _value) constexpr id b_##_name::type_id;
BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

#pragma push_macro("X")
#undef X
#define X(_name, _value)                              \
    b_##_name view::get_##_name() const {             \
        if (_id != id::k_##_name) {                   \
            throw v1::exception{code::type_mismatch}; \
        }                                             \
        return _b_##_name;                            \
    }

BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

std::error_category const& view::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::types::view";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::type_mismatch:
                    return "requested type does not match the underlying type";
                default:
                    return "unknown: " + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::type_mismatch:
                        return source == condition::bsoncxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::type_mismatch:
                        return source == condition::runtime_error;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

namespace {

v1::stdx::string_view to_sv(char const* data, std::size_t size) {
    if (!data || size == 0u) {
        return {};
    }

    return {data, size};
}

v1::stdx::string_view to_sv(char const* data) {
    if (!data) {
        return {};
    }

    return to_sv(data, std::strlen(data));
}

} // namespace

view view::internal::make(bson_value_t const& v) {
    // BSONCXX_V1_TYPES_XMACRO: update below.
    switch (static_cast<v1::types::id>(v.value_type)) {
        case v1::types::id::k_double:
            return v1::types::b_double{v.value.v_double};

        case v1::types::id::k_string:
            return b_string{to_sv(v.value.v_utf8.str, v.value.v_utf8.len)};

        case v1::types::id::k_document:
            return v1::types::b_document{v1::document::view{v.value.v_doc.data}};

        case v1::types::id::k_array:
            return v1::types::b_array{v1::array::view(v.value.v_doc.data)};

        case v1::types::id::k_binary:
            return v1::types::b_binary{
                static_cast<v1::types::binary_subtype>(v.value.v_binary.subtype),
                v.value.v_binary.data_len,
                v.value.v_binary.data};

        case v1::types::id::k_undefined:
            return v1::types::b_undefined{};

        case v1::types::id::k_oid:
            return v1::types::b_oid{v1::oid{v.value.v_oid.bytes, sizeof(v.value.v_oid.bytes)}};

        case v1::types::id::k_bool:
            return v1::types::b_bool{v.value.v_bool};

        case v1::types::id::k_date:
            return v1::types::b_date{std::chrono::milliseconds{v.value.v_datetime}};

        case v1::types::id::k_null:
            return v1::types::b_null{};

        case v1::types::id::k_regex:
            return v1::types::b_regex{to_sv(v.value.v_regex.regex), to_sv(v.value.v_regex.options)};

        case v1::types::id::k_dbpointer:
            return v1::types::b_dbpointer{
                to_sv(v.value.v_dbpointer.collection, v.value.v_dbpointer.collection_len),
                v1::oid{v.value.v_dbpointer.oid.bytes, sizeof(v.value.v_dbpointer.oid.bytes)}};

        case v1::types::id::k_code:
            return v1::types::b_code{v1::stdx::string_view{v.value.v_code.code, v.value.v_code.code_len}};

        case v1::types::id::k_symbol:
            return v1::types::b_symbol{v1::stdx::string_view{v.value.v_symbol.symbol, v.value.v_symbol.len}};

        case v1::types::id::k_codewscope:
            return v1::types::b_codewscope{
                to_sv(v.value.v_codewscope.code, v.value.v_codewscope.code_len),
                v1::document::view{v.value.v_codewscope.scope_data}};

        case v1::types::id::k_int32:
            return v1::types::b_int32{v.value.v_int32};

        case v1::types::id::k_timestamp:
            return v1::types::b_timestamp{v.value.v_timestamp.increment, v.value.v_timestamp.timestamp};

        case v1::types::id::k_int64:
            return v1::types::b_int64{v.value.v_int64};

        case v1::types::id::k_decimal128:
            return v1::types::b_decimal128{v1::decimal128{v.value.v_decimal128.high, v.value.v_decimal128.low}};

        case v1::types::id::k_maxkey:
            return v1::types::b_maxkey{};

        case v1::types::id::k_minkey:
            return v1::types::b_minkey{};
    }
    // BSONCXX_V1_TYPES_XMACRO: update above.

    BSONCXX_PRIVATE_UNREACHABLE;
}

v1::stdx::optional<view>
view::internal::make(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen) {
    bson_iter_t iter;

    if (!bson_iter_init_from_data_at_offset(&iter, raw, length, offset, keylen)) {
        return v1::stdx::nullopt;
    }

    if (auto const value = bson_iter_value(&iter)) {
        return make(*value);
    }

    return v1::stdx::nullopt;
}

void view::internal::type_id(view& v, v1::types::id id) {
    v._id = id;
}

} // namespace types
} // namespace v1
} // namespace bsoncxx
