// Copyright 2016 MongoDB Inc.
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

#include <mongocxx/test_util/client_helpers.hh>

#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/stdx.hpp>
#include <third_party/catch/include/catch.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace test_util {

namespace {

using namespace mongocxx;
using namespace bsoncxx;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

std::vector<std::int32_t> parse_version(std::string version) {
    std::vector<std::int32_t> elements;
    std::stringstream ss{version};
    std::string element;

    while (std::getline(ss, element, '.')) {
        elements.push_back(std::stoi(element));
    }

    return elements;
}

bsoncxx::document::value transform_document_recursive(bsoncxx::document::view view,
                                                      const xformer_t& fcn,
                                                      bsoncxx::builder::basic::array* context);

bsoncxx::array::value transform_array(bsoncxx::array::view view,
                                      const xformer_t& fcn,
                                      bsoncxx::builder::basic::array* context) {
    bsoncxx::builder::basic::array builder;

    for (auto&& element : view) {
        // Array elements are passed with disengaged key.
        auto transformed = fcn({stdx::nullopt, element.get_value()}, context);

        // Omit element if transformed is disengaged.
        if (!transformed) {
            continue;
        }

        auto v = transformed->second;

        // For documents and arrays, append with recursive transform.
        // Otherwise, append the transformed value.
        switch (v.type()) {
            case bsoncxx::type::k_document:
                builder.append(transform_document_recursive(v.get_document().value, fcn, context));
                break;

            case bsoncxx::type::k_array:
                builder.append(transform_array(v.get_array().value, fcn, context));
                break;

            default:
                builder.append(v);
                break;
        }
    }

    return builder.extract();
}

bsoncxx::document::value transform_document_recursive(bsoncxx::document::view view,
                                                      const xformer_t& fcn,
                                                      bsoncxx::builder::basic::array* context) {
    bsoncxx::builder::basic::document builder;

    for (auto&& element : view) {
        auto transformed = fcn({element.key(), element.get_value()}, context);

        // Omit element if transformed is disengaged.
        if (!transformed) {
            continue;
        }

        // For document elements, it's an error if key is not returned.
        if (!transformed->first) {
            throw logic_error{error_code::k_invalid_parameter};
        }

        auto k = *(transformed->first);
        auto v = transformed->second;

        // For documents and arrays, append with transformed key and recursive
        // transform.  Otherwise, append the transformed key and value.
        switch (v.type()) {
            case bsoncxx::type::k_document:
                builder.append(bsoncxx::builder::basic::kvp(
                    k, transform_document_recursive(v.get_document().value, fcn, context)));
                break;

            case bsoncxx::type::k_array:
                builder.append(bsoncxx::builder::basic::kvp(
                    k, transform_array(v.get_array().value, fcn, context)));
                break;

            default:
                builder.append(bsoncxx::builder::basic::kvp(k, v));
                break;
        }
    }

    return builder.extract();
}

}  // namespace

std::int32_t compare_versions(std::string version1, std::string version2) {
    std::vector<std::int32_t> v1 = parse_version(version1);
    std::vector<std::int32_t> v2 = parse_version(version2);

    for (std::size_t i = 0; i < std::min(v1.size(), v2.size()); ++i) {
        std::int32_t difference = v1[i] - v2[i];

        if (difference != 0) {
            return difference;
        }
    }

    return 0;
}

bool newer_than(const client& client, std::string version) {
    auto server_version = get_server_version(client);
    return (compare_versions(server_version, version) >= 0);
}

std::basic_string<std::uint8_t> convert_hex_string_to_bytes(stdx::string_view hex) {
    std::basic_string<std::uint8_t> bytes;

    // Convert each pair of hexadecimal digits into a number and store it in the array.
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        stdx::string_view sub = hex.substr(i, 2);
        bytes.push_back(
            static_cast<std::uint8_t>(std::stoi(bsoncxx::string::to_string(sub), nullptr, 16)));
    }

    return bytes;
}

