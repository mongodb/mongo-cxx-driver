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

#include <bsoncxx/v1/element/view.hh>

//

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <bsoncxx/v1/types/value.hh>
#include <bsoncxx/v1/types/view.hh>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace element {

using code = v1::element::view::errc;

static_assert(is_semiregular<view>::value, "bsoncxx::v1::element::view must be semiregular");
static_assert(is_nothrow_moveable<view>::value, "bsoncxx::v1::element::view must be nothrow moveable");

class alignas(BSONCXX_PRIVATE_MAX_ALIGN_T) view::impl {
   private:
    enum : std::size_t {
        _padding_size = sizeof(view::_storage)       // Total reserved.
                        - sizeof(void*)              // _raw
                        - 3u * sizeof(std::uint32_t) // _length, _offset, and _keylen.
                        - 1u                         // _is_valid (final byte).
                        - 0u,
    };

    // `_padding_size == 3` given `sizeof(void*) == 8`.
    static_assert(_padding_size < sizeof(view::_storage), "sizeof(impl) must not exceed reserved storage size");

    std::uint8_t const* _raw = {};
    std::uint32_t _length = {};
    std::uint32_t _offset = {};
    std::uint32_t _keylen = {};

    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wunused"));
    std::array<unsigned char, _padding_size> _padding = {}; // Reserved.
    BSONCXX_PRIVATE_WARNINGS_POP();

    bool _is_valid = {}; // Last byte.

   public:
    impl() = default;

    impl(
        std::uint8_t const* raw,
        std::uint32_t length,
        std::uint32_t offset,
        std::uint32_t keylen,
        bool is_valid = true)
        : _raw{raw}, _length{length}, _offset{offset}, _keylen{keylen}, _is_valid{is_valid} {}

    void check() const;

    std::uint8_t const* raw() const {
        return _raw;
    }

    std::uint32_t length() const {
        return _length;
    }

    std::uint32_t offset() const {
        return _offset;
    }

    std::uint32_t keylen() const {
        return _keylen;
    }

    bool is_valid() const {
        return _raw && _is_valid;
    }

    impl to_invalid() const {
        auto ret = *this;
        ret._is_valid = false;
        return ret;
    }

    v1::types::id type_id() const {
        auto const iter = this->iter();
        return static_cast<v1::types::id>(bson_iter_type(&iter));
    }

    v1::types::id type_id_unchecked() const {
        if (auto const iter_opt = this->iter_unchecked()) {
            return static_cast<v1::types::id>(bson_iter_type(&*iter_opt));
        }
        return v1::types::id{}; // BSON_TYPE_EOD
    }

    v1::stdx::optional<bson_iter_t> iter_unchecked() const {
        bson_iter_t iter;
        if (bson_iter_init_from_data_at_offset(&iter, _raw, _length, _offset, _keylen)) {
            return iter;
        }
        return v1::stdx::nullopt;
    }

    bson_iter_t iter() const {
        this->check();
        if (auto iter_opt = this->iter_unchecked()) {
            return *iter_opt;
        }
        throw v1::exception{code::invalid_data};
    }

    v1::types::view type_view() const {
        this->check();
        return this->type_view_unchecked();
    }

    v1::types::view type_view_unchecked() const {
        if (auto opt = v1::types::view::internal::make(_raw, _length, _offset, _keylen)) {
            return *opt;
        }
        throw v1::exception{code::invalid_data};
    }

    v1::types::value type_value() const {
        this->check();
        if (auto opt = v1::types::value::internal::make(_raw, _length, _offset, _keylen)) {
            return *opt;
        }
        throw v1::exception{code::invalid_data};
    }

    // Helpers to access the inline PIMPL object.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    static impl const& with(view const& v) {
        return *reinterpret_cast<view::impl const*>(v._storage.data());
    }

    static impl const* with(view const* v) {
        return reinterpret_cast<view::impl const*>(v->_storage.data());
    }

    static impl* with(view* v) {
        return reinterpret_cast<view::impl*>(v->_storage.data());
    }
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
};

