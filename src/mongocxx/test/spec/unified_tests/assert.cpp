// Copyright 2020-present MongoDB Inc.
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

#include "assert.hh"

#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>

#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

using namespace bsoncxx;
using namespace mongocxx;

using assert::to_string;
using bsoncxx::types::bson_value::value;

namespace {

/// When we are matching two documents, we walk down them recursively. To aid test debugging, store
/// a path as we go, in this variable:
thread_local std::vector<std::string> S_match_doc_path;

/**
 * @brief Declare that the current scope is stepping into the given string key of a key-value
 * mapping
 */
struct match_scope_doc_key {
    match_scope_doc_key(bsoncxx::stdx::string_view key) {
        S_match_doc_path.push_back("/" + std::string(key));
    }

    ~match_scope_doc_key() {
        S_match_doc_path.pop_back();
    }
};

/**
 * @brief Declare that the current scope is stepping into the given index of an array
 */
struct match_scope_array_idx {
    match_scope_array_idx(int idx) {
        S_match_doc_path.push_back("[" + std::to_string(idx) + "]");
    }

    ~match_scope_array_idx() {
        S_match_doc_path.pop_back();
    }
};

/**
 * @brief Obtain a string representation of the current path in the matched object
 */
std::string match_doc_current_path() noexcept {
    return std::accumulate(S_match_doc_path.cbegin(), S_match_doc_path.cend(), std::string());
}

}  // namespace

std::string binary_to_string(types::b_binary binary) {
    std::stringstream ss;
    ss << std::hex;
    for (auto&& byte : std::vector<unsigned int>(binary.bytes, binary.bytes + binary.size)) {
        ss << std::setw(2) << std::setfill('0') << byte;
    }
    return ss.str();
}

std::string assert::to_string(types::bson_value::view_or_value val) {
    switch (val.view().type()) {
        case bsoncxx::type::k_string:
            return string::to_string(val.view().get_string().value);
        case bsoncxx::type::k_int32:
            return std::to_string(val.view().get_int32().value);
        case bsoncxx::type::k_int64:
            return std::to_string(val.view().get_int64().value);
        case bsoncxx::type::k_document:
            return to_json(val.view().get_document().value);
        case bsoncxx::type::k_array:
            return to_json(val.view().get_array().value);
        case bsoncxx::type::k_oid:
            return val.view().get_oid().value.to_string();
        case bsoncxx::type::k_binary:
            return binary_to_string(val.view().get_binary());
        case bsoncxx::type::k_bool:
            return val.view().get_bool().value ? "true" : "false";
        case bsoncxx::type::k_code:
            return string::to_string(val.view().get_code().code);
        case bsoncxx::type::k_codewscope:
            return "code={" + string::to_string(val.view().get_codewscope().code) + "}, scope={" +
                   to_json(val.view().get_codewscope().scope) + "}";
        case bsoncxx::type::k_date:
            return std::to_string(val.view().get_date().value.count());
        case bsoncxx::type::k_double:
            return std::to_string(val.view().get_double());
        case bsoncxx::type::k_null:
            return "null";
        case bsoncxx::type::k_undefined:
            return "undefined";
        case bsoncxx::type::k_timestamp:
            return "timestamp={" + std::to_string(val.view().get_timestamp().timestamp) +
                   "}, increment={" + std::to_string(val.view().get_timestamp().increment) + "}";
        case bsoncxx::type::k_regex:
            return "regex={" + string::to_string(val.view().get_regex().regex) + "}, options={" +
                   string::to_string(val.view().get_regex().options) + "}";
        case bsoncxx::type::k_minkey:
            return "minkey";
        case bsoncxx::type::k_maxkey:
            return "maxkey";
        case bsoncxx::type::k_decimal128:
            return val.view().get_decimal128().value.to_string();
        case bsoncxx::type::k_symbol:
            return string::to_string(val.view().get_symbol().symbol);
        case bsoncxx::type::k_dbpointer:
            return val.view().get_dbpointer().value.to_string();
        default:
            MONGOCXX_UNREACHABLE;
    }
}

template <typename Element>
type to_type(const Element& type) {
    auto type_str = string::to_string(type.get_string().value);
    if (type_str == "binData")
        return bsoncxx::type::k_binary;
    if (type_str == "long")
        return bsoncxx::type::k_int64;
    if (type_str == "int")
        return bsoncxx::type::k_int32;
    if (type_str == "objectId")
        return bsoncxx::type::k_oid;
    if (type_str == "object")
        return bsoncxx::type::k_document;
    if (type_str == "date")
        return bsoncxx::type::k_date;
    throw std::logic_error{"unrecognized element type '" + type_str + "'"};
}

bool is_set(types::bson_value::view val) {
    return val.type() != bsoncxx::type::k_null;
}