std::int32_t get_max_wire_version(const client& client) {
    auto reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto max_wire_version = reply.view()["maxWireVersion"];
    if (!max_wire_version) {
        // If wire version is not available (i.e. server version too old), it is assumed to be
        // zero.
        return 0;
    }
    if (max_wire_version.type() != bsoncxx::type::k_int32) {
        throw operation_exception{error_code::k_server_response_malformed};
    }
    return max_wire_version.get_int32().value;
}

std::string get_server_version(const client& client) {
    bsoncxx::builder::basic::document server_status{};
    server_status.append(bsoncxx::builder::basic::kvp("serverStatus", 1));
    bsoncxx::document::value output = client["test"].run_command(server_status.extract());

    return bsoncxx::string::to_string(output.view()["version"].get_utf8().value);
}

std::string replica_set_name(const client& client) {
    auto reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto name = reply.view()["setName"];
    if (name) {
        return bsoncxx::string::to_string(name.get_utf8().value);
    }

    return "";
}

bool is_replica_set(const client& client) {
    auto reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    return static_cast<bool>(reply.view()["setName"]);
}

std::string get_topology(const client& client) {
    auto reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    if (reply.view()["setName"]) {
        return "replicaset";
    } else if (reply.view()["msg"] &&
               std::string(reply.view()["msg"].get_utf8().value) == "isdbgrid") {
        return "sharded";
    } else {
        return "single";
    }
}

stdx::optional<bsoncxx::document::value> parse_test_file(std::string path) {
    std::stringstream stream;
    std::ifstream test_file{path};

    if (test_file.bad()) {
        return {};
    }

    stream << test_file.rdbuf();
    return bsoncxx::from_json(stream.str());
}

bool supports_collation(const client& client) {
    return get_max_wire_version(client) >= 5;
}

bsoncxx::document::value transform_document(bsoncxx::document::view view, const xformer_t& fcn) {
    bsoncxx::builder::basic::array context;

    return transform_document_recursive(view, fcn, &context);
}

double as_double(bsoncxx::types::bson_value::view value) {
    if (value.type() == type::k_int32) {
        return static_cast<double>(value.get_int32());
    }
    if (value.type() == type::k_int64) {
        return static_cast<double>(value.get_int64());
    }
    if (value.type() == type::k_double) {
        return static_cast<double>(value.get_double());
    }

    throw std::logic_error{"could not convert type " + bsoncxx::to_string(value.type()) +
                           " to double"};
}

bool is_numeric(types::bson_value::view value) {
    return value.type() == type::k_int32 || value.type() == type::k_int64 ||
           value.type() == type::k_double;
}

stdx::optional<type> is_type_operator(types::bson_value::view value) {
    if (value.type() == type::k_document && value.get_document().value["$$type"]) {
        auto t = value.get_document().value["$$type"].get_utf8().value;
        if (t.compare("binData") == 0) {
            return {type::k_binary};
        } else if (t.compare("long") == 0) {
            return {type::k_int64};
        }
        throw std::logic_error{"unsupported type for $$type"};
    }
    return {};
}

