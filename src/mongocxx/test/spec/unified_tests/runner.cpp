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
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>

#include "assert.hh"
#include "entity.hh"
#include "operations.hh"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/value.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {

using namespace mongocxx;
using namespace bsoncxx;
using namespace spec;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using schema_versions_t =
    std::array<std::array<int, 3 /* major.minor.patch */>, 2 /* supported version */>;
constexpr schema_versions_t schema_versions{{{{1, 1, 0}}, {{1, 7, 0}}}};

std::pair<std::unordered_map<std::string, spec::apm_checker>&, entity::map&> init_maps() {
    // Below initializes the static apm map and entity map if needed, in that order. This will also
    // ensure they are destroyed in reverse order, which prevents a "heap-use-after-free" error.
    //
    // The "heap-use-after-free" error will happen if:
    //      1. The apm checker's destructor is called
    // `    2. The client's destructor is called
    //          2a. The client calls _mongoc_client_end_sessions in its destruction process.
    //          2b. The client attempts to log this event in the apm checker, which has been freed.
    //
    // Reversing the order of destruction fixes the issue.
    static std::unordered_map<std::string, spec::apm_checker> apm_map;
    static entity::map entity_map;
    return {apm_map, entity_map};
}

std::unordered_map<std::string, spec::apm_checker>& get_apm_map() {
    return init_maps().first;
}

entity::map& get_entity_map() {
    return init_maps().second;
}

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
    return get_version(string::to_string(doc.get_string().value));
}

// Toggle ignoring patch number when comparing version strings.
enum struct ignore_patch { no, yes };

template <typename Range1, typename Range2>
bool is_compatible_version(Range1 range1, Range2 range2, ignore_patch ip) {
    // Incompatible major.
    if (range1[0] > range2[0]) {
        return false;
    }

    // Compatible major, minor and patch ignored.
    if (range1[0] < range2[0]) {
        return true;
    }

    // Compatible major, incompatible minor.
    if (range1[1] > range2[1]) {
        return false;
    }

    // Compatible major, compatible minor, patch ignored.
    if (ip == ignore_patch::yes) {
        return true;
    }

    // Compatible major, compatible minor, and compatible patch.
    return range1[2] <= range2[2];
}

