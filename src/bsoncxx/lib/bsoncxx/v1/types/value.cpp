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

#include <bsoncxx/v1/types/value.hh>

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

#include <bsoncxx/v1/types/view.hh>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace types {

using code = v1::types::value::errc;

static_assert(is_regular<value>::value, "bsoncxx::v1::types::value must be regular");
static_assert(is_nothrow_moveable<value>::value, "bsoncxx::v1::types::value must be nothrow moveable");

class alignas(BSONCXX_PRIVATE_MAX_ALIGN_T) value::impl {
    friend value::internal;

   public:
    enum : std::size_t {
        _padding_size = sizeof(value::_storage) // Total reserved.
                        - sizeof(bson_value_t)  // _value.
                        - 0u,
    };

    // `_padding_size == 0` given `sizeof(void*) == 8`.
    static_assert(_padding_size < sizeof(value::_storage), "sizeof(impl) must not exceed reserved storage size");

    bson_value_t _value = {}; // BSON_TYPE_EOD

    ~impl();

    impl(impl&& other) noexcept
        : _value{other._value} // Ownership transfer.
    {
        other._value = {BSON_TYPE_NULL, {}, {}};
    }

    impl& operator=(impl&& other) noexcept {
        if (&other != this) {
            bson_value_destroy(&_value);
            _value = other._value; // Ownership transfer.
            other._value = {BSON_TYPE_NULL, {}, {}};
        }
        return *this;
    }

    impl(impl const& other) {
        bson_value_copy(&other._value, &_value);
    }

    impl& operator=(impl const& other) {
        if (&other != this) {
            bson_value_destroy(&_value);
            bson_value_copy(&other._value, &_value);
        }
        return *this;
    }

    impl() = default;

    auto t() const -> decltype((_value.value_type)) {
        return _value.value_type;
    }

    auto t() -> decltype((_value.value_type)) {
        return _value.value_type;
    }

    auto v() const -> decltype((_value.value)) {
        return _value.value;
    }

    auto v() -> decltype((_value.value)) {
        return _value.value;
    }

    static impl const& with(value const& self) {
        return *reinterpret_cast<value::impl const*>(self._storage.data());
    }

    static impl const* with(value const* self) {
        return reinterpret_cast<value::impl const*>(self->_storage.data());
    }

    static impl& with(value& self) {
        return *reinterpret_cast<value::impl*>(self._storage.data());
    }

    static impl* with(value* self) {
        return reinterpret_cast<value::impl*>(self->_storage.data());
    }
};

value::impl::~impl() {
    static_assert(is_semiregular<impl>::value, "bsoncxx::v1::types::value::impl must be semiregular");
    static_assert(is_nothrow_moveable<impl>::value, "bsoncxx::v1::types::value::impl must be nothrow moveable");

    static_assert(sizeof(value::_storage) >= sizeof(value::impl), "insufficient size");
    static_assert(alignof(value) >= alignof(value::impl), "insufficient alignment");

    bson_value_destroy(&_value);
}

value::~value() {
    impl::with(this)->~impl();
}

value::value(value&& other) noexcept {
    new (impl::with(this)) impl{std::move(impl::with(other))};
}

value& value::operator=(value&& other) noexcept {
    *impl::with(this) = std::move(impl::with(other));
    return *this;
}

value::value(value const& other) {
    new (impl::with(this)) impl{impl::with(other)};
}

value& value::operator=(value const& other) {
    *impl::with(this) = impl::with(other);
    return *this;
}

namespace {

// For backward compatibility, do not prematurely truncate strings in bsoncxx API. Instead, defer handling of potential
// embedded null bytes to the bson library.
char* to_bson_copy(v1::stdx::string_view sv) {
    if (sv.empty()) {
        return nullptr;
    }

    auto ret = static_cast<char*>(bson_malloc(sv.size() + 1u));
    std::memcpy(ret, sv.data(), sv.size());
    ret[sv.size()] = '\0';
    return ret;
}

std::uint8_t* to_bson_copy(void const* data, std::size_t size) {
    if (!data) {
        return nullptr;
    }

    auto ret = static_cast<std::uint8_t*>(bson_malloc(size));
    std::memcpy(ret, data, size);
    return ret;
}

} // namespace

