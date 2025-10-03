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

#include <mongocxx/private/scoped_bson.hh>

//

#include <cstdint>
#include <stdexcept>

#include <bsoncxx/private/bson.hh>

namespace mongocxx {

scoped_bson& scoped_bson::operator+=(scoped_bson_view other) {
    if (!_value) {
        throw std::logic_error{"mongocxx::scoped_bson::operator+=: this->data() == nullptr"};
    }

    if (!other) {
        throw std::logic_error{"mongocxx::scoped_bson::operator+=: other.data() == nullptr"};
    }

    auto const is_self_assignment = _value.data() == other.data();
    auto const is_bson_freeable = [&]() -> bool {
        auto const deleter_ptr = _value.get_deleter().target<decltype(&bson_free)>();
        return deleter_ptr && *deleter_ptr == &bson_free;
    }();

    // Copy is required for strong exception safety or allocator compatibility.
    if (is_self_assignment || !is_bson_freeable) {
        bson_t bson = BSON_INITIALIZER;

        if (bson_concat(&bson, this->bson()) && bson_concat(&bson, other.bson())) {
            *this = scoped_bson{&bson}; // Ownership transfer.
        } else {
            bson_destroy(&bson);
            throw std::logic_error{"mongocxx::scoped_bson::operator+=: bson_new_from_data failed"};
        }
    }

    // Compatible allocator: avoid an unnecessary copy by reusing the underlying BSON data.
    else {
        auto length = _value.length();

        // Temporarily acquire ownership of underlying BSON data.
        auto ptr = _value.release();

        std::uint8_t* data = ptr.get();

        // Conditional ownership transfer.
        auto bson = bson_new_from_buffer(&data, &length, nullptr, nullptr);

        // Ownership transfer failed.
        if (!bson) {
            bson_destroy(bson);
            _value = bsoncxx::v1::document::value{std::move(ptr)}; // Strong exception safety.
            throw std::logic_error{"mongocxx::scoped_bson::operator+=: bson_new_from_buffer failed"};
        }

        // Unset BSON_FLAG_NO_FREE: data is now owning.
        bson->flags = 0;

        // Ownership transfer succeeded: fully release data.
        (void)ptr.release();

        auto const ret = bson_concat(bson, other.bson());

        *this = scoped_bson{bson}; // Ownership transfer (success) + strong exception safety (failure).

        if (!ret) {
            throw std::logic_error{"mongocxx::scoped_bson::operator+=: bson_concat failed"};
        }
    }

    return *this;
}

} // namespace mongocxx
