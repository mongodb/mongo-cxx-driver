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

#include "entity.hh"
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

template <typename Range1, typename Range2>
bool is_compatible_version(Range1 range1, Range2 range2) {
    // only compare major and minor in version of the form "<int>.<int>.<int>", i.e., [0:2)
    return std::lexicographical_compare(std::begin(range1),
                                        std::begin(range1) + 2,
                                        std::begin(range2),
                                        std::begin(range2) + 2,
                                        std::less_equal<int>{});
}

bool equals_server_topology(const document::element& topologies) {
    using bsoncxx::types::bson_value::value;

    // The server's topology will not change during the test. No need to make a round-trip for every
    // test file.
    static std::string actual = test_util::get_topology();
    auto equals = [&](const bsoncxx::array::element& expected) {
        return expected == value(actual) ||
               (expected == value("sharded") && actual == "sharded-replicaset");
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
        if (!is_compatible_version(actual, expected))
            return false;
    }

    if (auto max_server_version = requirement["maxServerVersion"]) {
        auto actual = get_version(max_server_version);
        if (!is_compatible_version(expected, actual))
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

std::string json_kvp_to_uri_kvp(std::string s) {
    // The transformation is as follows:
    //     1. "{ "key" : "value" }"     -- initial representation
    //     2. "key:value"               -- intermediate step (without quotes)
    //     3. "key=value"               -- final step (without quotes)

    using namespace std;
    auto should_remove = [&](const char c) {
        const auto remove = {' ', '"', '{', '}'};
        return end(remove) != find(begin(remove), end(remove), c);
    };

    s.erase(remove_if(begin(s), end(s), should_remove), end(s));
    replace(begin(s), end(s), ':', '=');
    return s;
}

std::string json_to_uri_opts(const std::string& input) {
    // Transforms a non-nested JSON document string (assumed to contain URI keys and values) to a
    // string of equivalent URI options and values. That is,
    //      input   := "{ "readConcernLevel" : "local", "w" : 1 }"
    //      output  := "readConcernLevel=local&w=1"
    std::vector<std::string> output;
    const std::regex delim(",");
    std::transform(std::sregex_token_iterator(std::begin(input), std::end(input), delim, -1),
                   std::sregex_token_iterator(),
                   std::back_inserter(output),
                   json_kvp_to_uri_kvp);

    auto join = [](const std::string& s1, const std::string& s2) { return s1 + "&" + s2; };
    return std::accumulate(std::begin(output) + 1, std::end(output), output[0], join);
}

std::string uri_options_to_string(document::view object) {
    // Spec: Optional object. Additional URI options to apply to the test suite's connection string
    // that is used to create this client. Any keys in this object MUST override conflicting keys in
    // the connection string.
    if (!object["uriOptions"])
        return {};

    // TODO: Spec: if 'readPreferenceTags' is specified in this object, the key will map to an array
    //  of strings, each representing a tag set, since it is not feasible to define multiple
    //  'readPreferenceTags' keys in the object.
    REQUIRE_FALSE(object["readPreferenceTags"]);

    auto json = to_json(object["uriOptions"].get_document());
    auto opts = json_to_uri_opts(json);

    CAPTURE(json, opts);
    return opts;
}

std::string get_hostnames(document::view object) {
    const auto default_uri = std::string{"localhost:27017"};
    // Spec: This [useMultipleMongoses] option has no effect for non-sharded topologies.
    if (test_util::is_replica_set())
        return default_uri;

    // Spec: If true and the topology is a sharded cluster, the test runner MUST assert that this
    // MongoClient connects to multiple mongos hosts (e.g. by inspecting the connection string).
    if (!object["useMultipleMongoses"] || !object["useMultipleMongoses"].get_bool())
        return default_uri;

    // from: https://docs.mongodb.com/manual/reference/config-database/#config.shards
    // If the shard is a replica set, the host field displays the name of the replica set, then a
    // slash, then a comma-separated list of the hostnames of each member of the replica set, as in
    // the following example:
    //      { ... , "host" : "shard0001/localhost:27018,localhost:27019,localhost:27020", ... }
    auto host = test_util::get_hosts();
    auto after_slash = ++std::find(std::begin(host), std::end(host), '/');
    REQUIRE(after_slash < std::end(host));

    auto hostnames = std::string{after_slash, std::end(host)};
    CAPTURE(host, hostnames);

    // require multiple mongos hosts
    REQUIRE(std::end(hostnames) != std::find(std::begin(hostnames), std::end(hostnames), ','));
    return hostnames;
}

apm_checker& get_apm_checker() {
    static auto apm = apm_checker{};
    return apm;
}

void add_observe_events(options::apm& apm_opts, document::view object) {
    using types::bson_value::value;
    if (!object["observeEvents"])
        return;

    auto events = object["observeEvents"].get_array().value;
    auto& apm = get_apm_checker();
    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandStartedEvent")))
        apm.set_command_started(apm_opts);

    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandSucceededEvent")))
        apm.set_command_succeeded(apm_opts);

    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandFailedEvent")))
        apm.set_command_failed(apm_opts);
}

void add_ignore_command_monitoring_events(document::view object) {
    if (!object["ignoreCommandMonitoringEvents"])
        return;
    for (auto cme : object["ignoreCommandMonitoringEvents"].get_array().value) {
        auto event = apm_checker::to_event(cme.get_string());

        CAPTURE(apm_checker::to_string(event), cme.get_string());
        auto& apm = get_apm_checker();
        apm.set_ignore_command_monitoring_event(event);
    }
}

entity::map& get_entity_map() {
    static auto m = entity::map{};
    return m;
}

write_concern get_write_concern(const document::element& opts) {
    if (!opts["writeConcern"])
        return {};

    auto wc = write_concern{};
    if (auto w = opts["writeConcern"]["w"]) {
        REQUIRE(w.type() == type::k_int32);  // TODO: support type k_utf8
        wc.nodes(w.get_int32());
    }

    return wc;
}

read_concern get_read_concern(const document::element& opts) {
    if (!opts["readConcern"])
        return {};

    auto rc = read_concern{};
    if (auto level = opts["readConcern"]["level"])
        rc.acknowledge_string(level.get_string().value);
    return rc;
}

template <typename T>
void set_common_options(T& t, const document::element& opts) {
    if (!opts)
        return;

    t.read_concern(get_read_concern(opts));
    t.write_concern(get_write_concern(opts));
    REQUIRE_FALSE(/* TODO */ opts["readPreference"]);
}

options::gridfs::bucket get_bucket_options(document::view object) {
    if (!object["bucketOptions"])
        return {};

    auto opts = options::gridfs::bucket{};
    set_common_options(opts, object["bucketOptions"]);

    if (auto name = object["bucketOptions"]["bucketName"])
        opts.bucket_name(name.get_string().value.to_string());
    if (auto size = object["bucketOptions"]["chunkSizeBytes"])
        opts.chunk_size_bytes(size.get_int32().value);
    REQUIRE_FALSE(object["bucketOptions"]["disableMD5"]);

    return opts;
}

options::client_session get_session_options(document::view object) {
    if (!object["sessionOptions"])
        return {};

    auto session_opts = options::client_session{};
    auto txn_opts = options::transaction{};

    set_common_options(txn_opts, object["sessionOptions"]["defaultTransactionOptions"]);
    REQUIRE_FALSE(/* TODO */ object["sessionOptions"]["causalConsistency"]);

    session_opts.default_transaction_opts(txn_opts);
    return session_opts;
}

// TODO: create_change_stream

gridfs::bucket create_bucket(document::view object) {
    auto id = object["database"].get_string().value.to_string();
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto opts = get_bucket_options(object);
    auto bucket = db.gridfs_bucket(opts);

    CAPTURE(id);
    return bucket;
}

client_session create_session(document::view object) {
    auto id = object["client"].get_string().value.to_string();
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto opts = get_session_options(object);
    auto session = client.start_session(opts);

    CAPTURE(id);
    return session;
}

collection create_collection(document::view object) {
    auto id = object["database"].get_string().value.to_string();
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto name = object["collectionName"].get_string().value.to_string();
    auto coll = collection{db.collection(name)};

    set_common_options(coll, object["collectionOptions"]);

    CAPTURE(name, id);
    return coll;
}

database create_database(document::view object) {
    auto id = object["client"].get_string().value.to_string();
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto name = object["databaseName"].get_string().value.to_string();
    auto db = database{client.database(name)};

    set_common_options(db, object["databaseOptions"]);

    CAPTURE(name, id);
    return db;
}

client create_client(document::view object) {
    auto conn = "mongodb://" + get_hostnames(object) + "/?" + uri_options_to_string(object);
    auto opts = options::apm{};

    add_observe_events(opts, object);
    add_ignore_command_monitoring_events(object);

    CAPTURE(conn);
    return client{uri{conn}, options::client{}.apm_opts(opts)};
}

bool add_to_map(const array::element& obj) {
    // Spec: This object MUST contain exactly one top-level key that identifies the entity type and
    // maps to a nested object, which specifies a unique name for the entity ('id' key) and any
    // other parameters necessary for its construction.
    auto doc = obj.get_document().view().begin();
    auto type = doc->key().to_string();
    auto params = doc->get_document().view();
    auto id = params["id"].get_string().value.to_string();
    auto& map = get_entity_map();

    // clang-format off
    if (type == "client")       return map.insert(id, create_client(params));
    if (type == "database")     return map.insert(id, create_database(params));
    if (type == "collection")   return map.insert(id, create_collection(params));
    if (type == "bucket")       return map.insert(id, create_bucket(params));
    if (type == "session")      return map.insert(id, create_session(params));
    // clang-format on

    CAPTURE(type, id, to_json(params));
    FAIL("unrecognized type { " + type + " }");
    return false;
}

void create_entities(const document::view test) {
    if (!test["createEntities"])
        return;

    get_entity_map().clear();
    auto entities = test["createEntities"].get_array().value;
    REQUIRE(std::all_of(std::begin(entities), std::end(entities), add_to_map));
}

document::value parse_test_file(const std::string& test_path) {
    bsoncxx::stdx::optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);
    return test_spec.value();
}

bool is_compatible_schema_version(document::view test_spec) {
    REQUIRE(test_spec["schemaVersion"]);
    auto test_schema_version = get_version(test_spec["schemaVersion"]);
    auto compat = [&](std::array<int, 3> v) {
        // Test files are considered compatible with a test runner if their schemaVersion is less
        // than or equal to a supported version in the test runner, given the same major version
        // component.
        return test_schema_version[0] == v[0] && is_compatible_version(test_schema_version, v);
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
    if (!is_compatible_schema_version(test_spec_view)) {
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
        create_entities(test_spec_view);
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
