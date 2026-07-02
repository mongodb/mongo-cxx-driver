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

#include <bsoncxx/v1/document/view.hh>

//

#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <bsoncxx/v1/element/view.hh>

#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace document {

using code = v1::document::view::errc;

static_assert(is_regular<view>::value, "bsoncxx::v1::document::view must be regular");
static_assert(is_semitrivial<view>::value, "bsoncxx::v1::document::view must be semitrivial");

static_assert(is_regular<view::const_iterator>::value, "bsoncxx::v1::document::view::const_iterator must be regular");
static_assert(
    is_nothrow_moveable<view::const_iterator>::value,
    "bsoncxx::v1::document::view::const_iterator must be nothrow moveable");

namespace {

constexpr std::array<std::uint8_t, 5> k_default_view = {{5u, 0u, 0u, 0u, 0u}};

} // namespace

view::view() : view{k_default_view.data()} {}

view::view(std::uint8_t const* data, std::size_t length) : view{data} {
    if (length < _empty_length || length < this->size()) {
        throw v1::exception{v1::document::view::errc::invalid_length};
    }
}

view::const_iterator view::cbegin() const {
    if (!this->operator bool()) {
        return this->cend();
    }

    bson_iter_t iter;

    if (!bson_iter_init_from_data(&iter, _data, this->size())) {
        throw v1::exception{code::invalid_data};
    }

    if (bson_iter_next(&iter)) {
        return const_iterator::internal::make_const_iterator(
            _data, this->size(), bson_iter_offset(&iter), bson_iter_key_len(&iter));
    }

    if (iter.err_off != 0) {
        throw v1::exception{code::invalid_data};
    }

    return this->cend();
}

view::const_iterator view::find(v1::stdx::string_view key) const {
    if (!this->operator bool()) {
        return this->cend();
    }

    if (key.size() >= std::size_t{INT_MAX}) {
        return this->cend();
    }

    // Support null as equivalent to empty.
    if (!key.data()) {
        key = "";
    }

    bson_t bson;

    if (!bson_init_static(&bson, _data, this->size())) {
        throw v1::exception{code::invalid_data};
    }

    bson_iter_t iter;

    if (bson_iter_init_find_w_len(&iter, &bson, key.data(), static_cast<int>(key.size()))) {
        return const_iterator::internal::make_const_iterator(
            _data, this->size(), bson_iter_offset(&iter), bson_iter_key_len(&iter));
    }

    if (iter.err_off != 0) {
        throw v1::exception{code::invalid_data};
    }

    return this->end();
}

std::error_category const& view::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::document::view";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_length:
                    return "length is invalid";
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
                    case code::invalid_length:
                    case code::invalid_data:
                        return source == condition::bsoncxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_length:
                        return type == condition::invalid_argument;

                    case code::invalid_data:
                        return type == condition::runtime_error;

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

view::const_iterator& view::const_iterator::operator++() {
    if (!_element) {
        return *this;
    }

    auto iter_opt = to_bson_iter(_element);
    if (!iter_opt) {
        throw v1::exception{code::invalid_data};
    }
    auto& iter = *iter_opt;

    if (bson_iter_next(&iter)) {
        _element = v1::element::view::internal::make(
            _element.raw(), _element.length(), bson_iter_offset(&iter), bson_iter_key_len(&iter));
        return *this;
    }

    if (iter.err_off != 0) {
        throw v1::exception{code::invalid_data};
    }

    _element = {};

    return *this;
}

view::const_iterator::const_iterator(v1::element::view element) : _element(element) {}

view::const_iterator view::const_iterator::internal::make_const_iterator(
    std::uint8_t const* raw,
    std::size_t length,
    std::uint32_t offset,
    std::uint32_t keylen) {
    return const_iterator{v1::element::view::internal::make(
        raw,
        static_cast<std::uint32_t>(length), // Guarded by `bson_init_static`.
        offset,
        keylen)};
}

} // namespace document
} // namespace v1
} // namespace bsoncxx
