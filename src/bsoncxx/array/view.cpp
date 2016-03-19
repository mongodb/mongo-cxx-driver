// Copyright 2014 MongoDB Inc.
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

#include <cstdlib>
#include <cstring>
#include <tuple>

#include <bson.h>

#include <bsoncxx/private/itoa.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/private/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace array {

namespace {
bson_iter_t to_bson_iter_t(element e) {
    bson_iter_t i;
    i.raw = e.raw();
    i.len = e.length();
    i.next_off = e.offset();
    return i;
}
}  // namespace

view::iterator::iterator() {
}

view::iterator::iterator(const element& element) : _element(element) {
}

view::iterator::reference view::iterator::operator*() {
    return _element;
}

view::iterator::pointer view::iterator::operator->() {
    return &_element;
}

view::iterator& view::iterator::operator++() {
    if (!_element) {
        return *this;
    }

    bson_iter_t i = to_bson_iter_t(_element);
    bson_iter_next(&i);

    if (!bson_iter_next(&i)) {
        _element = element{nullptr, 0, 0};
    } else {
        _element = element{i.raw, i.len, i.off};
    }

    return *this;
}

view::iterator view::iterator::operator++(int) {
    iterator before(*this);
    operator++();
    return before;
}

bool operator==(const view::iterator& lhs, const view::iterator& rhs) {
    return std::forward_as_tuple(lhs._element.raw(), lhs._element.offset()) ==
           std::forward_as_tuple(rhs._element.raw(), rhs._element.offset());
}

bool operator!=(const view::iterator& lhs, const view::iterator& rhs) {
    return !(lhs == rhs);
}

view::const_iterator::const_iterator() {
}

view::const_iterator::const_iterator(const element& element) : _element(element) {
}

view::const_iterator::reference view::const_iterator::operator*() {
    return _element;
}

view::const_iterator::pointer view::const_iterator::operator->() {
    return &_element;
}

view::const_iterator& view::const_iterator::operator++() {
    if (!_element) {
        return *this;
    }

    bson_iter_t i = to_bson_iter_t(_element);
    bson_iter_next(&i);

    if (!bson_iter_next(&i)) {
        _element = element{nullptr, 0, 0};
    } else {
        _element = element{i.raw, i.len, i.off};
    }

    return *this;
}

view::const_iterator view::const_iterator::operator++(int) {
    const_iterator before(*this);
    operator++();
    return before;
}

bool operator==(const view::const_iterator& lhs, const view::const_iterator& rhs) {
    return std::forward_as_tuple(lhs._element.raw(), lhs._element.offset()) ==
           std::forward_as_tuple(rhs._element.raw(), rhs._element.offset());
}

bool operator!=(const view::const_iterator& lhs, const view::const_iterator& rhs) {
    return !(lhs == rhs);
}

view::const_iterator view::cbegin() const {
    bson_t b;
    bson_iter_t iter;

    bson_init_static(&b, data(), length());
    bson_iter_init(&iter, &b);
    if (!bson_iter_next(&iter)) {
        return const_iterator{};
    }

    return const_iterator(element{iter.raw, iter.len, iter.off});
}

view::const_iterator view::cend() const {
    return const_iterator();
}

view::iterator view::begin() const {
    bson_t b;
    bson_iter_t iter;

    bson_init_static(&b, data(), length());
    bson_iter_init(&iter, &b);
    if (!bson_iter_next(&iter)) {
        return iterator{};
    }

    return iterator(element{iter.raw, iter.len, iter.off});
}

view::iterator view::end() const {
    return iterator();
}

view::iterator view::find(std::uint32_t i) const {
    itoa key(i);

    bson_t b;
    bson_iter_t iter;

    bson_init_static(&b, data(), length());

    if (bson_iter_init_find(&iter, &b, key.c_str())) {
        return iterator(element(iter.raw, iter.len, iter.off));
    }

    return end();
}

element view::operator[](std::uint32_t i) const {
    return *(this->find(i));
}

view::view(const std::uint8_t* data, std::size_t length) : _view(data, length) {
}

view::view() : _view() {
}

const std::uint8_t* view::data() const {
    return _view.data();
}
std::size_t view::length() const {
    return _view.length();
}

bool view::empty() const {
    return _view.empty();
}

view::operator document::view() const {
    return _view;
}

bool operator==(view lhs, view rhs) {
    return (lhs.length() == rhs.length()) &&
           (std::memcmp(lhs.data(), rhs.data(), lhs.length()) == 0);
}

bool operator!=(view lhs, view rhs) {
    return !(lhs == rhs);
}

}  // namespace array
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
