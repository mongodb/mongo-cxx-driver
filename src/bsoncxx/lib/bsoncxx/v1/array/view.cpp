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

#include <bsoncxx/v1/array/view.hpp>

//

#include <bsoncxx/v1/exception.hpp>

#include <bsoncxx/v1/document/view.hh>
#include <bsoncxx/v1/element/view.hh>

#include <cstdint>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/itoa.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace array {

using code = v1::document::view::errc;

static_assert(is_regular<view>::value, "bsoncxx::v1::array::view must be regular");
static_assert(is_semitrivial<view>::value, "bsoncxx::v1::array::view must be semitrivial");

static_assert(is_regular<view::const_iterator>::value, "bsoncxx::v1::array::view::const_iterator must be regular");
static_assert(
    is_nothrow_moveable<view::const_iterator>::value,
    "bsoncxx::v1::array::view::const_iterator must be nothrow moveable");

view::const_iterator view::find(std::uint32_t i) const {
    if (!_view) {
        return this->cend();
    }

    bsoncxx::itoa key{i};

    bson_t bson;

    if (!bson_init_static(&bson, _view.data(), _view.length())) {
        throw v1::exception{code::invalid_data};
    }

    bson_iter_t iter;

    if (bson_iter_init_find_w_len(&iter, &bson, key.c_str(), static_cast<int>(key.length()))) {
        return const_iterator::internal::make_const_iterator(
            _view.data(), _view.length(), bson_iter_offset(&iter), bson_iter_key_len(&iter));
    }

    if (iter.err_off != 0) {
        throw v1::exception{code::invalid_data};
    }

    return this->end();
}

} // namespace array
} // namespace v1
} // namespace bsoncxx
