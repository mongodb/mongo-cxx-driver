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

#include <bsoncxx/v1/decimal128.hpp>

//

#include <bsoncxx/v1/exception.hpp>

#include <climits>
#include <cstddef>
#include <string>
#include <system_error>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {

using code = v1::decimal128::errc;

static_assert(is_regular<decimal128>::value, "bsoncxx::v1::decimal128 must be regular");
static_assert(is_semitrivial<decimal128>::value, "bsoncxx::v1::decimal128 must be semitrivial");

decimal128::decimal128(v1::stdx::string_view sv) {
    if (sv.empty()) {
        throw v1::exception{code::empty_string};
    }

    if (sv.size() > std::size_t{INT_MAX}) {
        throw v1::exception{code::invalid_string_length};
    }

    bson_decimal128_t d128;

    if (!bson_decimal128_from_string_w_len(sv.data(), static_cast<int>(sv.size()), &d128)) {
        throw v1::exception{code::invalid_string_data};
    }

    _high = d128.high;
    _low = d128.low;
}

std::string decimal128::to_string() const {
    bson_decimal128_t d128;
    d128.high = _high;
    d128.low = _low;
    char str[BSON_DECIMAL128_STRING];
    bson_decimal128_to_string(&d128, str);
    return {str};
}

std::error_category const& decimal128::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::decimal128";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::empty_string:
                    return "string must not be empty";
                case code::invalid_string_length:
                    return "length of string is too long (exceeds INT_MAX)";
                case code::invalid_string_data:
                    return "string is not a valid Decimal128 representation";
                default:
                    return "unknown: " + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::empty_string:
                    case code::invalid_string_length:
                    case code::invalid_string_data:
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
                    case code::empty_string:
                    case code::invalid_string_length:
                    case code::invalid_string_data:
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