value::value(v1::types::view const& v) : value{} {
#pragma push_macro("X")
#undef X
#define X(_name, _value)                \
    case v1::types::id::k_##_name:      \
        *this = value{v.get_##_name()}; \
        return;

    switch (v.type_id()) {
        BSONCXX_V1_TYPES_XMACRO(X)

        default:
            throw v1::exception{code::invalid_type};
    }
#pragma pop_macro("X")
}

// BSONCXX_V1_TYPES_XMACRO: update below.

value::value(v1::types::b_double const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_DOUBLE;
    impl::with(this)->v().v_double = v;
}

value::value(v1::types::b_string const v) : value{} {
    if (v.value.size() > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }
    auto const len = static_cast<std::uint32_t>(v.value.size());

    impl::with(this)->t() = BSON_TYPE_UTF8;

    auto& v_utf8 = impl::with(this)->v().v_utf8;

    v_utf8.str = to_bson_copy(v.value);
    v_utf8.len = len;
}

value::value(v1::types::b_document const v) : value{} {
    // Range is guaranteed by bsoncxx::v1::document::view::raw_size().
    auto const data_len = static_cast<std::uint32_t>(v.value.size());

    impl::with(this)->t() = BSON_TYPE_DOCUMENT;

    auto& v_doc = impl::with(this)->v().v_doc;

    v_doc.data = to_bson_copy(v.value.data(), v.value.size());
    v_doc.data_len = data_len;
}

value::value(v1::types::b_array const v) : value{} {
    // Range is guaranteed by bsoncxx::v1::document::view::raw_size().
    auto const data_len = static_cast<std::uint32_t>(v.value.size());

    impl::with(this)->t() = BSON_TYPE_ARRAY;

    auto& v_doc = impl::with(this)->v().v_doc;

    v_doc.data = to_bson_copy(v.value.data(), v.value.size());
    v_doc.data_len = data_len;
}

value::value(v1::types::b_binary const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_BINARY;

    auto& v_binary = impl::with(this)->v().v_binary;

    v_binary.subtype = static_cast<bson_subtype_t>(v.subtype);
    v_binary.data = to_bson_copy(v.bytes, v.size);
    v_binary.data_len = v.size;
}

value::value(v1::types::b_undefined) : value{} {
    impl::with(this)->t() = BSON_TYPE_UNDEFINED;
}

value::value(v1::types::b_oid const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_OID;
    std::memcpy(impl::with(this)->v().v_oid.bytes, v.value.bytes(), v.value.size());
}

value::value(v1::types::b_bool const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_BOOL;
    impl::with(this)->v().v_bool = v.value;
}

value::value(v1::types::b_date const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_DATE_TIME;
    impl::with(this)->v().v_datetime = v.value.count();
}

value::value(v1::types::b_null) {
    (new (impl::with(this)) impl{})->t() = BSON_TYPE_NULL;
}

value::value(v1::types::b_regex const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_REGEX;

    auto& v_regex = impl::with(this)->v().v_regex;

    v_regex.regex = to_bson_copy(v.regex);
    v_regex.options = v.options.empty() ? nullptr : to_bson_copy(v.options);
}

value::value(v1::types::b_dbpointer const v) : value{} {
    if (v.collection.size() > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }
    auto const collection_len = static_cast<std::uint32_t>(v.collection.size());

    impl::with(this)->t() = BSON_TYPE_DBPOINTER;

    auto& v_dbpointer = impl::with(this)->v().v_dbpointer;

    v_dbpointer.collection = to_bson_copy(v.collection);
    v_dbpointer.collection_len = collection_len;
    std::memcpy(v_dbpointer.oid.bytes, v.value.bytes(), v.value.size());
}

value::value(v1::types::b_code const v) : value{} {
    if (v.code.size() > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }
    auto const code_len = static_cast<std::uint32_t>(v.code.size());

    impl::with(this)->t() = BSON_TYPE_CODE;

    auto& v_code = impl::with(this)->v().v_code;

    v_code.code = to_bson_copy(v.code);
    v_code.code_len = code_len;
}

value::value(v1::types::b_symbol const v) : value{} {
    if (v.symbol.size() > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }
    auto const len = static_cast<std::uint32_t>(v.symbol.size());

    impl::with(this)->t() = BSON_TYPE_SYMBOL;

    auto& v_symbol = impl::with(this)->v().v_symbol;

    v_symbol.symbol = to_bson_copy(v.symbol);
    v_symbol.len = len;
}

value::value(v1::types::b_codewscope const v) : value{} {
    if (v.code.size() > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }
    auto const code_len = static_cast<std::uint32_t>(v.code.size());

    // Range is guaranteed by bsoncxx::v1::document::view::raw_size().
    auto const scope_len = static_cast<std::uint32_t>(v.scope.size());

    impl::with(this)->t() = BSON_TYPE_CODEWSCOPE;

    auto& v_codewscope = impl::with(this)->v().v_codewscope;

    v_codewscope.code = to_bson_copy(v.code);
    v_codewscope.code_len = code_len;
    v_codewscope.scope_data = to_bson_copy(v.scope.data(), v.scope.size());
    v_codewscope.scope_len = scope_len;
}

value::value(v1::types::b_int32 const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_INT32;
    impl::with(this)->v().v_int32 = v.value;
}

value::value(v1::types::b_timestamp const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_TIMESTAMP;

    auto& v_timestamp = impl::with(this)->v().v_timestamp;

    v_timestamp.timestamp = v.timestamp;
    v_timestamp.increment = v.increment;
}

value::value(v1::types::b_int64 const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_INT64;
    impl::with(this)->v().v_int64 = v.value;
}

value::value(v1::types::b_decimal128 const v) : value{} {
    impl::with(this)->t() = BSON_TYPE_DECIMAL128;

    auto& v_decimal128 = impl::with(this)->v().v_decimal128;

    v_decimal128.high = v.value.high();
    v_decimal128.low = v.value.low();
}

value::value(v1::types::b_maxkey) : value{} {
    impl::with(this)->t() = BSON_TYPE_MAXKEY;
}

value::value(v1::types::b_minkey) : value{} {
    impl::with(this)->t() = BSON_TYPE_MINKEY;
}

// BSONCXX_V1_TYPES_XMACRO: update above.

value::value(std::uint8_t const* data, std::size_t size, v1::types::binary_subtype const subtype) : value{} {
    if (size > UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    *this = value{v1::types::b_binary{subtype, static_cast<std::uint32_t>(size), data}};
}

v1::types::id value::type_id() const {
    return static_cast<v1::types::id>(impl::with(this)->t());
}

v1::types::view value::view() const {
    return v1::types::view::internal::make(impl::with(this)->_value);
}

std::error_category const& value::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::types::value";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_type:
                    return "requested BSON type is not supported";
                case code::invalid_length_u32:
                    return "length is too long (exceeds UINT32_MAX)";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_type:
                    case code::invalid_length_u32:
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
                    case code::invalid_type:
                    case code::invalid_length_u32:
                        return source == condition::invalid_argument;

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

v1::stdx::optional<value>
value::internal::make(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen) {
    bson_iter_t iter;

    if (!bson_iter_init_from_data_at_offset(&iter, raw, length, offset, keylen)) {
        return v1::stdx::nullopt;
    }

    value ret;
    bson_value_copy(bson_iter_value(&iter), &impl::with(ret)._value);
    return ret;
}

bson_value_t& value::internal::get_bson_value(value& v) {
    return impl::with(v)._value;
}

} // namespace types
} // namespace v1
} // namespace bsoncxx
