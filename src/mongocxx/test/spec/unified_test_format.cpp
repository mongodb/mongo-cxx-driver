// Copyright 2020 MongoDB Inc.
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

#include <fstream>
#include <regex>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/value.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {

using namespace mongocxx;
using namespace bsoncxx;
using namespace spec;

using schema_versions_t =
    std::array<std::array<int, 3 /* major.minor.patch */>, 1 /* supported version */>;
constexpr schema_versions_t schema_versions{{{1, 0, 0}}};

// Spec: Version strings, which are used for schemaVersion and runOnRequirement, MUST conform to
// one of the following formats, where each component is a non-negative integer:
//      <major>.<minor>.<patch>
//      <major>.<minor> (<patch> is assumed to be zero)
//      <major> (<minor> and <patch> are assumed to be zero)
std::vector<int> get_version(const std::string& input) {
    std::vector<int> output;
    const std::regex period("\\.");
    std::transform(std::sregex_token_iterator(std::begin(input), std::end(input), period, -1),
                   std::sregex_token_iterator(),
                   std::back_inserter(output),
                   [](const std::string& s) { return std::stoi(s); });

    while (output.size() < schema_versions[0].size())
        output.push_back(0);

    return output;
}

std::vector<int> get_version(bsoncxx::document::element doc) {
    return get_version(doc.get_string().value.to_string());
}

template <typename Range1, typename Range2, typename Compare>
bool is_compatible_version(Range1 range1, Range2 range2, Compare comp) {
    // only compare major and minor in version of the form "<int>.<int>.<int>", i.e., [0:2)
    return std::lexicographical_compare(std::begin(range1),
                                        std::begin(range1) + 2,
                                        std::begin(range2),
                                        std::begin(range2) + 2,
                                        comp);
}

bool equals_server_topology(const document::element& topologies) {
    using bsoncxx::types::bson_value::value;

    // The server's topology will not change during the test. No need to make a round-trip for every
    // test file.
    static std::string server_topology = test_util::get_topology();
    auto equals = [&](const bsoncxx::array::element& topology) {
        return topology == value(server_topology) ||
               (topology == value("sharded-replicaset") && server_topology == "shared");
    };

    auto t = topologies.get_array().value;
    return std::end(t) != std::find_if(std::begin(t), std::end(t), equals);
}

bool compatible_with_server(const bsoncxx::array::element& requirement) {
    // The server's version will not change during the test. No need to make a round-trip for every
    // test file.
    static std::vector<int> expected = get_version(test_util::get_server_version());

    if (auto min_server_version = requirement["minServerVersion"]) {
        auto actual = get_version(min_server_version);
        if (!is_compatible_version(actual, expected, std::less_equal<int>{}))
            return false;
    }

    if (auto max_server_version = requirement["maxServerVersion"]) {
        auto actual = get_version(max_server_version);
        if (!is_compatible_version(actual, expected, std::greater_equal<int>{}))
            return false;
    }

    if (auto topologies = requirement["topologies"])
        return equals_server_topology(topologies);
    return true;
}

bool has_run_on_requirements(const bsoncxx::document::view test) {
    if (!test["runOnRequirements"])
        return true;

    auto requirements = test["runOnRequirements"].get_array().value;
    return std::any_of(std::begin(requirements), std::end(requirements), compatible_with_server);
}

document::value parse_test_file(const std::string& test_path) {
    bsoncxx::stdx::optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);
    return test_spec.value();
}

bool is_compatible_scehema_version(document::view test_spec) {
    REQUIRE(test_spec["schemaVersion"]);
    auto test_schema_version = get_version(test_spec["schemaVersion"]);
    auto compat = [&](std::array<int, 3> v) {
        return is_compatible_version(test_schema_version, v, std::less_equal<int>{});
    };
    return std::any_of(std::begin(schema_versions), std::end(schema_versions), compat);
}

std::vector<std::string> versions_to_string(schema_versions_t versions) {
    std::vector<std::string> out;
    for (const auto& v : versions) {
        std::stringstream v_str;
        v_str << std::to_string(v[0]) << '.'  // major.
              << std::to_string(v[1]) << '.'  // minor.
              << std::to_string(v[2]);        // patch
        out.push_back(v_str.str());
    }
    return out;
}

void run_tests_in_file(const std::string& test_path) {
    auto test_spec = parse_test_file(test_path);
    auto test_spec_view = test_spec.view();

    CAPTURE(test_path, to_json(test_spec_view));
    if (!is_compatible_scehema_version(test_spec_view)) {
        std::stringstream error;
        error << "incompatible schema version" << std::endl
              << "Expected: " << test_spec_view["schemaVersion"].get_string().value << std::endl
              << "Supported versions:" << std::endl;

        auto v = versions_to_string(schema_versions);
        std::copy(std::begin(v), std::end(v), std::ostream_iterator<std::string>(error, "\n"));

        FAIL(error.str());
        return;
    }

    if (!has_run_on_requirements(test_spec_view)) {
        std::stringstream warning;
        warning << "file skipped: " << test_path << std::endl
                << "none of the runOnRequirements were met" << std::endl
                << to_json(test_spec_view["runOnRequirements"].get_array().value);
        WARN(warning.str());
        return;
    }

    const std::string description = test_spec_view["description"].get_string().value.to_string();
    SECTION(description) {
        // TODO: createEntities
        // TODO: initialData
        // TODO: tests
    }
}

TEST_CASE("unified format spec automated tests", "[unified_format_spec]") {
    instance::current();

    std::string path = std::getenv("UNIFIED_FORMAT_TESTS_PATH");
    CAPTURE(path);
    REQUIRE(path.size());

    std::ifstream files{path + "/test_files.txt"};
    REQUIRE(files.good());

    for (std::string file; std::getline(files, file);) {
        CAPTURE(file);
        run_tests_in_file(path + '/' + file);
    }
}
}  // namespace
