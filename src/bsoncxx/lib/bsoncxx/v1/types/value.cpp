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
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace types {

using code = v1::types::value::errc;

static_assert(is_regular<value>::value, "bsoncxx::v1::types::value must be regular");
static_assert(is_nothrow_moveable<value>::value, "bsoncxx::v1::types::value must be nothrow moveable");

namespace {

v1::stdx::string_view to_sv(char const* data) {
    // `std::string_view` does not accept null pointers.
    return data ? v1::stdx::string_view{data} : v1::stdx::string_view{};
}

char* to_bson_copy(v1::stdx::string_view sv) {
    // Guard the call to `memcpy()` (passing nullptr is UB).
    if (sv.data() == nullptr) {
        return nullptr;
    }

    // For BSON compatibility, an empty string is always represented as a not-null string.
    // Invariant: `sv.size() < UINT32_MAX` is enforced by `code::invalid_length_u32`.
    auto const ret = static_cast<char*>(bson_malloc(sv.size() + 1u));
    std::memcpy(ret, sv.data(), sv.size());
    ret[sv.size()] = '\0';
    return ret;
}

std::uint8_t* to_bson_copy(std::uint8_t const* data, std::size_t size) {
    // Guard the call to `memcpy()` (passing nullptr is UB).
    if (!data || size == 0u) {
        return nullptr;
    }

    // BSON bytes do not have the same not-null requirement as strings.
    auto const ret = static_cast<std::uint8_t*>(bson_malloc(size));
    std::memcpy(ret, data, size);
    return ret;
}

void assign_to(bson_value_t& dst, v1::types::b_string const value) {
    dst.value_type = BSON_TYPE_UTF8;

    auto& v_utf8 = dst.value.v_utf8;

    v_utf8.str = to_bson_copy(value.value);
    v_utf8.len = static_cast<std::uint32_t>(value.value.size()); // Invariant: enforced by `code::invalid_length_u32`.
}

void assign_to(bson_value_t& dst, v1::types::b_document const value) {
    dst.value_type = BSON_TYPE_DOCUMENT;

    auto& v_doc = dst.value.v_doc;

    v_doc.data = to_bson_copy(value.value.data(), value.value.size());
    v_doc.data_len = static_cast<std::uint32_t>(value.value.size()); // Invariant: enforced by
                                                                     // `v1::document::view::raw_size()`.
}

void assign_to(bson_value_t& dst, v1::types::b_array const value) {
    dst.value_type = BSON_TYPE_ARRAY;

    auto& v_doc = dst.value.v_doc;

    v_doc.data = to_bson_copy(value.value.data(), value.value.size());
    v_doc.data_len = static_cast<std::uint32_t>(value.value.size()); // Invariant: enforced by
                                                                     // `v1::document::view::raw_size()`.
}

void assign_to(bson_value_t& dst, v1::types::b_binary const value) {
    dst.value_type = BSON_TYPE_BINARY;

    auto& v_binary = dst.value.v_binary;

    v_binary.subtype = static_cast<bson_subtype_t>(value.subtype);
    v_binary.data = to_bson_copy(value.bytes, value.size);
    v_binary.data_len = value.size;
}

void assign_to(bson_value_t& dst, v1::types::b_regex const value) {
    dst.value_type = BSON_TYPE_REGEX;

    auto& v_regex = dst.value.v_regex;

    v_regex.regex = to_bson_copy(value.regex);
    v_regex.options = to_bson_copy(value.options);
}

void assign_to(bson_value_t& dst, v1::types::b_dbpointer const value) {
    dst.value_type = BSON_TYPE_DBPOINTER;

    auto& v_dbpointer = dst.value.v_dbpointer;

    v_dbpointer.collection = to_bson_copy(value.collection);
    v_dbpointer.collection_len =
        static_cast<std::uint32_t>(value.collection.size()); // Invariant: enforced by `code::invalid_length_u32`.
    std::memcpy(v_dbpointer.oid.bytes, value.value.bytes(), value.value.size());
}

void assign_to(bson_value_t& dst, v1::types::b_code const value) {
    dst.value_type = BSON_TYPE_CODE;

    auto& v_code = dst.value.v_code;

    v_code.code = to_bson_copy(value.code);
    v_code.code_len =
        static_cast<std::uint32_t>(value.code.size()); // Invariant: enforced by `code::invalid_length_u32`.
}

void assign_to(bson_value_t& dst, v1::types::b_symbol const value) {
    dst.value_type = BSON_TYPE_SYMBOL;

    auto& v_symbol = dst.value.v_symbol;

    v_symbol.symbol = to_bson_copy(value.symbol);
    v_symbol.len =
        static_cast<std::uint32_t>(value.symbol.size()); // Invariant: enforced by `code::invalid_length_u32`.
}

void assign_to(bson_value_t& dst, v1::types::b_codewscope const value) {
    dst.value_type = BSON_TYPE_CODEWSCOPE;

    auto& v_codewscope = dst.value.v_codewscope;

    v_codewscope.code = to_bson_copy(value.code);
    v_codewscope.code_len =
        static_cast<std::uint32_t>(value.code.size()); // Invariant: enforced by `code::invalid_length_u32`.
    v_codewscope.scope_data = to_bson_copy(value.scope.data(), value.scope.size());
    v_codewscope.scope_len = static_cast<std::uint32_t>(value.scope.size()); // Invariant: enforced by
                                                                             // `v1::document::view::raw_size()`.
}

void assign_to(bson_value_t& dst, v1::types::b_double const value) {
    dst.value_type = BSON_TYPE_DOUBLE;
    dst.value.v_double = value.value;
}

void assign_to(bson_value_t& dst, v1::types::b_undefined) {
    dst.value_type = BSON_TYPE_UNDEFINED;
}

void assign_to(bson_value_t& dst, v1::types::b_oid const value) {
    dst.value_type = BSON_TYPE_OID;
    std::memcpy(dst.value.v_oid.bytes, value.value.bytes(), value.value.size());
}

void assign_to(bson_value_t& dst, v1::types::b_bool const value) {
    dst.value_type = BSON_TYPE_BOOL;
    dst.value.v_bool = value.value;
}

void assign_to(bson_value_t& dst, v1::types::b_date const value) {
    dst.value_type = BSON_TYPE_DATE_TIME;
    dst.value.v_datetime = value.value.count();
}

void assign_to(bson_value_t& dst, v1::types::b_null) {
    dst.value_type = BSON_TYPE_NULL;
}

void assign_to(bson_value_t& dst, v1::types::b_int32 const value) {
    dst.value_type = BSON_TYPE_INT32;
    dst.value.v_int32 = value.value;
}

void assign_to(bson_value_t& dst, v1::types::b_timestamp const value) {
    dst.value_type = BSON_TYPE_TIMESTAMP;
    dst.value.v_timestamp.timestamp = value.timestamp;
    dst.value.v_timestamp.increment = value.increment;
}

void assign_to(bson_value_t& dst, v1::types::b_int64 const value) {
    dst.value_type = BSON_TYPE_INT64;
    dst.value.v_int64 = value.value;
}

void assign_to(bson_value_t& dst, v1::types::b_decimal128 const value) {
    dst.value_type = BSON_TYPE_DECIMAL128;
    dst.value.v_decimal128.high = value.value.high();
    dst.value.v_decimal128.low = value.value.low();
}

void assign_to(bson_value_t& dst, v1::types::b_maxkey) {
    dst.value_type = BSON_TYPE_MAXKEY;
}

void assign_to(bson_value_t& dst, v1::types::b_minkey) {
    dst.value_type = BSON_TYPE_MINKEY;
}

} // namespace

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
            auto tmp = std::move(other);
            this->swap(tmp);
        }
        return *this;
    }

    impl(impl const& other) {
        // BSONCXX_V1_TYPES_XMACRO: update below.
        switch (other._value.value_type) {
            case BSON_TYPE_UTF8: {
                auto& v_utf8 = other._value.value.v_utf8;
                assign_to(_value, v1::types::b_string{v1::stdx::string_view{v_utf8.str, v_utf8.len}});
                return;
            }

            case BSON_TYPE_DOCUMENT: {
                auto& v_doc = other._value.value.v_doc;
                assign_to(_value, v1::types::b_document{v1::document::view{v_doc.data, v_doc.data_len}});
                return;
            }

            case BSON_TYPE_ARRAY: {
                auto& v_doc = other._value.value.v_doc;
                assign_to(_value, v1::types::b_array{v1::array::view{v_doc.data, v_doc.data_len}});
                return;
            }

            case BSON_TYPE_BINARY: {
                auto& v_binary = other._value.value.v_binary;
                assign_to(
                    _value,
                    v1::types::b_binary{
                        static_cast<v1::types::binary_subtype>(v_binary.subtype),
                        v_binary.data_len,
                        v_binary.data,
                    });
                return;
            }

            case BSON_TYPE_REGEX: {
                auto& v_regex = other._value.value.v_regex;
                assign_to(_value, v1::types::b_regex{to_sv(v_regex.regex), to_sv(v_regex.options)});
                return;
            }

            case BSON_TYPE_DBPOINTER: {
                auto& v_dbpointer = other._value.value.v_dbpointer;
                assign_to(
                    _value,
                    v1::types::b_dbpointer{
                        v1::stdx::string_view{
                            v_dbpointer.collection,
                            v_dbpointer.collection_len,
                        },
                        v1::oid{v_dbpointer.oid.bytes, sizeof(bson_oid_t)},
                    });
                return;
            }

            case BSON_TYPE_CODE: {
                auto& v_code = other._value.value.v_code;
                assign_to(_value, v1::types::b_code{v1::stdx::string_view{v_code.code, v_code.code_len}});
                return;
            }

            case BSON_TYPE_SYMBOL: {
                auto& v_symbol = other._value.value.v_symbol;
                assign_to(_value, v1::types::b_symbol{v1::stdx::string_view{v_symbol.symbol, v_symbol.len}});
                return;
            }

            case BSON_TYPE_CODEWSCOPE: {
                auto& v_codewscope = other._value.value.v_codewscope;
                assign_to(
                    _value,
                    v1::types::b_codewscope{
                        v1::stdx::string_view{
                            v_codewscope.code,
                            v_codewscope.code_len,
                        },
                        v1::document::view{
                            v_codewscope.scope_data,
                            v_codewscope.scope_len,
                        },
                    });
                return;
            }

            case BSON_TYPE_EOD:
            case BSON_TYPE_DOUBLE:
            case BSON_TYPE_UNDEFINED:
            case BSON_TYPE_OID:
            case BSON_TYPE_BOOL:
            case BSON_TYPE_DATE_TIME:
            case BSON_TYPE_NULL:
            case BSON_TYPE_INT32:
            case BSON_TYPE_TIMESTAMP:
            case BSON_TYPE_INT64:
            case BSON_TYPE_DECIMAL128:
            case BSON_TYPE_MAXKEY:
            case BSON_TYPE_MINKEY:
                // Straightforward copy: no potentially-nullable values.
                bson_value_copy(&other._value, &_value);
                return;
        }
        // BSONCXX_V1_TYPES_XMACRO: update above.

        BSONCXX_PRIVATE_UNREACHABLE;
    }

    impl& operator=(impl const& other) {
        if (&other != this) {
            auto tmp = other;
            this->swap(tmp);
        }
        return *this;
    }

    void swap(impl& other) {
        std::swap(_value, other._value);
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

    // Helpers to access the inline PIMPL object.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
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
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
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

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
value::value(value&& other) noexcept {
    new (_storage.data()) impl{std::move(impl::with(other))};
}

value& value::operator=(value&& other) noexcept {
    *impl::with(this) = std::move(impl::with(other));
    return *this;
}

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
value::value(value const& other) {
    new (_storage.data()) impl{impl::with(other)};
}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by impl.
value& value::operator=(value const& other) {
    *impl::with(this) = impl::with(other);
    return *this;
}

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
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_string const v) : value{} {
    if (v.value.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_document const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_array const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_binary const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_undefined v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_oid const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_bool const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_date const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
value::value(v1::types::b_null v) {
    assign_to((new (_storage.data()) impl{})->_value, v);
}

value::value(v1::types::b_regex const v) : value{} {
    if (v.regex.size() >= UINT32_MAX || v.options.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_dbpointer const v) : value{} {
    if (v.collection.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_code const v) : value{} {
    if (v.code.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_symbol const v) : value{} {
    if (v.symbol.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_codewscope const v) : value{} {
    if (v.code.size() >= UINT32_MAX) {
        throw v1::exception{code::invalid_length_u32};
    }

    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_int32 const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_timestamp const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_int64 const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_decimal128 const v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_maxkey v) : value{} {
    assign_to(impl::with(this)->_value, v);
}

value::value(v1::types::b_minkey v) : value{} {
    assign_to(impl::with(this)->_value, v);
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

bson_value_t const& value::internal::get_bson_value(value const& v) {
    return impl::with(v)._value;
}

bson_value_t& value::internal::get_bson_value(value& v) {
    return impl::with(v)._value;
}

} // namespace types
} // namespace v1
} // namespace bsoncxx
