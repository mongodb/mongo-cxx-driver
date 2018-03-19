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
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace test_util {

namespace {

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

}  // namespace test_util
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
