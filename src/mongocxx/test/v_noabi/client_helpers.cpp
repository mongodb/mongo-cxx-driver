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

#include <mongocxx/test/v_noabi/client_helpers.hh>

#include <algorithm>
#include <fstream>
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
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/private/config/config.hh>
#include <mongocxx/private/mongoc.hh>

#include <bsoncxx/test/catch.hh>

namespace mongocxx {
namespace test_util {

namespace {

using namespace mongocxx;
using namespace bsoncxx;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

namespace {
// These frequently used network calls are cached to avoid bottlenecks during tests.
document::value get_is_master() {
    static auto reply = []() {
        auto client = mongocxx::client{mongocxx::uri{}, test_util::add_test_server_api()};
        return client["admin"].run_command(make_document(kvp("isMaster", 1)));
    }();
    return reply;
}

document::value get_server_status() {
    static auto status = []() {
        auto client = mongocxx::client{mongocxx::uri{}, test_util::add_test_server_api()};
        return client["admin"].run_command(make_document(kvp("serverStatus", 1)));
    }();
    return status;
}

bsoncxx::stdx::optional<document::value> get_shards() {
    static auto shards = []() {
        auto client = mongocxx::client{mongocxx::uri{}, test_util::add_test_server_api()};
        return client["config"]["shards"].find_one({});
    }();
    return (shards) ? shards.value() : bsoncxx::stdx::optional<document::value>{};
}
} // namespace

std::vector<std::int32_t> parse_version(std::string version) {
    std::vector<std::int32_t> elements;
    std::stringstream ss{version};
    std::string element;

    while (std::getline(ss, element, '.')) {
        elements.push_back(std::stoi(element));
    }

    return elements;
}

bsoncxx::document::value transform_document_recursive(
    bsoncxx::document::view view,
    xformer_t const& fcn,
    bsoncxx::builder::basic::array* context);

bsoncxx::array::value
transform_array(bsoncxx::array::view view, xformer_t const& fcn, bsoncxx::builder::basic::array* context) {
    bsoncxx::builder::basic::array builder;

    for (auto&& element : view) {
        // Array elements are passed with disengaged key.
        auto transformed = fcn({bsoncxx::stdx::nullopt, element.get_value()}, context);

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

            case bsoncxx::type::k_double:
            case bsoncxx::type::k_string:
            case bsoncxx::type::k_binary:
            case bsoncxx::type::k_undefined:
            case bsoncxx::type::k_oid:
            case bsoncxx::type::k_bool:
            case bsoncxx::type::k_date:
            case bsoncxx::type::k_null:
            case bsoncxx::type::k_regex:
            case bsoncxx::type::k_dbpointer:
            case bsoncxx::type::k_code:
            case bsoncxx::type::k_symbol:
            case bsoncxx::type::k_codewscope:
            case bsoncxx::type::k_int32:
            case bsoncxx::type::k_timestamp:
            case bsoncxx::type::k_int64:
            case bsoncxx::type::k_decimal128:
            case bsoncxx::type::k_maxkey:
            case bsoncxx::type::k_minkey:
            default:
                builder.append(v);
                break;
        }
    }

    return builder.extract();
}

bsoncxx::document::value transform_document_recursive(
    bsoncxx::document::view view,
    xformer_t const& fcn,
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
                builder.append(
                    bsoncxx::builder::basic::kvp(
                        k, transform_document_recursive(v.get_document().value, fcn, context)));
                break;

            case bsoncxx::type::k_array:
                builder.append(bsoncxx::builder::basic::kvp(k, transform_array(v.get_array().value, fcn, context)));
                break;

            case bsoncxx::type::k_double:
            case bsoncxx::type::k_string:
            case bsoncxx::type::k_binary:
            case bsoncxx::type::k_undefined:
            case bsoncxx::type::k_oid:
            case bsoncxx::type::k_bool:
            case bsoncxx::type::k_date:
            case bsoncxx::type::k_null:
            case bsoncxx::type::k_regex:
            case bsoncxx::type::k_dbpointer:
            case bsoncxx::type::k_code:
            case bsoncxx::type::k_symbol:
            case bsoncxx::type::k_codewscope:
            case bsoncxx::type::k_int32:
            case bsoncxx::type::k_timestamp:
            case bsoncxx::type::k_int64:
            case bsoncxx::type::k_decimal128:
            case bsoncxx::type::k_maxkey:
            case bsoncxx::type::k_minkey:
            default:
                builder.append(bsoncxx::builder::basic::kvp(k, v));
                break;
        }
    }

