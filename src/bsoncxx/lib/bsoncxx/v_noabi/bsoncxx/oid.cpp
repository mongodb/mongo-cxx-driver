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

#include <bsoncxx/oid.hpp>

//

#include <bsoncxx/v1/exception.hpp>

#include <bsoncxx/v1/oid.hh>

#include <cstring>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {

static_assert(is_explicitly_convertible<oid&&, v1::oid>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<oid const&, v1::oid>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::oid&&, oid>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::oid const&, oid>::value, "v1 -> v_noabi must be implicit");

oid::oid() : _oid{v1::oid::internal::make_oid_for_overwrite()} {
    try {
        _oid = v1::oid{};
    } catch (...) {
        // For backward compatibility, ignore any exceptions and initialize anyways.
        bson_oid_t oid;
        bson_oid_init(&oid, nullptr);

        auto& _bytes = v1::oid::internal::bytes(_oid);
        std::memcpy(_bytes.data(), oid.bytes, sizeof(oid.bytes));
    }
}

oid::oid(stdx::string_view const& str) try : _oid{str} {
} catch (v1::exception const&) {
    throw v_noabi::exception{v_noabi::error_code::k_invalid_oid};
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): v1 vs. v_noabi compatibility.
oid::oid(char const* bytes, std::size_t len) try : _oid{reinterpret_cast<std::uint8_t const*>(bytes), len} {
} catch (v1::exception const&) {
    throw v_noabi::exception{v_noabi::error_code::k_invalid_oid};
}

int oid_compare(oid const& lhs, oid const& rhs) {
    return lhs._oid.compare(rhs._oid);
}

} // namespace v_noabi
} // namespace bsoncxx
