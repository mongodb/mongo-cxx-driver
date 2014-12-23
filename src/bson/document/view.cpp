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

#include <cstdlib>
#include <cstring>

#include "bson.h"
#include "bson/document/view.hpp"
#include "bson/types.hpp"
#include "bson/json.hpp"

namespace bson {
namespace document {

view::iterator::iterator(const void* i) : iter(i), is_end(false) {}
view::iterator::iterator(bool is_end) : is_end(is_end) {}

const element& view::iterator::operator*() const { return iter; }
const element* view::iterator::operator->() const { return &iter; }

view::iterator& view::iterator::operator++() {
    bson_iter_t i;
    i.raw = iter._raw;
    i.len = iter._len;
    i.next_off = iter._off;
    bson_iter_next(&i);

    is_end = !bson_iter_next(&i);

    iter._raw = i.raw;
    iter._len = i.len;
    iter._off = i.off;

    return *this;
}

view::iterator view::iterator::operator++(int) {
    iterator before(*this);
    operator++();
    return before;
}

bool view::iterator::operator==(const iterator& rhs) const {
    if (is_end && rhs.is_end) return true;
    if (is_end || rhs.is_end) return false;
    return iter == rhs.iter;

    return false;
}

bool view::iterator::operator!=(const iterator& rhs) const { return !(*this == rhs); }

view::iterator view::begin() const {
    bson_t b;
    bson_iter_t iter;

    bson_init_static(&b, buf, len);
    bson_iter_init(&iter, &b);
    bson_iter_next(&iter);

    return iterator(&iter);
}

bool view::has_key(const string_or_literal& key) const {
    return !((*this)[key] == bson::document::element{});
}

view::iterator view::end() const { return iterator(true); }

element view::operator[](const string_or_literal& key) const {
    bson_t b;
    bson_iter_t iter;

    bson_init_static(&b, buf, len);

    if (bson_iter_init_find(&iter, &b, key.c_str())) {
        return element(reinterpret_cast<const void*>(&iter));
    } else {
        return element{};
    }
}

view::view(const std::uint8_t* b, std::size_t l) : buf(b), len(l) {}

static uint8_t kDefaultView[5] = {5, 0, 0, 0, 0};

view::view() : buf(kDefaultView), len(5) {}

const std::uint8_t* view::get_buf() const { return buf; }
std::size_t view::get_len() const { return len; }

std::ostream& operator<<(std::ostream& out, const bson::document::view& view) {
    json_visitor v(out, false, 0);
    v.visit_value(types::b_document{view});

    return out;
}

}  // namespace document
}  // namespace bson
