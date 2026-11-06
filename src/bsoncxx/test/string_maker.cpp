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

#include <bsoncxx/test/string_maker.hh>

//

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/decimal128.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>
#include <bsoncxx/v1/exception.hpp>
#include <bsoncxx/v1/oid.hpp>
#include <bsoncxx/v1/types/id.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <bsoncxx/test/v_noabi/to_string.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#if defined(BSONCXX_POLY_USE_IMPLS)
#include <bsoncxx/v1/stdx/string_view.hpp>
#endif

#include <chrono>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <system_error>

#include <catch2/catch_test_macros.hpp>

std::string Catch::StringMaker<bsoncxx::oid>::convert(bsoncxx::oid const& value) {
    return value.to_string();
}

std::string Catch::StringMaker<bsoncxx::document::view>::convert(bsoncxx::document::view const& value) {
    return bsoncxx::to_json(value, bsoncxx::ExtendedJsonMode::k_relaxed);
}

std::string Catch::StringMaker<bsoncxx::document::view_or_value>::convert(
    bsoncxx::document::view_or_value const& value) {
    return bsoncxx::test::stringify(value.view());
}

std::string Catch::StringMaker<bsoncxx::document::value>::convert(bsoncxx::document::value const& value) {
    return bsoncxx::test::stringify(value.view());
}

std::string Catch::StringMaker<bsoncxx::types::bson_value::view>::convert(
    bsoncxx::types::bson_value::view const& value) {
    using bsoncxx::to_string;
    return '{' + to_string(value.type()) + ": " + to_string(value) + '}';
}

std::string Catch::StringMaker<bsoncxx::types::bson_value::value>::convert(
    bsoncxx::types::bson_value::value const& value) {
    return bsoncxx::test::stringify(value.view());
}

std::string Catch::StringMaker<bsoncxx::types::bson_value::view_or_value>::convert(
    bsoncxx::types::bson_value::view_or_value const& value) {
    return bsoncxx::test::stringify(value.view());
}

std::string Catch::StringMaker<bsoncxx::detail::strong_ordering>::convert(bsoncxx::detail::strong_ordering o) {
    if (o < nullptr) {
        return "[less-than]";
    } else if (o > nullptr) {
        return "[greater-than]";
    } else {
        return "[equal/equivalent]";
    }
}

std::string Catch::StringMaker<std::error_category>::convert(std::error_category const& value) {
    return value.name();
}

std::string Catch::StringMaker<std::error_condition>::convert(std::error_condition const& value) {
    std::string res;

    res += value.category().name();
    res += ':';
    res += bsoncxx::test::stringify(value.value());

    return res;
}

std::string Catch::StringMaker<bsoncxx::v1::oid>::convert(bsoncxx::v1::oid const& value) {
    return value.to_string();
}

std::string Catch::StringMaker<bsoncxx::v1::decimal128>::convert(bsoncxx::v1::decimal128 const& value) {
    return value.to_string();
}