template <typename Range1, typename Range2>
bool is_compatible_version(Range1 range1, Range2 range2) {
    return is_compatible_version(range1, range2, ignore_patch::no);
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

    if (auto server_params = requirement["serverParameters"]) {
        document::value actual = make_document();
        try {
            actual = test_util::get_server_params();
        } catch (const operation_exception& e) {
            // Mongohouse does not support getParameter, so if we get an error from
            // getParameter, exit this logic early and skip the test.
            std::string message = e.what();
            if (message.find("command getParameter is unsupported") != std::string::npos) {
                return false;
            }

            throw e;
        }

        for (auto kvp : server_params.get_document().view()) {
            auto param = kvp.key();
            auto value = kvp.get_value();
            // If actual parameter is unset or unequal to requirement, skip test.
            if (!actual[param] || actual[param].get_bool() != value.get_bool()) {
                return false;
            }
        }
    }
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
    if (!test_util::is_sharded_cluster()) {
        return default_uri;
    }

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

void add_observe_events(options::apm& apm_opts, document::view object) {
    using types::bson_value::value;
    if (!object["observeEvents"])
        return;

    auto name = string::to_string(object["id"].get_string().value);
    auto& apm = get_apm_map()[name];

    auto observe_sensitive = object["observeSensitiveCommands"];
    apm.observe_sensitive_events = observe_sensitive && observe_sensitive.get_bool();

    auto events = object["observeEvents"].get_array().value;
    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandStartedEvent")))
        apm.set_command_started_unified(apm_opts);

    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandSucceededEvent")))
        apm.set_command_succeeded_unified(apm_opts);

    if (std::end(events) !=
        std::find(std::begin(events), std::end(events), value("commandFailedEvent")))
        apm.set_command_failed_unified(apm_opts);
}

void add_ignore_command_monitoring_events(document::view object) {
    if (!object["ignoreCommandMonitoringEvents"])
        return;
    for (auto cme : object["ignoreCommandMonitoringEvents"].get_array().value) {
        CAPTURE(cme.get_string());
        auto name = string::to_string(object["id"].get_string().value);
        auto& apm = get_apm_map()[name];
        apm.set_ignore_command_monitoring_event(string::to_string(cme.get_string().value));
    }
}

options::server_api create_server_api(document::view object) {
    document::element sav;
    if (!(sav = object["serverApi"]["version"])) {
        FAIL("must specify a version when using serverApi");
    }

    REQUIRE(sav.type() == type::k_string);
    auto version = options::server_api::version_from_string(sav.get_string().value);
    auto server_api_opts = options::server_api(version);

    if (auto de = object["serverApi"]["deprecationErrors"]) {
        REQUIRE(de.type() == type::k_bool);
        server_api_opts.deprecation_errors(de.get_bool());
    }
    if (auto strict = object["serverApi"]["strict"]) {
        REQUIRE(strict.type() == type::k_bool);
        server_api_opts.strict(strict.get_bool());
    }

    return server_api_opts;
}

write_concern get_write_concern(const document::element& opts) {
    if (!opts["writeConcern"])
        return {};

    auto wc = write_concern{};
    if (auto w = opts["writeConcern"]["w"]) {
        if (w.type() == type::k_utf8) {
            auto strval = w.get_string().value;
            if (0 == strval.compare("majority")) {
                wc.acknowledge_level(mongocxx::write_concern::level::k_majority);
            } else {
                FAIL("Unsupported write concern string " << strval);
            }
            return wc;
        } else if (w.type() == type::k_int32) {
            wc.nodes(w.get_int32());
        } else {
            FAIL("Unsupported write concern value");
        }

        wc.nodes(w.get_int32());
    }

    return wc;
}

read_concern get_read_concern(const document::element& opts) {
    if (!opts["readConcern"])
        return {};

    auto rc = read_concern{};

    if (auto level = opts["readConcern"]["level"]) {
        rc.acknowledge_string(level.get_string().value);
    }

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
        opts.bucket_name(string::to_string(name.get_string().value));
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

    if (object["sessionOptions"]["snapshot"])
        session_opts.snapshot(true);

    return session_opts;
}

gridfs::bucket create_bucket(document::view object) {
    auto id = string::to_string(object["database"].get_string().value);
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto opts = get_bucket_options(object);
    auto bucket = db.gridfs_bucket(opts);

    CAPTURE(id);
    return bucket;
}

client_session create_session(document::view object) {
    auto id = string::to_string(object["client"].get_string().value);
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto opts = get_session_options(object);
    auto session = client.start_session(opts);

    CAPTURE(id);
    return session;
}

collection create_collection(document::view object) {
    auto id = string::to_string(object["database"].get_string().value);
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto name = string::to_string(object["collectionName"].get_string().value);
    auto coll = collection{db.collection(name)};

    set_common_options(coll, object["collectionOptions"]);

    CAPTURE(name, id);
    return coll;
}

database create_database(document::view object) {
    auto id = string::to_string(object["client"].get_string().value);
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto name = string::to_string(object["databaseName"].get_string().value);
    auto db = database{client.database(name)};

    set_common_options(db, object["databaseOptions"]);

    CAPTURE(name, id);
    return db;
}

client create_client(document::view object) {
    auto conn = "mongodb://" + get_hostnames(object) + "/?" + uri_options_to_string(object);
    auto apm_opts = options::apm{};
    auto client_opts = options::client{};
    // Use specified serverApi or default if none is provided.
    if (object["serverApi"]) {
        auto server_api_opts = create_server_api(object);
        client_opts.server_api_opts(server_api_opts);
    } else {
        client_opts = test_util::add_test_server_api();
    }

    add_observe_events(apm_opts, object);
    add_ignore_command_monitoring_events(object);

    CAPTURE(conn);
    return client{uri{conn}, client_opts.apm_opts(apm_opts)};
}

bool add_to_map(const array::element& obj) {
    // Spec: This object MUST contain exactly one top-level key that identifies the entity type and
    // maps to a nested object, which specifies a unique name for the entity ('id' key) and any
    // other parameters necessary for its construction.
    auto doc = obj.get_document().view().begin();
    auto type = string::to_string(doc->key());
    auto params = doc->get_document().view();
    auto id = string::to_string(params["id"].get_string().value);
    auto& map = get_entity_map();

    // clang-format off
    if (type == "client")       return map.insert(id, create_client(params));
    if (type == "database")     return map.insert(id, create_database(params));
    if (type == "collection")   return map.insert(id, create_collection(params));
    if (type == "bucket")       return map.insert(id, create_bucket(params));
    if (type == "session")      return map.insert(id, create_session(params));
    // clang-format on

    CAPTURE(type, id, params);
    FAIL("unrecognized type { " + type + " }");
    return false;
}

void create_entities(const document::view test) {
    if (!test["createEntities"])
        return;

    get_entity_map().clear();
    get_apm_map().clear();
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
        return test_schema_version[0] == v[0] &&
               is_compatible_version(test_schema_version, v, ignore_patch::yes);
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

std::vector<document::view> array_elements_to_documents(array::view array) {
    // no implicit conversion from 'bsoncxx::array::view' to 'bsoncxx::document::view'
    auto docs = std::vector<document::view>{};
    auto arr_to_doc = [](const array::element& doc) { return doc.get_document().value; };

    std::transform(std::begin(array), std::end(array), std::back_inserter(docs), arr_to_doc);
    return docs;
}

void add_data_to_collection(const array::element& data) {
    auto db_name = data["databaseName"].get_string().value;
    auto& map = get_entity_map();
    auto& db = map.get_database_by_name(db_name);
    auto insert_opts = mongocxx::options::insert();

    auto wc = write_concern{};
    wc.acknowledge_level(write_concern::level::k_majority);
    wc.majority(std::chrono::milliseconds{0});

    auto coll_name = data["collectionName"].get_string().value;

    if (db.has_collection(coll_name))
        db[coll_name].drop();

    auto coll = db.create_collection(coll_name, {}, wc);
    insert_opts.write_concern(wc);

    auto to_insert = array_elements_to_documents(data["documents"].get_array().value);
    REQUIRE((to_insert.empty() ||
             coll.insert_many(to_insert, insert_opts)->result().inserted_count() != 0));
}

void load_initial_data(document::view test) {
    if (!test["initialData"])
        return;

    auto data = test["initialData"].get_array().value;
    for (auto&& d : data)
        add_data_to_collection(d);
}

void assert_result(const array::element& ops,
                   document::view actual_result,
                   bool is_array_of_root_docs) {
    if (!ops["expectResult"])
        return;

    auto expected_result = ops["expectResult"];
    assert::matches(actual_result["result"].get_value(),
                    expected_result.get_value(),
                    get_entity_map(),
                    true,
                    is_array_of_root_docs);

    if (ops["saveResultAsEntity"]) {
        auto key = string::to_string(ops["saveResultAsEntity"].get_string().value);
        get_entity_map().insert(key, actual_result);
    }
}

void assert_error(const mongocxx::operation_exception& exception,
                  const array::element& expected,
                  document::view actual) {
    std::string server_error_msg =
        exception.raw_server_error() ? to_json(*exception.raw_server_error()) : "no server error";

    CAPTURE(exception.what(), server_error_msg);

    auto expect_error = expected["expectError"];
    REQUIRE(expect_error);

    if (expect_error["isError"])
        return;

    auto actual_result = actual["result"];
    if (auto expected_result = expect_error["expectResult"]) {
        assert::matches(actual_result.get_value(), expected_result.get_value(), get_entity_map());
    }

    if (auto is_client_error = expect_error["isClientError"]) {
        if (std::strstr(exception.what(), "Snapshot reads require MongoDB 5.0 or later") !=
            nullptr) {
            // Original error: { MONGOC_ERROR_CLIENT, MONGOC_ERROR_CLIENT_SESSION_FAILURE }
            // Do not assert a server-side error.
            // The C++ driver throws this error as a server-side error operation_exception.
            // Remove this special case as part of CXX-2377.
            REQUIRE(is_client_error.get_bool());
        } else if (std::strstr(exception.what(), "The selected server does not support hint for") !=
                   nullptr) {
            // Original error: { MONGOC_ERROR_COMMAND, MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION }
            // Do not assert a server-side error.
            // The C++ driver throws this error as a server-side error operation_exception.
            // Remove this special case as part of CXX-2377.
            REQUIRE(is_client_error.get_bool());
        } else if (is_client_error.get_bool()) {
            // An operation_exception represents a server-side error.
            REQUIRE(!is_client_error.get_bool());
        }
    }

    if (auto contains = expect_error["errorLabelsContain"]) {
        auto labels = contains.get_array().value;
        auto has_error_label = [&](const array::element& ele) {
            return exception.has_error_label(ele.get_string().value);
        };
        REQUIRE(std::all_of(std::begin(labels), std::end(labels), has_error_label));
    }

    if (auto omit = expect_error["errorLabelsOmit"]) {
        auto labels = omit.get_array().value;
        auto has_error_label = [&](const array::element& ele) {
            return exception.has_error_label(ele.get_string().value);
        };
        REQUIRE(std::none_of(std::begin(labels), std::end(labels), has_error_label));
    }

    if (auto expected_code = expect_error["errorCode"]) {
        auto actual_code = exception.code().value();
        REQUIRE(actual_code == expected_code.get_int32());
    }

    if (auto code_name = expect_error["errorCodeName"]) {
        auto expected_name = string::to_string(code_name.get_string().value);
        uint32_t expected_code = error_code_from_name(expected_name);
        REQUIRE(exception.code().value() == static_cast<int>(expected_code));
    }

    /*
    // This has no data to act on until CXX-834 as been implemented; see notes
    if (auto expected_error = expect_error["errorContains"]) {
        // in assert_error():
        // See
        //
    "https://github.com/mongodb/specifications/blob/master/source/unified-test-format/unified-test-format.rst#expectederror":
        // A substring of the expected error message (e.g. "errmsg" field in a server error
        // document). The test runner MUST assert that the error message contains this string using
        // a case-insensitive match.
        std::string expected_error_str(expected_error.get_string().value);
        std::string actual_str(reinterpret_cast<const std::string::value_type*>(actual.data()),
                               actual.length());

        transform(begin(expected_error_str),
                  end(expected_error_str),
                  begin(expected_error_str),
                  &toupper);

        REQUIRE(actual_str.substr(expected_error_str.size()) == expected_error_str);
    }
    */
}

void assert_error(mongocxx::exception& e, const array::element& ops) {
    CAPTURE(e.what());
    auto expect_error = ops["expectError"];
    REQUIRE(expect_error);

    if (expect_error["isError"])
        return;

    if (auto is_client_error = expect_error["isClientError"]) {
        REQUIRE(is_client_error.get_bool());
    }

    // below is only used for server-side errors.
    REQUIRE_FALSE(expect_error["expectResult"]);

    // TODO CXX-834: client-side errors may contain error labels. However, only
    //  mongocxx::operation_exception keeps track of the raw_sever_error (and consequently the
    //  error label) and, as a result, is the only exception type with the required
    //  `has_error_label` method. Until we fix CXX-834, there's no way to check the error label of a
    //  mongocxx::exception.
    REQUIRE_FALSE(expect_error["errorLabelsContain"]);
    REQUIRE_FALSE(expect_error["errorLabelsOmit"]);

    REQUIRE_FALSE(/* TODO */ expect_error["errorContains"]);
    REQUIRE_FALSE(/* TODO */ expect_error["errorCode"]);
    REQUIRE_FALSE(/* TODO */ expect_error["errorCodeName"]);
}

void assert_events(const array::element& test) {
    if (!test["expectEvents"])
        return;

    for (auto e : test["expectEvents"].get_array().value) {
        const auto ignore_extra_events = [&]() -> bool {
            const auto elem = e["ignoreExtraEvents"];
            return elem && elem.get_bool().value;
        }();
        const auto events = e["events"].get_array().value;
        const auto name = string::to_string(e["client"].get_string().value);
        get_apm_map()[name].compare_unified(events, get_entity_map(), ignore_extra_events);
    }
}

void assert_outcome(const array::element& test) {
    using std::begin;
    using std::end;
    using std::equal;

    if (!test["outcome"])
        return;

    for (auto outcome : test["outcome"].get_array().value) {
        CAPTURE(to_json(outcome.get_document()));

        auto db_name = outcome["databaseName"].get_string().value;
        auto coll_name = outcome["collectionName"].get_string().value;
        auto docs = outcome["documents"].get_array().value;

        auto db = get_entity_map().get_database_by_name(db_name);
        auto coll = db.collection(coll_name);

        auto results = coll.find({}, options::find{}.sort(make_document(kvp("_id", 1))));

        auto actual = results.begin();
        for (auto& expected : docs) {
            assert::matches(
                types::bson_value::value(*actual), expected.get_value(), get_entity_map());
            ++actual;
        }

        REQUIRE(begin(results) == end(results)); /* cursor is exhausted */
    }
}

struct disable_fail_point {
    std::vector<std::pair<std::string, std::string>> fail_points;

    void add_fail_point(const std::string& uri, const std::string& command) {
        fail_points.emplace_back(uri, command);
    }

    void operator()() const {
        for (auto&& f : fail_points) {
            spec::disable_fail_point(f.first, {}, f.second);
        }
    }
};

document::value bulk_write_result(const mongocxx::bulk_write_exception& e) {
    auto reply = e.raw_server_error().value();

    auto get_or_default = [&](bsoncxx::stdx::string_view key) {
        return reply[key] ? reply[key].get_int32().value : 0;
    };

    auto result = bsoncxx::builder::basic::document{};
    result.append(kvp("result",
                      make_document(kvp("matchedCount", get_or_default("nMatched")),
                                    kvp("modifiedCount", get_or_default("nModified")),
                                    kvp("upsertedCount", get_or_default("nUpserted")),
                                    kvp("deletedCount", get_or_default("nRemoved")),
                                    kvp("insertedCount", get_or_default("nInserted")),
                                    kvp("upsertedIds", make_document()))));

    return result.extract();
}

void run_tests(document::view test) {
    REQUIRE(test["tests"]);

    for (auto ele : test["tests"].get_array().value) {
        auto description = string::to_string(ele["description"].get_string().value);
        SECTION(description) {
            if (!has_run_on_requirements(ele.get_document())) {
                std::stringstream warning;
                warning << "test skipped: "
                        << "none of the runOnRequirements were met" << std::endl
                        << to_json(ele["runOnRequirements"].get_array().value);
                WARN(warning.str());
                continue;
            }

            if (ele["skipReason"]) {
                WARN("Skip Reason: " + string::to_string(ele["skipReason"].get_string().value));
                continue;
            }

            disable_fail_point disable_fail_point_fn{};

            for (auto&& apm : get_apm_map()) {
                apm.second.clear_events();
            }

            operations::state state;

            for (auto ops : ele["operations"].get_array().value) {
                const auto ignore_result_and_error = [&]() -> bool {
                    const auto elem = ops["ignoreResultAndError"];
                    return elem && elem.get_bool().value;
                }();

                try {
                    auto result = bsoncxx::builder::basic::make_document();
                    result = operations::run(get_entity_map(), get_apm_map(), ops, state);

                    if (string::to_string(ops["object"].get_string().value) == "testRunner") {
                        if (string::to_string(ops["name"].get_string().value) == "failPoint") {
                            disable_fail_point_fn.add_fail_point(
                                string::to_string(result["uri"].get_string().value),
                                string::to_string(result["failPoint"].get_string().value));
                        }

                        // Special test operations return no result and are always expected to
                        // succeed. These operations SHOULD NOT be combined with expectError,
                        // expectResult, or saveResultAsEntity.
                        continue;
                    }

                    // Some operations fully iterate a cursor and return the result as an array. The
                    // elements of such an array should all be treated as root-level documents.
                    auto is_array_of_root_docs = false;
                    {
                        static const std::unordered_set<std::string> names = {
                            "aggregate",
                            "find",
                            "iterateUntilDocumentOrError",
                            "listCollections",
                            "listDatabases",
                            "listIndexes",
                        };

                        const auto name = string::to_string(ops["name"].get_string().value);

                        is_array_of_root_docs = names.find(name) != names.end();
                    }

                    if (!ignore_result_and_error) {
                        assert_result(ops, result, is_array_of_root_docs);
                    }
                } catch (mongocxx::bulk_write_exception& e) {
                    if (!ignore_result_and_error) {
                        auto result = bulk_write_result(e);
                        assert_error(e, ops, result);
                    }
                } catch (mongocxx::operation_exception& e) {
                    if (!ignore_result_and_error) {
                        assert_error(e, ops, make_document());
                    }
                } catch (mongocxx::exception& e) {
                    if (!ignore_result_and_error) {
                        assert_error(e, ops);
                    }
                }
            }
            disable_fail_point_fn();

            assert_events(ele);
            assert_outcome(ele);
        }
    }
}

void run_tests_in_file(const std::string& test_path) {
    auto test_spec = parse_test_file(test_path);
    auto test_spec_view = test_spec.view();

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

    const std::string description =
        string::to_string(test_spec_view["description"].get_string().value);
    CAPTURE(description);
    create_entities(test_spec_view);
    load_initial_data(test_spec_view);
    run_tests(test_spec_view);
}

// Check the environment for the specified variable; if present, extract it
// as a directory and run all the tests contained in the magic "test_files.txt"
// file:
bool run_unified_format_tests_in_env_dir(
    const std::string& env_path,
    const std::set<mongocxx::stdx::string_view>& unsupported_tests = {}) {
    const char* p = std::getenv(env_path.c_str());

    if (nullptr == p)
        FAIL("unable to look up path from environment variable \"" << env_path << "\"");

    const std::string base_path{p};

    auto test_file_set_path = base_path + "/test_files.txt";
    std::ifstream files{test_file_set_path};

    if (!files.good()) {
        FAIL("unable to find/open test_files.txt in path \"" << test_file_set_path << '\"');
    }

    instance::current();

    for (std::string file; std::getline(files, file);) {
        SECTION(file) {
            if (unsupported_tests.find(file) != unsupported_tests.end()) {
                WARN("Skipping unsupported test file: " << file);
            } else {
                run_tests_in_file(base_path + '/' + file);
            }
        }
    }

    return true;
}

TEST_CASE("unified format spec automated tests", "[unified_format_spec]") {
    const std::set<mongocxx::stdx::string_view> unsupported_tests = {
        // Waiting on CDRIVER-3525 and CXX-2166.
        "valid-pass/entity-client-cmap-events.json",
        // Waiting on CDRIVER-3525 and CXX-2166.
        "valid-pass/assertNumberConnectionsCheckedOut.json"};

    CHECK(run_unified_format_tests_in_env_dir("UNIFIED_FORMAT_TESTS_PATH", unsupported_tests));
}

TEST_CASE("session unified format spec automated tests", "[unified_format_spec]") {
    CHECK(run_unified_format_tests_in_env_dir("SESSION_UNIFIED_TESTS_PATH"));
}

TEST_CASE("CRUD unified format spec automated tests", "[unified_format_spec]") {
    CHECK(run_unified_format_tests_in_env_dir("CRUD_UNIFIED_TESTS_PATH"));
}

TEST_CASE("change streams unified format spec automated tests", "[unified_format_spec]") {
    CHECK(run_unified_format_tests_in_env_dir("CHANGE_STREAMS_UNIFIED_TESTS_PATH"));
}

TEST_CASE("versioned API spec automated tests", "[unified_format_spec]") {
    CHECK(run_unified_format_tests_in_env_dir("VERSIONED_API_TESTS_PATH"));
}

TEST_CASE("collection management spec automated tests", "[unified_format_spec]") {
    CHECK(run_unified_format_tests_in_env_dir("COLLECTION_MANAGEMENT_TESTS_PATH"));
}

}  // namespace