    return builder.extract();
}

} // namespace

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

bool newer_than(std::string version) {
    auto server_version = get_server_version();
    return (compare_versions(server_version, version) >= 0);
}

std::vector<std::uint8_t> convert_hex_string_to_bytes(bsoncxx::stdx::string_view hex) {
    std::vector<std::uint8_t> bytes;

    // Convert each pair of hexadecimal digits into a number and store it in the array.
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        bsoncxx::stdx::string_view sub = hex.substr(i, 2);
        bytes.push_back(static_cast<std::uint8_t>(std::stoi(bsoncxx::string::to_string(sub), nullptr, 16)));
    }

    return bytes;
}

options::client add_test_server_api(options::client opts) {
    auto api_version = std::getenv("MONGODB_API_VERSION");
    // Check if MONGODB_API_VERSION is set and not empty.
    if (api_version && std::string(api_version).length() > 0) {
        auto version = options::server_api::version_from_string(api_version);
        opts.server_api_opts(options::server_api(version));
    }
    return opts;
}

std::int32_t get_max_wire_version() {
    auto reply = get_is_master();
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

std::string get_server_version() {
    auto output = get_server_status();
    return bsoncxx::string::to_string(output.view()["version"].get_string().value);
}

document::value get_server_params() {
    // Cache reply.
    static auto reply = []() {
        auto client = mongocxx::client{mongocxx::uri{}, test_util::add_test_server_api()};
        return client["admin"].run_command(make_document(kvp("getParameter", "*")));
    }();

    return reply;
}

std::string replica_set_name() {
    auto reply = get_is_master();
    auto name = reply.view()["setName"];
    if (name) {
        return bsoncxx::string::to_string(name.get_string().value);
    }

    return "";
}

static bool is_replica_set(document::view reply) {
    return static_cast<bool>(reply["setName"]);
}

bool is_replica_set() {
    return is_replica_set(get_is_master());
}

static bool is_sharded_cluster(document::view reply) {
    auto const msg = reply["msg"];

    if (!msg) {
        return false;
    }

    return msg.get_string().value == "isdbgrid";
}

bool is_sharded_cluster() {
    return is_sharded_cluster(get_is_master());
}

std::string get_hosts() {
    auto shards = get_shards();
    if (shards)
        return string::to_string(shards->view()["host"].get_string().value);
    return "";
}

std::string get_topology() {
    auto const reply = get_is_master();

    if (is_replica_set(reply)) {
        return "replicaset";
    }

    if (is_sharded_cluster(reply)) {
        return "sharded";
    }

    return "single";
}

bsoncxx::stdx::optional<bsoncxx::document::value> parse_test_file(std::string path) {
    std::stringstream stream;
    std::ifstream test_file{path};

    if (test_file.bad()) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    stream << test_file.rdbuf();
    return bsoncxx::from_json(stream.str());
}

bsoncxx::document::value transform_document(bsoncxx::document::view view, xformer_t const& fcn) {
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

    throw std::logic_error{"could not convert type " + bsoncxx::to_string(value.type()) + " to double"};
}

bool is_numeric(types::bson_value::view value) {
    return value.type() == type::k_int32 || value.type() == type::k_int64 || value.type() == type::k_double;
}

static bsoncxx::stdx::optional<type> is_type_operator(types::bson_value::view value) {
    if (value.type() == type::k_document && value.get_document().value["$$type"]) {
        auto t = value.get_document().value["$$type"].get_string().value;
        if (t == "binData") {
            return {type::k_binary};
        } else if (t == "long") {
            return {type::k_int64};
        }
        throw std::logic_error{"unsupported type for $$type"};
    }
    return bsoncxx::v_noabi::stdx::nullopt;
}

bool matches(types::bson_value::view main, types::bson_value::view pattern, match_visitor visitor_fn) {
    if (auto t = is_type_operator(pattern)) {
        return t == main.type();
    }

    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

    if (is_numeric(pattern) && as_double(pattern) == 42.0) {
        return true;
    }

    // Different numeric types are considered equal.
    if (is_numeric(main) && is_numeric(pattern) && as_double(main) == as_double(pattern)) {
        return true;
    }

    BSONCXX_PRIVATE_WARNINGS_POP();

    if (main.type() == type::k_document) {
        // the value '42' acts as placeholders for "any value"
        if (pattern.type() == type::k_string && pattern.get_string().value == "42") {
            return true;
        }

        document::view main_view = main.get_document().value;
        for (auto&& el : pattern.get_document().value) {
            match_action action = match_action::k_proceed;
            if (visitor_fn) {
                bsoncxx::stdx::optional<types::bson_value::view> main_value;
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
            if (el.key() == "writeErrors") {
                if (main_view.find(el.key()) == main_view.end()) {
                    return false;
                }
                continue;
            }
            // The C++ driver does not include insertedIds as part of the bulk write result.
            if (el.key() == "insertedIds") {
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

        if (size(main_array) < size(pattern_array)) {
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
    return matches(
        types::bson_value::view{types::b_document{doc}},
        types::bson_value::view{types::b_document{pattern}},
        visitor_fn);
}

std::string tolowercase(bsoncxx::stdx::string_view view) {
    std::string out;
    out.reserve(view.size());
    for (size_t i = 0; i < view.length(); i++) {
        out += static_cast<char>(::tolower(view[i]));
    }
    return out;
}

void check_outcome_collection(mongocxx::collection* coll, bsoncxx::document::view expected) {
    REQUIRE(coll);

    read_preference rp;
    rp.mode(read_preference::read_mode::k_primary);

    read_concern rc;
    rc.acknowledge_level(read_concern::level::k_local);

    struct coll_state_guard_type {
        mongocxx::collection& coll;
        read_preference old_rp;
        read_concern old_rc;

        coll_state_guard_type(mongocxx::collection& coll) : coll(coll) {
            old_rp = coll.read_preference();
            old_rc = coll.read_concern();
        }

        ~coll_state_guard_type() {
            try {
                coll.read_preference(old_rp);
                coll.read_concern(old_rc);
            } catch (...) {
            }
        }
    } coll_state_guard(*coll);

    // Ensure this find reads the latest data by using primary read preference with local read
    // concern even when the MongoClient is configured with another read preference or read concern.
    coll->read_preference(rp);
    coll->read_concern(rc);

    using namespace std;

    auto const expected_data = expected["data"].get_array().value;
    auto actual = coll->find({}, options::find().sort(make_document(kvp("_id", 1))));

    REQUIRE(equal(
        begin(expected_data),
        end(expected_data),
        begin(actual),
        [&](bsoncxx::array::element const& ele, document::view const& doc) {
            REQUIRE_BSON_MATCHES(doc, ele.get_document().value);
            return true;
        }));
    REQUIRE(begin(actual) == end(actual));
}

bool server_has_sessions_impl() {
    auto result = get_is_master();
    auto result_view = result.view();

    if (result_view["logicalSessionTimeoutMinutes"]) {
        return true;
    }

    return false;
}

#if defined(MONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION)

cseeos_result client_side_encryption_enabled_or_skip_impl() {
    static cseeos_result const result = [] {
        std::vector<char const*> vars{
            "MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY",
            "MONGOCXX_TEST_AWS_ACCESS_KEY_ID",
            "MONGOCXX_TEST_AZURE_TENANT_ID",
            "MONGOCXX_TEST_AZURE_CLIENT_ID",
            "MONGOCXX_TEST_AZURE_CLIENT_SECRET",
            "MONGOCXX_TEST_CSFLE_TLS_CA_FILE",
            "MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE",
            "MONGOCXX_TEST_GCP_EMAIL",
            "MONGOCXX_TEST_GCP_PRIVATEKEY",
        };

        auto const is_set = [](char const* var) -> bool { return std::getenv(var) != nullptr; };

        auto const count = std::count_if(vars.begin(), vars.end(), is_set);

        if (count == 0) {
            return cseeos_result::skip;
        }

        if (static_cast<std::size_t>(count) < vars.size()) {
            return cseeos_result::fail;
        }

        return cseeos_result::enable;
    }();

    return result;
}

#endif // defined(MONGOC_ENABLE_CLIENT_SIDE_ENCRYPTION)

std::string getenv_or_fail(const std::string env_name) {
    auto val = std::getenv(env_name.c_str());
    if (!val) {
        FAIL("Please set the environment variable: " << env_name);
    }
    return val;
}

} // namespace test_util
} // namespace mongocxx