void special_operator(types::bson_value::view actual, document::view expected, entity::map& map) {
    auto op = *expected.begin();
    REQUIRE(string::to_string(op.key()).rfind("$$", 0) == 0);  // assert special operator

    CAPTURE(match_doc_current_path());
    if (string::to_string(op.key()) == "$$type") {
        auto actual_t = actual.type();
        if (op.type() == type::k_string) {
            REQUIRE(actual_t == to_type(op));
            return;
        }

        REQUIRE(op.type() == type::k_array);
        for (auto t : op.get_array().value) {
            if (actual_t == to_type(t)) {
                return;
            }
        }

        FAIL("type '" + to_string(actual_t) + "' expected in array '" +
             to_json(op.get_array().value) + "'");
    } else if (string::to_string(op.key()) == "$$unsetOrMatches") {
        auto val = op.get_value();
        if (is_set(actual))
            assert::matches(actual, val, map, false);
    } else if (string::to_string(op.key()) == "$$sessionLsid") {
        auto id = string::to_string(op.get_string().value);
        const auto& type = map.type(id);
        if (type == typeid(client_session)) {
            REQUIRE(actual == map.get_client_session(id).id());
        } else {
            // If the map does not contain the client session, then its ID should have been cached
            // as a BSON value.
            REQUIRE(type == typeid(value));
            REQUIRE(actual == map.get_value(id));
        }
    } else if (string::to_string(op.key()) == "$$matchesEntity") {
        auto name = string::to_string(op.get_string().value);
        REQUIRE(actual == map.get_value(name));
    } else if (string::to_string(op.key()) == "$$exists") {
        const bool should_exist = op.get_bool();
        const bool does_exist = actual.type() != bsoncxx::type::k_null;
        if (does_exist && !should_exist) {
            FAIL_CHECK("Expected this document element to be absent, but it is present");
        } else if (!does_exist && should_exist) {
            FAIL_CHECK("Expected this document element to be present, but it is absent");
        }
    } else if (string::to_string(op.key()) == "$$matchesHexBytes") {
        REQUIRE(actual.type() == bsoncxx::type::k_binary);

        auto expected_bytes = test_util::convert_hex_string_to_bytes(op.get_value().get_string());
        decltype(expected_bytes) actual_bytes(actual.get_binary().bytes, actual.get_binary().size);

        REQUIRE(actual_bytes == expected_bytes);
    } else {
        FAIL("unrecognized special operator '" + string::to_string(op.key()) + "'");
    }
}

template <typename T>
bool is_special(T doc) {
    return doc.type() == type::k_document && test_util::size(doc.get_document().value) == 1 &&
           string::to_string(doc.get_document().value.begin()->key()).rfind("$$", 0) == 0;
}

void matches_document(types::bson_value::view actual,
                      types::bson_value::view expected,
                      entity::map& map,
                      bool is_root) {
    if (is_special(expected)) {
        special_operator(actual, expected.get_document(), map);
        return;
    }

    REQUIRE(actual.type() == bsoncxx::type::k_document);
    auto actual_doc = actual.get_document().value;
    auto extra_fields = test_util::size(actual_doc);

    for (auto&& kvp : expected.get_document().value) {
        match_scope_doc_key scope_key{string::to_string(kvp.key())};
        if (is_special(kvp)) {
            if (!actual_doc[kvp.key()]) {
                special_operator(value(nullptr), kvp.get_document(), map);
                continue;
            }
        }

        REQUIRE(actual_doc[kvp.key()]);
        assert::matches(actual_doc[kvp.key()].get_value(), kvp.get_value(), map, false);
        --extra_fields;
    }

    REQUIRE((is_root || extra_fields == 0));
}

void matches_array(types::bson_value::view actual,
                   types::bson_value::view expected,
                   entity::map& map) {
    REQUIRE(actual.type() == bsoncxx::type::k_array);

    auto actual_arr = actual.get_array().value;
    auto expected_arr = expected.get_array().value;

    REQUIRE(test_util::size(actual_arr) == test_util::size(expected_arr));
    int idx = 0;
    for (auto a = actual_arr.begin(), e = expected_arr.begin(); e != expected_arr.end();
         e++, a++, ++idx) {
        match_scope_array_idx scope_idx{idx};
        assert::matches(a->get_value(), e->get_value(), map, false);
    }
}

void assert::matches(types::bson_value::view actual,
                     types::bson_value::view expected,
                     entity::map& map,
                     bool is_root) {
    switch (expected.type()) {
        case bsoncxx::type::k_document:
            matches_document(actual, expected, map, is_root);
            return;
        case bsoncxx::type::k_array:
            matches_array(actual, expected, map);
            return;
        case bsoncxx::type::k_int32:
        case bsoncxx::type::k_int64:
        case bsoncxx::type::k_double: {
            CAPTURE(is_root,
                    to_string(actual.type()),
                    to_string(expected.type()),
                    to_string(actual),
                    to_string(expected),
                    match_doc_current_path());
            REQUIRE(test_util::is_numeric(actual));
            REQUIRE(test_util::as_double(expected) == test_util::as_double(actual));
            return;
        }
        default: {
            CAPTURE(is_root,
                    to_string(actual.type()),
                    to_string(expected.type()),
                    to_string(actual),
                    to_string(expected),
                    match_doc_current_path());
            REQUIRE(actual.type() == expected.type());
            REQUIRE(actual == expected);
        }
    }
}
