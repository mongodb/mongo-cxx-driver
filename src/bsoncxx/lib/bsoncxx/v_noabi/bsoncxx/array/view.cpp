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

#include <bsoncxx/array/view.hpp>

//

#include <bsoncxx/v1/array/view.hpp>

#include <bsoncxx/v1/element/view.hh>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/itoa.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {
namespace array {

static_assert(is_explicitly_convertible<view&&, v1::array::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<view const&, v1::array::view>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::array::view&&, view>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::array::view const&, view>::value, "v1 -> v_noabi must be implicit");

namespace {

bson_iter_t to_bson_iter_t(v_noabi::array::element e) {
    bson_iter_t iter{};
    (void)bson_iter_init_from_data_at_offset(&iter, e.raw(), e.length(), e.offset(), e.keylen());
    return iter;
}

} // namespace

view::const_iterator& view::const_iterator::operator++() {
    if (!_element) {
        return *this;
    }

    // the bson_t pointer and length remain unchanged while iterating.
    auto raw = _element.raw();
    auto len = _element.length();

    bson_iter_t iter = to_bson_iter_t(_element);

    if (!bson_iter_next(&iter)) {
        _element = {};
    } else {
        _element = v1::element::view::internal::make(raw, len, bson_iter_offset(&iter), bson_iter_key_len(&iter));
    }

    return *this;
}

view::const_iterator view::cbegin() const {
    bson_iter_t iter;

    if (!bson_iter_init_from_data(&iter, this->data(), this->length())) {
        return this->cend();
    }

    if (!bson_iter_next(&iter)) {
        return this->cend();
    }

    return const_iterator{v1::element::view::internal::make(
        this->data(), static_cast<std::uint32_t>(this->length()), bson_iter_offset(&iter), bson_iter_key_len(&iter))};
}

view::const_iterator view::find(std::uint32_t i) const {
    itoa key{i};

    bson_t bson;

    if (!bson_init_static(&bson, this->data(), this->length())) {
        return this->cend();
    }

    bson_iter_t iter;

    if (!bson_iter_init(&iter, &bson)) {
        return this->cend();
    }

    if (!bson_iter_init_find(&iter, &bson, key.c_str())) {
        return this->cend();
    }

    return const_iterator{v1::element::view::internal::make(
        this->data(), static_cast<std::uint32_t>(this->length()), bson_iter_offset(&iter), bson_iter_key_len(&iter))};
}

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx
