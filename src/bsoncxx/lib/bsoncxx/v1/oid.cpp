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

#include <bsoncxx/v1/oid.hh>

//

#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <system_error>

#include <bsoncxx/private/bson.hh> // <winsock.h> via <bson/bson-compat.h>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {

using code = v1::oid::errc;

static_assert(is_regular<oid>::value, "bsoncxx::v1::oid must be regular");
static_assert(is_semitrivial<oid>::value, "bsoncxx::v1::oid must be semitrivial");

// _bytes: initialized with memcpy.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
oid::oid() {
#if defined(_WIN32)
    // Ensure the Winsock DLL is initialized prior to calling `gethostname` in `bsoncxx::v1::oid::oid()`:
    //  - bson_oid_init -> bson_context_get_default -> ... -> _bson_context_init_random -> gethostname.
    static struct WSAGuard {
        ~WSAGuard() {
            (void)WSACleanup();
        }

        WSAGuard(WSAGuard&&) = delete;
        WSAGuard& operator=(WSAGuard&) = delete;
        WSAGuard(WSAGuard const&) = delete;
        WSAGuard& operator=(WSAGuard const&) = delete;

        WSAGuard() {
            WSADATA wsaData;
            if (WSAStartup((MAKEWORD(2, 2)), &wsaData) != 0) {
                throw v1::exception{
                    WSAGetLastError(), std::system_category(), "WSAStartup() failed in bsoncxx::v1::oid::oid()"};
            }
        }
    } wsa_guard;
#endif

    bson_oid_t oid;
    bson_oid_init(&oid, nullptr);
    std::memcpy(_bytes.data(), oid.bytes, sizeof(oid.bytes));
}

// _bytes: initialized with memcpy.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
oid::oid(std::uint8_t const* bytes, std::size_t len) {
    if (!bytes) {
        throw v1::exception{code::null_bytes_ptr};
    }

    if (len != this->size()) {
        throw v1::exception{code::invalid_length};
    }

    std::memcpy(_bytes.data(), bytes, _bytes.size());
}

// _bytes: initialized with memcpy.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
oid::oid(v1::stdx::string_view str) {
    if (str.empty()) {
        throw v1::exception{code::empty_string};
    }

    if (!bson_oid_is_valid(str.data(), str.size())) {
        throw v1::exception{code::invalid_string};
    }

    bson_oid_t oid;
    bson_oid_init_from_string(&oid, str.data());
    std::memcpy(_bytes.data(), oid.bytes, _bytes.size());
}

std::string oid::to_string() const {
    bson_oid_t oid;
    std::memcpy(oid.bytes, _bytes.data(), sizeof(oid.bytes));
    std::array<char, 2u * k_oid_length + 1u> str = {}; // Two hex digits per byte + null terminator: 25 characters.
    bson_oid_to_string(&oid, str.data());
    return std::string(str.data());
}

std::time_t oid::get_time_t() const {
    bson_oid_t oid;
    std::memcpy(oid.bytes, _bytes.data(), sizeof(oid.bytes));
    return bson_oid_get_time_t(&oid);
}

int oid::compare(oid const& other) const {
    bson_oid_t lhs_oid;
    bson_oid_t rhs_oid;

    std::memcpy(lhs_oid.bytes, this->bytes(), k_oid_length);
    std::memcpy(rhs_oid.bytes, other.bytes(), k_oid_length);

    return bson_oid_compare(&lhs_oid, &rhs_oid);
}

std::error_category const& oid::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::oid";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::null_bytes_ptr:
                    return "bytes pointer must not be null";
                case code::invalid_length:
                    return "byte length must equal k_oid_length";
                case code::empty_string:
                    return "string must not be empty";
                case code::invalid_string:
                    return "string is not a valid ObjectID representation";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::null_bytes_ptr:
                    case code::invalid_length:
                    case code::empty_string:
                    case code::invalid_string:
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
                    case code::null_bytes_ptr:
                    case code::invalid_length:
                    case code::empty_string:
                    case code::invalid_string:
                        return type == condition::invalid_argument;

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

} // namespace v1
} // namespace bsoncxx