bool matches(types::bson_value::view main,
             types::bson_value::view pattern,
             match_visitor visitor_fn) {
    if (auto t = is_type_operator(pattern)) {
        return t == main.type() ? true : false;
    }

    if (is_numeric(pattern) && as_double(pattern) == 42) {
        return true;
    }

    // Different numeric types are considered equal.
    if (is_numeric(main) && is_numeric(pattern) && as_double(main) == as_double(pattern)) {
        return true;
    }

    if (main.type() == type::k_document) {
        // the value '42' acts as placeholders for "any value"
        if (pattern.type() == type::k_utf8 && 0 == pattern.get_utf8().value.compare("42")) {
            return true;
        }

        document::view main_view = main.get_document().value;
        for (auto&& el : pattern.get_document().value) {
            match_action action = match_action::k_proceed;
            if (visitor_fn) {
                stdx::optional<types::bson_value::view> main_value;
                if (main_view.find(el.key()) != main_view.end()) {
                    main_value = main_view[el.key()].get_value();
                }
                action = visitor_fn(el.key(), main_value, el.get_value());
            }

            if (action == match_action::k_skip) {
                continue;
            }

            if (action == match_action::k_not_equal) {
                return false;
            }

            // For write errors, only check for existence.
            if (el.key().compare("writeErrors") == 0) {
                if (main_view.find(el.key()) == main_view.end()) {
                    return false;
                }
                continue;
            }
            // The C++ driver does not include insertedIds as part of the bulk write result.
            if (el.key().compare("insertedIds") == 0) {
                if (main_view.find(el.key()) == main_view.end()) {
                    return false;
                }
                continue;
            }
            if (main_view.find(el.key()) == main_view.end()) {
                return el.get_value().type() == type::k_null;
            }
            if (!matches(main_view[el.key()].get_value(), el.get_value(), visitor_fn)) {
                return false;
            }
        }
        return true;
    }

    if (main.type() == type::k_array) {
        array::view main_array = main.get_array().value;
        array::view pattern_array = pattern.get_array().value;

        if (main_array.length() < pattern_array.length()) {
            return false;
        }

        auto main_iter = main_array.begin();
        for (auto&& el : pattern_array) {
            if (!matches((*main_iter).get_value(), el.get_value(), visitor_fn)) {
                return false;
            }
            main_iter++;
        }
        return true;
    }

    return main == pattern;
}

bool matches(document::view doc, document::view pattern, match_visitor visitor_fn) {
    return matches(types::bson_value::view{types::b_document{doc}},
                   types::bson_value::view{types::b_document{pattern}},
                   visitor_fn);
}

std::string tolowercase(stdx::string_view view) {
    std::string out;
    out.reserve(view.size());
    for (size_t i = 0; i < view.length(); i++) {
        out += static_cast<char>(::tolower(view[i]));
    }
    return out;
}

void check_outcome_collection(mongocxx::collection* coll, bsoncxx::document::view expected) {
    read_concern rc;
    rc.acknowledge_level(read_concern::level::k_local);
    auto old_rc = coll->read_concern();
    coll->read_concern(rc);

    options::find options{};
    options.sort(make_document(kvp("_id", 1)));

    using namespace std;
    cursor actual = coll->find({}, options);
    auto expected_data = expected["data"].get_array().value;
    REQUIRE(equal(begin(expected_data),
                  end(expected_data),
                  begin(actual),
                  [&](const bsoncxx::array::element& ele, const document::view& doc) {
                      REQUIRE_BSON_MATCHES(doc, ele.get_document().value);
                      return true;
                  }));
    REQUIRE(begin(actual) == end(actual));
    coll->read_concern(old_rc);
}

bool server_has_sessions(const client& conn) {
    auto result = conn["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto result_view = result.view();

    if (result_view["logicalSessionTimeoutMinutes"]) {
        return true;
    }

    WARN("skip: server does not support sessions");
    return false;
}

bool should_run_client_side_encryption_test(void) {
#ifndef MONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION
    WARN("linked libmongoc does not support client side encryption - skipping tests");
    return false;
#endif

    auto access_key = std::getenv("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY");
    auto key_id = std::getenv("MONGOCXX_TEST_AWS_ACCESS_KEY_ID");

    if (!access_key || !key_id) {
        WARN(
            "Skipping tests. Please set environment variables to enable client side encryption "
            "tests:\n"
            "\tMONGOCXX_TEST_AWS_SECRET_ACCESS_KEY\n"
            "\tMONGOCXX_TEST_AWS_ACCESS_KEY_ID\n\n");
        return false;
    }

    return true;
}

}  // namespace test_util
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