void view::impl::check() const {
    static_assert(is_semiregular<impl>::value, "bsoncxx::v1::element::view::impl must be semiregular");
    static_assert(is_semitrivial<impl>::value, "bsoncxx::v1::element::view::impl must be semitrivial");

    static_assert(sizeof(view::_storage) >= sizeof(view::impl), "insufficient size");
    static_assert(alignof(view) >= alignof(view::impl), "insufficient alignment");

    if (!this->is_valid()) {
        bson_iter_t iter;
        if (_raw && bson_iter_init_from_data_at_offset(&iter, _raw, _length, _offset, _keylen)) {
            std::string msg;
            msg += "last known element key \"";
            msg += bson_iter_key(&iter);
            msg += '"';
            throw v1::exception{code::invalid_view, msg};
        } else {
            throw v1::exception{code::invalid_view};
        }
    }
}

view::~view() = default;

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
view::view(view const& other) noexcept {
    new (_storage.data()) impl{impl::with(other)};
}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by impl.
view& view::operator=(view const& other) noexcept {
    *impl::with(this) = impl::with(other);
    return *this;
}

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
view::view() {
    new (_storage.data()) impl{};
}

// _storage: initialized with placement new.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
view::view(impl i) {
    new (_storage.data()) impl{i};
}

view::operator bool() const {
    return impl::with(this)->is_valid();
}

std::uint8_t const* view::raw() const {
    return impl::with(this)->raw();
}

std::uint32_t view::length() const {
    return impl::with(this)->length();
}

std::uint32_t view::offset() const {
    return impl::with(this)->offset();
}

std::uint32_t view::keylen() const {
    return impl::with(this)->keylen();
}

v1::types::id view::type_id() const {
    return impl::with(this)->type_id();
}

v1::stdx::string_view view::key() const {
    auto const iter = impl::with(this)->iter();
    return bson_iter_key(&iter);
}

#pragma push_macro("X")
#undef X
#define X(_name, _value)                                    \
    v1::types::b_##_name view::get_##_name() const {        \
        return impl::with(this)->type_view().get_##_name(); \
    }
BSONCXX_V1_TYPES_XMACRO(X)
#pragma pop_macro("X")

v1::types::view view::type_view() const {
    return impl::with(this)->type_view();
}

v1::types::value view::type_value() const {
    return impl::with(this)->type_value();
}

v1::element::view view::operator[](v1::stdx::string_view key) const {
    auto& impl = impl::with(*this);
    if (!impl.is_valid() || impl.type_id_unchecked() != v1::types::id::k_document) {
        return v1::element::view{impl.to_invalid()};
    }
    return impl.type_view_unchecked().get_document().value[key];
}

v1::element::view view::operator[](std::uint32_t idx) const {
    auto& impl = impl::with(*this);
    if (!impl.is_valid() || impl.type_id_unchecked() != v1::types::id::k_array) {
        return view{impl.to_invalid()};
    }
    return impl.type_view_unchecked().get_array().value[idx];
}

std::error_category const& view::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::element::view";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_view:
                    return "view is invalid";
                case code::invalid_data:
                    return "data is invalid";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_view:
                    case code::invalid_data:
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
                    case code::invalid_view:
                    case code::invalid_data:
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

view view::internal::make(
    std::uint8_t const* raw,
    std::uint32_t length,
    std::uint32_t offset,
    std::uint32_t keylen,
    bool is_valid) {
    return view{view::impl{raw, length, offset, keylen, is_valid}};
}

v1::stdx::optional<bson_iter_t> view::internal::to_bson_iter(view const& v) {
    return impl::with(v).iter_unchecked();
}

v1::stdx::optional<bson_iter_t> to_bson_iter(view const& v) {
    return view::internal::to_bson_iter(v);
}

} // namespace element
} // namespace v1
} // namespace bsoncxx