std::string Catch::StringMaker<bsoncxx::v1::array::view>::convert(bsoncxx::v1::array::view const& value) try {
    if (!value) {
        return "invalid";
    }

    auto const end = value.end();
    auto iter = value.begin();

    if (iter == end) {
        return "[]";
    }

    std::string res;
    res += '[';
    res += bsoncxx::test::stringify(iter->type_view());
    for (++iter; iter != end; ++iter) {
        res += ", ";
        res += bsoncxx::test::stringify(iter->type_view());
    }
    res += ']';
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    WARN("exception during stringification: " << ex.what());
    if (ex.code() == bsoncxx::v1::document::view::errc::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}

std::string Catch::StringMaker<bsoncxx::v1::document::view>::convert(bsoncxx::v1::document::view const& value) try {
    if (!value) {
        return "invalid";
    }

    auto const end = value.end();
    auto iter = value.begin();

    if (iter == end) {
        return "{}";
    }

    std::string res;
    res += '{';
    res += bsoncxx::test::stringify(*iter);
    for (++iter; iter != end; ++iter) {
        res += ", ";
        res += bsoncxx::test::stringify(*iter);
    }
    res += '}';
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    WARN("exception during stringification: " << ex.what());
    if (ex.code() == bsoncxx::v1::document::view::errc::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}

std::string Catch::StringMaker<bsoncxx::v1::document::view::const_iterator>::convert(
    bsoncxx::v1::document::view::const_iterator const& value) {
    if (auto const e = *value) {
        return bsoncxx::test::stringify(e);
    }

    return "end";
}

std::string Catch::StringMaker<bsoncxx::v1::element::view>::convert(bsoncxx::v1::element::view const& value) try {
    if (!value) {
        return "invalid";
    }

    std::string res;
    res += bsoncxx::test::stringify(value.key());
    res += ": ";
    res += bsoncxx::test::stringify(value.type_view());
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    using code = bsoncxx::v1::element::view::errc;

    if (ex.code() == code::invalid_view || ex.code() == code::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}

std::string Catch::StringMaker<bsoncxx::v1::types::id>::convert(bsoncxx::v1::types::id const& value) {
    return "k_" + to_string(value);
}

std::string Catch::StringMaker<bsoncxx::v1::types::binary_subtype>::convert(
    bsoncxx::v1::types::binary_subtype const& value) {
    return "k_" + to_string(value);
}

#if defined(BSONCXX_POLY_USE_IMPLS)
std::string Catch::StringMaker<bsoncxx::v1::stdx::string_view>::convert(bsoncxx::v1::stdx::string_view const& value) {
    return bsoncxx::test::stringify(std::string{value});
}
#endif

std::string Catch::StringMaker<bsoncxx::v1::types::view>::convert(bsoncxx::v1::types::view const& value) {
#pragma push_macro("X")
#undef X
#define X(_name, _value)                    \
    case bsoncxx::v1::types::id::k_##_name: \
        return bsoncxx::test::stringify(value.get_##_name());

    switch (value.type_id()) {
        BSONCXX_V1_TYPES_XMACRO(X)

        default:
            return "?";
    }
#pragma pop_macro("X")
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_binary>::convert(bsoncxx::v1::types::b_binary const& value) {
    if (!value.bytes || value.size == 0u) {
        return "empty";
    }

    std::ostringstream oss;
    oss << std::hex;
    auto const end = value.bytes + value.size;
    for (auto const* ptr = value.bytes; ptr != end; ++ptr) {
        oss << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(*ptr);
    }
    return std::move(oss).str();
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_date>::convert(bsoncxx::v1::types::b_date const& value) {
    using time_point = std::chrono::time_point<std::chrono::system_clock>;
    return bsoncxx::test::stringify(time_point{value.value});
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_regex>::convert(bsoncxx::v1::types::b_regex const& value) {
    std::string res;
    res += R"({"pattern": )";
    res += bsoncxx::test::stringify(value.regex);
    res += R"(, "options": )";
    res += bsoncxx::test::stringify(value.options);
    res += '}';
    return res;
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_dbpointer>::convert(bsoncxx::v1::types::b_dbpointer const& value) {
    std::string res;
    res += R"({"$ref": )";
    res += bsoncxx::test::stringify(value.collection);
    res += R"(, "$oid": )";
    res += bsoncxx::test::stringify(value.value);
    res += '}';
    return res;
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_codewscope>::convert(
    bsoncxx::v1::types::b_codewscope const& value) {
    std::string res;
    res += R"({"code": )";
    res += bsoncxx::test::stringify(value.code);
    res += R"(, "scope": )";
    res += bsoncxx::test::stringify(value.scope);
    res += '}';
    return res;
}

std::string Catch::StringMaker<bsoncxx::v1::types::b_timestamp>::convert(bsoncxx::v1::types::b_timestamp const& value) {
    std::string res;
    res += R"({"t": )";
    res += bsoncxx::test::stringify(value.timestamp);
    res += R"(, "i": )";
    res += bsoncxx::test::stringify(value.increment);
    res += '}';
    return res;
}
