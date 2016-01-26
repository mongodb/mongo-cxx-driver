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

#include <bsoncxx/oid.hpp>

#include <cstring>

#include <bson.h>

#include <bsoncxx/config/private/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

const oid::init_tag_t oid::init_tag{};

oid::oid() : _is_valid(false) {
}

oid::oid(init_tag_t) : _is_valid(true) {
    bson_oid_t oid;
    bson_oid_init(&oid, nullptr);

    std::memcpy(_bytes.data(), oid.bytes, sizeof(oid.bytes));
}

oid::oid(const string::view_or_value& str)
    : _is_valid(bson_oid_is_valid(str.data(), str.view().length())) {
    if (_is_valid) {
        bson_oid_t oid;
        bson_oid_init_from_string(&oid, str.terminated().data());
        memcpy(_bytes.data(), oid.bytes, _bytes.size());
    }
}

oid::oid(const char* bytes, std::size_t len) : _is_valid(len == 12) {
    if (_is_valid) {
        std::memcpy(_bytes.data(), bytes, _bytes.size());
    }
}

std::string oid::to_string() const {
    bson_oid_t oid;
    std::memcpy(oid.bytes, _bytes.data(), sizeof(oid.bytes));
    char str[25];

    bson_oid_to_string(&oid, str);

    return std::string(str);
}

oid::operator bool() const {
    return _is_valid;
}

std::time_t oid::get_time_t() const {
    bson_oid_t oid;
    std::memcpy(oid.bytes, _bytes.data(), sizeof(oid.bytes));

    return bson_oid_get_time_t(&oid);
}

const char* oid::bytes() const {
    return _bytes.data();
}

int oid_compare(const oid& lhs, const oid& rhs) {
    if (!lhs._is_valid || !rhs._is_valid) {
        if (lhs._is_valid) {
            return 1;
        } else if (rhs._is_valid) {
            return -1;
        } else {
            return 0;
        }
    }

    bson_oid_t lhs_oid;
    bson_oid_t rhs_oid;

    std::memcpy(lhs_oid.bytes, lhs.bytes(), sizeof(lhs_oid.bytes));
    std::memcpy(rhs_oid.bytes, rhs.bytes(), sizeof(rhs_oid.bytes));

    return bson_oid_compare(&lhs_oid, &rhs_oid);
}

bool operator<(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) < 0;
}

bool operator>(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) > 0;
}

bool operator<=(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) <= 0;
}

bool operator>=(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) >= 0;
}

bool operator==(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) == 0;
}

bool operator!=(const oid& lhs, const oid& rhs) {
    return oid_compare(lhs, rhs) != 0;
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
