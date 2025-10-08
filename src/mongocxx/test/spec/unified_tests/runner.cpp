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

#include "assert.hh"
#include "entity.hh"
#include "operations.hh"

#include <fstream>
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

#include <mongocxx/client_encryption.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>

#include <bsoncxx/test/catch.hh>

#include <mongocxx/test/client_helpers.hh>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/util.hh>

namespace {

using namespace mongocxx;
using namespace bsoncxx;
using namespace spec;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using schema_versions_t = std::array<std::array<int, 3 /* major.minor.patch */>, 2 /* supported version */>;
constexpr schema_versions_t schema_versions{{{{1, 1, 0}}, {{1, 8, 0}}}};

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

auto const kLocalMasterKey =
    "\x32\x78\x34\x34\x2b\x78\x64\x75\x54\x61\x42\x42\x6b\x59\x31\x36\x45\x72"
    "\x35\x44\x75\x41\x44\x61\x67\x68\x76\x53\x34\x76\x77\x64\x6b\x67\x38\x74"
    "\x70\x50\x70\x33\x74\x7a\x36\x67\x56\x30\x31\x41\x31\x43\x77\x62\x44\x39"
    "\x69\x74\x51\x32\x48\x46\x44\x67\x50\x57\x4f\x70\x38\x65\x4d\x61\x43\x31"
    "\x4f\x69\x37\x36\x36\x4a\x7a\x58\x5a\x42\x64\x42\x64\x62\x64\x4d\x75\x72"
    "\x64\x6f\x6e\x4a\x31\x64";

bsoncxx::document::value get_kms_values() {
    char key_storage[96];
    memcpy(&(key_storage[0]), kLocalMasterKey, 96);
    bsoncxx::types::b_binary const local_master_key{
        bsoncxx::binary_sub_type::k_binary, 96, reinterpret_cast<uint8_t const*>(&key_storage)};

    auto kms_doc = make_document(
        kvp("aws",
            make_document(
                kvp("accessKeyId", test_util::getenv_or_fail("MONGOCXX_TEST_AWS_ACCESS_KEY_ID")),
                kvp("secretAccessKey", test_util::getenv_or_fail("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY")))),
        kvp("azure",
            make_document(
                kvp("tenantId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_TENANT_ID")),
                kvp("clientId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_ID")),
                kvp("clientSecret", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_SECRET")))),
        kvp("gcp",
            make_document(
                kvp("email", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_EMAIL")),
                kvp("privateKey", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_PRIVATEKEY")))),
        kvp("kmip", make_document(kvp("endpoint", "localhost:5698"))),
        kvp("local", make_document(kvp("key", local_master_key))));

    return kms_doc;
}

bsoncxx::document::value parse_kms_doc(bsoncxx::document::view_or_value test_kms_doc) {
    auto const kms_values = get_kms_values();
    auto doc = bsoncxx::builder::basic::document{};
    auto const test_kms_doc_view = test_kms_doc.view();
    for (auto const& it : test_kms_doc_view) {
        auto const provider = it.key();
        if (!kms_values[provider]) {
            FAIL("FAIL: got unexpected KMS provider: " << provider);
        }
        auto variables_doc = bsoncxx::builder::basic::document{};
        auto const variables = test_kms_doc_view[provider].get_document().view();
        for (auto const& i : variables) {
            auto const variable = i.key();
            auto const actual_value = kms_values[provider][variable];
            if (!kms_values[provider][variable]) {
                FAIL(
                    "FAIL: expecting to find variable: '" << variable << "' in KMS doc for provider: '" << provider
                                                          << "'");
            }
            bool is_placeholder = false;
            if (i.type() == bsoncxx::type::k_document &&
                i.get_document().value == make_document(kvp("$$placeholder", 1))) {
                is_placeholder = true;
            }
            if (!is_placeholder) {
                // Append value as-is.
                variables_doc.append(kvp(variable, i.get_value()));
                continue;
            }
            // A placeholder was specified. Append the credential from the environment.
            switch (actual_value.type()) {
                case bsoncxx::type::k_string:
                    variables_doc.append(kvp(variable, actual_value.get_string()));
                    break;

                case bsoncxx::type::k_binary:
                    variables_doc.append(kvp(variable, actual_value.get_binary()));
                    break;

                case bsoncxx::type::k_double:
                case bsoncxx::type::k_document:
                case bsoncxx::type::k_array:
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
                    FAIL(
                        "FAIL: unexpected variable type in KMS doc: '" << bsoncxx::to_string(actual_value.type())
                                                                       << "'");
            }
        }
        doc.append(kvp(provider, variables_doc.extract()));
    }
    return doc.extract();
}

// Spec: Version strings, which are used for schemaVersion and runOnRequirement, MUST conform to
// one of the following formats, where each component is a non-negative integer:
//      <major>.<minor>.<patch>
//      <major>.<minor> (<patch> is assumed to be zero)
//      <major> (<minor> and <patch> are assumed to be zero)
std::vector<int> get_version(std::string const& input) {
    std::vector<int> output;
    std::regex const period("\\.");
    std::transform(
        std::sregex_token_iterator(std::begin(input), std::end(input), period, -1),
        std::sregex_token_iterator(),
        std::back_inserter(output),
        [](std::string const& s) { return std::stoi(s); });

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

bool equals_server_topology(document::element const& topologies) {
    using bsoncxx::types::bson_value::value;

    // The server's topology will not change during the test. No need to make a round-trip for every
    // test file.
    static auto const actual = value(test_util::get_topology());

    auto const t = topologies.get_array().value;
    return std::end(t) != std::find(std::begin(t), std::end(t), actual);
}

bool compatible_with_server(bsoncxx::array::element const& requirement) {
    // The server's version will not change during the test. No need to make a round-trip for every
    // test file.
    static std::vector<int> const expected = get_version(test_util::get_server_version());

    if (auto const min_server_version = requirement["minServerVersion"]) {
        auto const actual = get_version(min_server_version);
        if (!is_compatible_version(actual, expected))
            return false;
    }

    if (auto const max_server_version = requirement["maxServerVersion"]) {
        auto const actual = get_version(max_server_version);
        if (!is_compatible_version(expected, actual))
            return false;
    }

    if (auto const topologies = requirement["topologies"])
        return equals_server_topology(topologies);

    if (auto const server_params = requirement["serverParameters"]) {
        document::value actual = make_document();
        try {
            actual = test_util::get_server_params();
        } catch (operation_exception const& e) {
            // Mongohouse does not support getParameter, so if we get an error from
            // getParameter, exit this logic early and skip the test.
            std::string const message = e.what();
            if (message.find("command getParameter is unsupported") != std::string::npos) {
                return false;
            }

            throw e;
        }

        for (auto const& kvp : server_params.get_document().view()) {
            auto const param = kvp.key();
            auto const value = kvp.get_value();
            // If actual parameter is unset or unequal to requirement, skip test.
            if (!actual[param] || actual[param].get_bool() != value.get_bool()) {
                return false;
            }
        }
    }

    if (auto const csfle = requirement["csfle"]) {
        // csfle: Optional boolean. If true, the tests MUST only run if the
        // driver and server support Client-Side Field Level Encryption. A
        // server supports CSFLE if it is version 4.2.0 or higher. If false,
        // tests MUST only run if CSFLE is not enabled. If this field is
        // omitted, there is no CSFLE requirement.
        std::vector<int> const requires_at_least{4, 2, 0};
        bool const is_csfle = csfle.get_bool().value;
        if (is_csfle) {
            if (!is_compatible_version(requires_at_least, expected)) {
                return false;
            }
        }
    }
    return true;
}

bool has_run_on_requirements(bsoncxx::document::view const test) {
    if (!test["runOnRequirements"])
        return true;

    auto const requirements = test["runOnRequirements"].get_array().value;
    return std::any_of(std::begin(requirements), std::end(requirements), compatible_with_server);
}

std::string json_kvp_to_uri_kvp(std::string s) {
    // The transformation is as follows:
    //     1. "{ "key" : "value" }"     -- initial representation
    //     2. "key:value"               -- intermediate step (without quotes)
    //     3. "key=value"               -- final step (without quotes)

    using namespace std;
    auto const should_remove = [&](char const c) {
        auto const remove = {' ', '"', '{', '}'};
        return end(remove) != find(begin(remove), end(remove), c);
    };

    s.erase(remove_if(begin(s), end(s), should_remove), end(s));
    replace(begin(s), end(s), ':', '=');
    return s;
}

std::string json_to_uri_opts(std::string const& input) {
    // Transforms a non-nested JSON document string (assumed to contain URI keys and values) to a
    // string of equivalent URI options and values. That is,
    //      input   := "{ "readConcernLevel" : "local", "w" : 1 }"
    //      output  := "readConcernLevel=local&w=1"
    std::vector<std::string> output;
    std::regex const delim(",");
    std::transform(
        std::sregex_token_iterator(std::begin(input), std::end(input), delim, -1),
        std::sregex_token_iterator(),
        std::back_inserter(output),
        json_kvp_to_uri_kvp);

    auto const join = [](std::string const& s1, std::string const& s2) { return s1 + "&" + s2; };
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

    auto const json = to_json(object["uriOptions"].get_document());
    auto const opts = json_to_uri_opts(json);

    CAPTURE(json, opts);
    return opts;
}

std::string get_hostnames(bsoncxx::document::view object) {
    auto const uri0 = mongocxx::uri("mongodb://localhost:27017");

    // All test topologies should have either a mongod or mongos on localhost:27017.
    mongocxx::client const client0{uri0, test_util::add_test_server_api()};
    REQUIRE_NOTHROW(client0.list_databases().begin());

    // The topology must be consistent with what was set up by the test environment.
    static constexpr auto one = "localhost:27017";
    static constexpr auto two = "localhost:27017,localhost:27018";
    static constexpr auto three = "localhost:27017,localhost:27018,localhost:27019";

    auto const topology = test_util::get_topology();

    if (topology == "single") {
        return one; // Single mongod.
    }

    if (topology == "replicaset") {
        return three; // Three replset members.
    }

    if (topology == "sharded") {
        auto const use_multiple_mongoses = object["useMultipleMongoses"];

        if (use_multiple_mongoses) {
            auto const value = use_multiple_mongoses.get_bool().value;

            if (value) {
                auto const uri1 = mongocxx::uri("mongodb://localhost:27018");

                // If true and the topology is a sharded cluster, the test runner MUST assert that
                // this MongoClient connects to multiple mongos hosts (e.g. by inspecting the
                // connection string).
                mongocxx::client const client1{uri1, test_util::add_test_server_api()};

                if (!client0["config"].has_collection("shards")) {
                    FAIL("missing required mongos on port 27017 with useMultipleMongoses=true");
                }

                if (!client1["config"].has_collection("shards")) {
                    FAIL("missing required mongos on port 27018 with useMultipleMongoses=true");
                }

                return two; // Two mongoses.
            } else {
                // If false and the topology is a sharded cluster, the test runner MUST ensure that
                // this MongoClient connects to only a single mongos host (e.g. by modifying the
                // connection string).
                return one; // Single mongos.
            }
        } else {
            // If this option is not specified and the topology is a sharded cluster, the test
            // runner MUST NOT enforce any limit on the number of mongos hosts in the connection
            // string and any tests using this client SHOULD NOT depend on a particular number of
            // mongos hosts.

            // But we still only support exactly two mongoses.
            return two; // Two mongoses.
        }
    }

    FAIL("unexpected topology: " << topology);
    return {}; // -Wreturn-type
}

void add_observe_events(spec::apm_checker& apm, options::apm& apm_opts, document::view object) {
    if (!object["observeEvents"]) {
        return;
    }

    auto const observe_sensitive = object["observeSensitiveCommands"];
    apm.observe_sensitive_events = observe_sensitive && observe_sensitive.get_bool();

    auto const events = object["observeEvents"].get_array().value;

    for (auto const& event : events) {
        auto const event_type = event.get_string().value;
        if (event_type == bsoncxx::stdx::string_view("commandStartedEvent")) {
            apm.set_command_started_unified(apm_opts);
        } else if (event_type == bsoncxx::stdx::string_view("commandSucceededEvent")) {
            apm.set_command_succeeded_unified(apm_opts);
        } else if (event_type == bsoncxx::stdx::string_view("commandFailedEvent")) {
            apm.set_command_failed_unified(apm_opts);
        } else {
            UNSCOPED_INFO("ignoring unsupported command monitoring event " << event_type);
        }
    }
}

void add_ignore_command_monitoring_events(spec::apm_checker& apm, document::view object) {
    if (!object["ignoreCommandMonitoringEvents"]) {
        return;
    }

    for (auto cme : object["ignoreCommandMonitoringEvents"].get_array().value) {
        CAPTURE(cme.get_string());
        apm.set_ignore_command_monitoring_event(string::to_string(cme.get_string().value));
    }
}

options::server_api create_server_api(document::view object) {
    document::element sav;
    if (!(sav = object["serverApi"]["version"])) {
        FAIL("must specify a version when using serverApi");
    }

    REQUIRE(sav.type() == type::k_string);
    auto const version = options::server_api::version_from_string(sav.get_string().value);
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

read_preference get_read_preference(document::element const& opts) {
    read_preference rp;

    auto const read_pref = opts["readPreference"];

    if (auto const mss = read_pref["maxStalenessSeconds"]) {
        rp.max_staleness(std::chrono::seconds(mss.get_int32().value));
    }

    auto const mode = read_pref["mode"].get_string().value;

    if (mode == "secondaryPreferred") {
        rp.mode(read_preference::read_mode::k_secondary_preferred);
    } else {
        FAIL("unhandled readPreference mode: " << mode);
    }

    return rp;
}

write_concern get_write_concern(document::element const& opts) {
    auto wc = write_concern{};
    if (auto w = opts["writeConcern"]["w"]) {
        if (w.type() == type::k_string) {
            auto const strval = w.get_string().value;
            if (strval == "majority") {
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

read_concern get_read_concern(document::element const& opts) {
    auto rc = read_concern{};

    if (auto const level = opts["readConcern"]["level"]) {
        rc.acknowledge_string(level.get_string().value);
    }

    return rc;
}

template <typename T>
void set_common_options(T& t, document::element const& opts) {
    if (!opts)
        return;

    if (opts["readConcern"]) {
        t.read_concern(get_read_concern(opts));
    }

    if (opts["writeConcern"]) {
        t.write_concern(get_write_concern(opts));
    }

    if (opts["readPreference"]) {
        t.read_preference(get_read_preference(opts));
    }
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

options::client_encryption get_client_encryption_options(document::view object) {
    auto const key_vault_namespace =
        std::string(object["clientEncryptionOpts"]["keyVaultNamespace"].get_string().value);
    auto const dot = key_vault_namespace.find(".");
    std::string const db = key_vault_namespace.substr(0, dot);
    std::string const coll = key_vault_namespace.substr(dot + 1);

    auto const id = string::to_string(object["clientEncryptionOpts"]["keyVaultClient"].get_string().value);

    auto& map = get_entity_map();
    auto& client = map.get_client(id);
    CAPTURE(id);

    auto const providers = object["clientEncryptionOpts"]["kmsProviders"].get_document().value;

    options::client_encryption ce_opts;
    ce_opts.key_vault_client(&client);
    ce_opts.key_vault_namespace({db, coll});
    ce_opts.kms_providers(parse_kms_doc(providers));

    if (!providers.empty()) {
        // Configure TLS options.
        auto tls_opts = make_document(
            kvp("kmip",
                make_document(
                    kvp("tlsCAFile", test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CA_FILE")),
                    kvp("tlsCertificateKeyFile",
                        test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE")))));
        ce_opts.tls_opts(std::move(tls_opts));
    }
    return ce_opts;
}

gridfs::bucket create_bucket(document::view object) {
    auto const id = string::to_string(object["database"].get_string().value);
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto const opts = get_bucket_options(object);
    auto const bucket = db.gridfs_bucket(opts);

    CAPTURE(id);
    return bucket;
}

client_session create_session(document::view object) {
    auto const id = string::to_string(object["client"].get_string().value);
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto const opts = get_session_options(object);
    auto session = client.start_session(opts);

    CAPTURE(id);
    return session;
}

client_encryption create_client_encryption(document::view object) {
    auto const opts = get_client_encryption_options(object);
    client_encryption ce(std::move(opts));

    return ce;
}

collection create_collection(document::view object) {
    auto const id = string::to_string(object["database"].get_string().value);
    auto& map = get_entity_map();
    auto& db = map.get_database(id);

    auto const name = string::to_string(object["collectionName"].get_string().value);
    auto coll = collection{db.collection(name)};

    set_common_options(coll, object["collectionOptions"]);

    CAPTURE(name, id);
    return coll;
}

database create_database(document::view object) {
    auto const id = string::to_string(object["client"].get_string().value);
    auto& map = get_entity_map();
    auto& client = map.get_client(id);

    auto const name = string::to_string(object["databaseName"].get_string().value);
    auto db = database{client.database(name)};

    set_common_options(db, object["databaseOptions"]);

    CAPTURE(name, id);
    return db;
}

client create_client(document::view object) {
    auto const conn = "mongodb://" + get_hostnames(object) + "/?" + uri_options_to_string(object);
    auto apm_opts = options::apm{};
    auto client_opts = test_util::add_test_server_api();
    // Use specified serverApi or default if none is provided.
    if (object["serverApi"]) {
        auto const server_api_opts = create_server_api(object);
        client_opts.server_api_opts(server_api_opts);
    }
    auto& apm = get_apm_map()[string::to_string(object["id"].get_string().value)];

    add_observe_events(apm, apm_opts, object);
    add_ignore_command_monitoring_events(apm, object);

    // The test runner MUST also ensure that the configureFailPoint command is excluded from the
    // list of observed command monitoring events for this client (if applicable).
    apm.set_ignore_command_monitoring_event("configureFailPoint");

    CAPTURE(conn);
    return client{uri{conn}, client_opts.apm_opts(apm_opts)};
}

bool add_to_map(array::element const& obj) {
    // Spec: This object MUST contain exactly one top-level key that identifies the entity type and
    // maps to a nested object, which specifies a unique name for the entity ('id' key) and any
    // other parameters necessary for its construction.
    auto doc = obj.get_document().view().begin();
    auto const type = string::to_string(doc->key());
    auto const params = doc->get_document().view();
    auto const id = string::to_string(params["id"].get_string().value);
    auto& map = get_entity_map();

    if (type == "client") {
        return map.insert(id, create_client(params));
    } else if (type == "database") {
        return map.insert(id, create_database(params));
    } else if (type == "collection") {
        return map.insert(id, create_collection(params));
    } else if (type == "bucket") {
        return map.insert(id, create_bucket(params));
    } else if (type == "session") {
        return map.insert(id, create_session(params));
    } else if (type == "clientEncryption") {
        return map.insert(id, create_client_encryption(params));
    }

    CAPTURE(type, id, params);
    FAIL("unrecognized type { " + type + " }");
    return false;
}

void create_entities(document::view const test) {
    if (!test["createEntities"])
        return;

    get_entity_map().clear();
    get_apm_map().clear();
    auto const entities = test["createEntities"].get_array().value;
    REQUIRE(std::all_of(std::begin(entities), std::end(entities), add_to_map));
}

document::value parse_test_file(std::string const& test_path) {
    bsoncxx::stdx::optional<document::value> const test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);
    return test_spec.value();
}

bool is_compatible_schema_version(document::view test_spec) {
    REQUIRE(test_spec["schemaVersion"]);
    auto const test_schema_version = get_version(test_spec["schemaVersion"]);
    auto const compat = [&](std::array<int, 3> v) {
        // Test files are considered compatible with a test runner if their schemaVersion is less
        // than or equal to a supported version in the test runner, given the same major version
        // component.
        return test_schema_version[0] == v[0] && is_compatible_version(test_schema_version, v, ignore_patch::yes);
    };
    return std::any_of(std::begin(schema_versions), std::end(schema_versions), compat);
}

std::vector<std::string> versions_to_string(schema_versions_t versions) {
    std::vector<std::string> out;
    for (auto const& v : versions) {
        std::stringstream v_str;
        v_str << std::to_string(v[0]) << '.' // major.
              << std::to_string(v[1]) << '.' // minor.
              << std::to_string(v[2]);       // patch
        out.push_back(v_str.str());
    }
    return out;
}

std::vector<document::view> array_elements_to_documents(array::view array) {
    // no implicit conversion from 'bsoncxx::array::view' to 'bsoncxx::document::view'
    auto docs = std::vector<document::view>{};
    auto const arr_to_doc = [](array::element const& doc) { return doc.get_document().value; };

    std::transform(std::begin(array), std::end(array), std::back_inserter(docs), arr_to_doc);
    return docs;
}

void add_data_to_collection(array::element const& data) {
    auto const db_name = data["databaseName"].get_string().value;
    auto& map = get_entity_map();
    auto& db = map.get_database_by_name(db_name);
    auto insert_opts = mongocxx::options::insert();

    auto wc = write_concern{};
    wc.acknowledge_level(write_concern::level::k_majority);
    wc.majority(std::chrono::milliseconds{0});

    auto const coll_name = data["collectionName"].get_string().value;

    if (db.has_collection(coll_name))
        db[coll_name].drop();

    auto coll = db.create_collection(coll_name, {}, wc);
    insert_opts.write_concern(wc);

    auto const to_insert = array_elements_to_documents(data["documents"].get_array().value);
    REQUIRE((to_insert.empty() || coll.insert_many(to_insert, insert_opts)->result().inserted_count() != 0));
}

void load_initial_data(document::view test) {
    if (!test["initialData"])
        return;

    auto const data = test["initialData"].get_array().value;
    for (auto&& d : data)
        add_data_to_collection(d);
}

void assert_result(array::element const& ops, document::view actual_result, bool is_array_of_root_docs) {
    if (!ops["expectResult"]) {
        return;
    }

    auto const expected_result = ops["expectResult"];
    assert::matches(
        actual_result["result"].get_value(),
        expected_result.get_value(),
        get_entity_map(),
        true,
        is_array_of_root_docs);

    if (ops["saveResultAsEntity"]) {
        auto const key = string::to_string(ops["saveResultAsEntity"].get_string().value);
        get_entity_map().insert(key, actual_result);
    }
}

void assert_error(
    mongocxx::operation_exception const& exception,
    array::element const& expected,
    document::view actual) {
    std::string const server_error_msg =
        exception.raw_server_error() ? to_json(*exception.raw_server_error()) : "no server error";

    CAPTURE(exception.what(), server_error_msg);

    auto const expect_error = expected["expectError"];
    REQUIRE(expect_error);

    if (expect_error["isError"])
        return;

    auto const actual_result = actual["result"];
    if (auto const expected_result = expect_error["expectResult"]) {
        assert::matches(actual_result.get_value(), expected_result.get_value(), get_entity_map());
    }

    if (auto const is_client_error = expect_error["isClientError"]) {
        // An explicit list of client-side errors. We do not yet have a reliable and consistent
        // method to distinguish client-side errors from server-side errors. (CXX-2377)
        static bsoncxx::stdx::string_view const patterns[] = {
            // { MONGOC_ERROR_CLIENT, MONGOC_ERROR_CLIENT_SESSION_FAILURE }
            // mongoc: mongoc_cmd_parts_assemble
            "Snapshot reads require MongoDB 5.0 or later",

            // { MONGOC_ERROR_COMMAND, MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION }
            // mongoc: mongoc_collection_find_and_modify_with_opts,
            // _mongoc_write_command_execute_idl
            "The selected server does not support hint for",

            // { MONGOC_ERROR_STREAM, MONGOC_ERROR_STREAM_NAME_RESOLUTION }
            // mongoc: mongoc_client_connect_tcp, mongoc_topology_scanner_node_setup_tcp
            "Failed to resolve ",

            // { MONGOCRYPT_STATUS_ERROR_CLIENT, MONGOCRYPT_GENERIC_ERROR_CODE }
            // libmongocrypt: mongocrypt_kms_ctx_feed
            "Error in KMS response",

            // { MONGOCRYPT_STATUS_ERROR_CLIENT, MONGOCRYPT_GENERIC_ERROR_CODE }
            // libmongocrypt: mongocrypt_ctx_setopt_key_material
            "keyMaterial should have length 96, but has length 84",

            // { MONGOCRYPT_STATUS_ERROR_CLIENT, MONGOCRYPT_GENERIC_ERROR_CODE }
            // libmongocrypt: _mongocrypt_parse_optional_utf8, _mongocrypt_parse_required_utf8
            "expected UTF-8 key",

            // { MONGOCRYPT_STATUS_ERROR_CLIENT, MONGOCRYPT_GENERIC_ERROR_CODE }
            // libmongocrypt: _mongocrypt_check_allowed_fields
            "Unexpected field: 'invalid'",

            // { MONGOCRYPT_STATUS_ERROR_CLIENT, MONGOCRYPT_GENERIC_ERROR_CODE }
            // libmongocrypt: _kms_done
            "key material not expected length",
        };

        bsoncxx::stdx::string_view const message = exception.what();

        auto const iter =
            std::find_if(std::begin(patterns), std::end(patterns), [message](bsoncxx::stdx::string_view pattern) {
                return message.find(pattern) != message.npos;
            });

        if (iter != std::end(patterns)) {
            // Treat this as a client-side error.
            auto const pattern = *iter;
            CAPTURE(pattern);
            REQUIRE(is_client_error.get_bool().value);
        } else {
            // Treat this as a server-side error.
            REQUIRE(!is_client_error.get_bool().value);
        }
    }

    if (auto const contains = expect_error["errorLabelsContain"]) {
        auto const labels = contains.get_array().value;
        auto const has_error_label = [&](array::element const& ele) {
            return exception.has_error_label(ele.get_string().value);
        };
        REQUIRE(std::all_of(std::begin(labels), std::end(labels), has_error_label));
    }

    if (auto const omit = expect_error["errorLabelsOmit"]) {
        auto const labels = omit.get_array().value;
        auto const has_error_label = [&](array::element const& ele) {
            return exception.has_error_label(ele.get_string().value);
        };
        REQUIRE(std::none_of(std::begin(labels), std::end(labels), has_error_label));
    }

    if (auto const expected_code = expect_error["errorCode"]) {
        auto const actual_code = exception.code().value();
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
    "https://github.com/mongodb/specifications/blob/master/source/unified-test-format/unified-test-format.md#expectederror":
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

void assert_error(mongocxx::exception& e, array::element const& ops) {
    CAPTURE(e.what());
    auto const expect_error = ops["expectError"];
    REQUIRE(expect_error);

    if (expect_error["isError"])
        return;

    if (auto const is_client_error = expect_error["isClientError"]) {
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

void assert_events(array::element const& test) {
    if (!test["expectEvents"])
        return;

    for (auto e : test["expectEvents"].get_array().value) {
        auto const ignore_extra_events = [&]() -> bool {
            auto const elem = e["ignoreExtraEvents"];
            return elem && elem.get_bool().value;
        }();
        auto const events = e["events"].get_array().value;
        auto const name = string::to_string(e["client"].get_string().value);
        get_apm_map()[name].compare_unified(events, get_entity_map(), ignore_extra_events);
    }
}

void assert_outcome(array::element const& test) {
    using std::begin;
    using std::end;
    using std::equal;

    if (!test["outcome"]) {
        return;
    }

    read_preference rp;
    rp.mode(read_preference::read_mode::k_primary);

    read_concern rc;
    rc.acknowledge_level(read_concern::level::k_local);

    for (auto const& outcome : test["outcome"].get_array().value) {
        CAPTURE(to_json(outcome.get_document()));

        auto const db_name = outcome["databaseName"].get_string().value;
        auto const coll_name = outcome["collectionName"].get_string().value;
        auto const docs = outcome["documents"].get_array().value;

        auto const db = get_entity_map().get_database_by_name(db_name);
        auto coll = db.collection(coll_name);

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
        } coll_state_guard(coll);

        // The test runner MUST query each collection using the internal MongoClient, an ascending
        // sort order on the `_id` field (i.e. `{ _id: 1 }`), a "primary" read preference, and a
        // "local" read concern.
        coll.read_preference(rp);
        coll.read_concern(rc);

        auto results = coll.find({}, options::find{}.sort(make_document(kvp("_id", 1))));

        auto actual = results.begin();
        for (auto const& expected : docs) {
            assert::matches(types::bson_value::value(*actual), expected.get_value(), get_entity_map());
            ++actual;
        }

        REQUIRE(begin(results) == end(results)); /* cursor is exhausted */
    }
}

struct fail_point_guard_type {
    std::vector<std::pair<std::string, std::string>> fail_points;

    fail_point_guard_type() = default;

    ~fail_point_guard_type() {
        try {
            for (auto const& f : fail_points) {
                spec::disable_fail_point(f.first, {}, f.second);
            }
        } catch (...) {
        }
    }

    void add_fail_point(std::string uri, std::string command) {
        fail_points.emplace_back(std::move(uri), std::move(command));
    }
};

void disable_targeted_fail_point(
    bsoncxx::stdx::string_view uri,
    std::uint32_t server_id,
    bsoncxx::stdx::string_view fail_point) {
    auto const command_owner = make_document(kvp("configureFailPoint", fail_point), kvp("mode", "off"));
    auto const command = command_owner.view();

    // Unlike in the legacy test runner, there are no tests (at time of writing) that require
    // multiple attempts to disable a targetedFailPoint, so only one attempt should suffice.
    mongocxx::client client = {mongocxx::uri{uri}, test_util::add_test_server_api()};
    client["admin"].run_command(command, server_id);
}

struct targeted_fail_point_guard_type {
    std::vector<std::tuple<std::string, std::uint32_t, std::string>> fail_points;

    targeted_fail_point_guard_type() = default;

    ~targeted_fail_point_guard_type() {
        try {
            for (auto const& f : fail_points) {
                disable_targeted_fail_point(std::get<0>(f), std::get<1>(f), std::get<2>(f));
            }
        } catch (...) {
        }
    }

    void add_fail_point(std::string uri, std::uint32_t server_id, std::string command) {
        fail_points.emplace_back(std::move(uri), server_id, std::move(command));
    }
};

document::value bulk_write_result(mongocxx::bulk_write_exception const& e) {
    auto const reply = e.raw_server_error().value();

    auto const get_or_default = [&](bsoncxx::stdx::string_view key) {
        return reply[key] ? reply[key].get_int32().value : 0;
    };

    auto result = bsoncxx::builder::basic::document{};
    result.append(
        kvp("result",
            make_document(
                kvp("matchedCount", get_or_default("nMatched")),
                kvp("modifiedCount", get_or_default("nModified")),
                kvp("upsertedCount", get_or_default("nUpserted")),
                kvp("deletedCount", get_or_default("nRemoved")),
                kvp("insertedCount", get_or_default("nInserted")),
                kvp("upsertedIds", make_document()))));

    return result.extract();
}

// Match test cases that should be skipped by both test and case descriptions.
std::map<std::pair<bsoncxx::stdx::string_view, bsoncxx::stdx::string_view>, bsoncxx::stdx::string_view> const
    should_skip_test_cases = {
        {{"retryable reads handshake failures", "collection.findOne succeeds after retryable handshake network error"},
         "collection.findOne optional helper is not supported"},
        {{"retryable reads handshake failures",
          "collection.findOne succeeds after retryable handshake server error "
          "(ShutdownInProgress)"},
         "collection.findOne optional helper is not supported"},
        {{"retryable reads handshake failures",
          "collection.listIndexNames succeeds after retryable handshake network error"},
         "collection.listIndexNames optional helper is not supported"},
        {{"retryable reads handshake failures",
          "collection.listIndexNames succeeds after retryable handshake server error "
          "(ShutdownInProgress)"},
         "collection.listIndexNames optional helper is not supported"},
};

void run_tests(bsoncxx::stdx::string_view test_description, document::view test) {
    REQUIRE(test["tests"]);

    for (auto const& ele : test["tests"].get_array().value) {
        auto const description = string::to_string(ele["description"].get_string().value);

        DYNAMIC_SECTION(description) {
            {
                auto const iter = should_skip_test_cases.find({test_description, description});
                if (iter != should_skip_test_cases.end()) {
                    SKIP(test_description << ": " << description << ": unsupported test case");
                }
            }

            if (!has_run_on_requirements(ele.get_document())) {
                SKIP(
                    test_description << ": " << description << ": none of the runOnRequirements were met: "
                                     << to_json(ele["runOnRequirements"].get_array().value));
            }

            if (ele["skipReason"]) {
                SKIP(
                    test_description << ": " << description << ": "
                                     << string::to_string(ele["skipReason"].get_string().value));
            }

            fail_point_guard_type fail_point_guard;
            targeted_fail_point_guard_type targeted_fail_point_guard;

            for (auto&& apm : get_apm_map()) {
                apm.second.clear_events();
            }

            operations::state state;

            for (auto const& ops : ele["operations"].get_array().value) {
                auto const ignore_result_and_error = [&]() -> bool {
                    auto const elem = ops["ignoreResultAndError"];
                    return elem && elem.get_bool().value;
                }();

                try {
                    auto const result = operations::run(get_entity_map(), get_apm_map(), ops, state);

                    if (string::to_string(ops["object"].get_string().value) == "testRunner") {
                        auto const op_name = string::to_string(ops["name"].get_string().value);

                        if (op_name == "failPoint") {
                            fail_point_guard.add_fail_point(
                                string::to_string(result["uri"].get_string().value),
                                string::to_string(result["failPoint"].get_string().value));
                        }

                        if (op_name == "targetedFailPoint") {
                            targeted_fail_point_guard.add_fail_point(
                                string::to_string(result["uri"].get_string().value),
                                static_cast<std::uint32_t>(result["serverId"].get_int64().value),
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
                        static std::unordered_set<std::string> const names = {
                            "aggregate",
                            "find",
                            "iterateUntilDocumentOrError",
                            "listCollections",
                            "listDatabases",
                            "listIndexes",
                        };

                        auto const name = string::to_string(ops["name"].get_string().value);

                        is_array_of_root_docs = names.find(name) != names.end();
                    }

                    if (!ignore_result_and_error) {
                        assert_result(ops, result, is_array_of_root_docs);
                    }
                } catch (mongocxx::bulk_write_exception const& e) {
                    if (!ignore_result_and_error) {
                        auto result = bulk_write_result(e);
                        assert_error(e, ops, result);
                    }
                } catch (mongocxx::operation_exception const& e) {
                    if (!ignore_result_and_error) {
                        assert_error(e, ops, make_document());
                    }
                } catch (mongocxx::exception& e) {
                    if (!ignore_result_and_error) {
                        assert_error(e, ops);
                    }
                }
            }

            assert_events(ele);
            assert_outcome(ele);
        }
    }
}

void run_tests_in_file(std::string const& test_path) {
    auto const test_spec = parse_test_file(test_path);
    auto const test_spec_view = test_spec.view();

    if (!is_compatible_schema_version(test_spec_view)) {
        std::stringstream error;
        error << "incompatible schema version" << std::endl
              << "Expected: " << test_spec_view["schemaVersion"].get_string().value << std::endl
              << "Supported versions:" << std::endl;

        auto const v = versions_to_string(schema_versions);
        std::copy(std::begin(v), std::end(v), std::ostream_iterator<std::string>(error, "\n"));

        FAIL(error.str());
        return;
    }

    if (!has_run_on_requirements(test_spec_view)) {
        CAPTURE(to_json(test_spec_view["runOnRequirements"].get_array().value));
        SKIP(test_path << ": none of the runOnRequirements were met");
    }

    auto const description = test_spec_view["description"].get_string().value;
    CAPTURE(description);
    create_entities(test_spec_view);
    load_initial_data(test_spec_view);
    run_tests(description, test_spec_view);
}

// Check the environment for the specified variable; if present, extract it
// as a directory and run all the tests contained in the magic "test_files.txt"
// file:
void run_unified_format_tests_in_env_dir(
    std::string const& env_path,
    std::set<bsoncxx::stdx::string_view> const& unsupported_tests = {}) {
    char const* p = std::getenv(env_path.c_str());

    if (nullptr == p)
        FAIL("unable to look up path from environment variable \"" << env_path << "\"");

    std::string const base_path{p};

    auto const test_file_set_path = base_path + "/test_files.txt";
    std::ifstream files{test_file_set_path};

    if (!files.good()) {
        FAIL("unable to find/open test_files.txt in path \"" << test_file_set_path << '\"');
    }

    instance::current();

    for (std::string file; std::getline(files, file);) {
        DYNAMIC_SECTION(file) {
            if (unsupported_tests.find(file) != unsupported_tests.end()) {
                SKIP("unsupported test file: " << file);
            }

            run_tests_in_file(base_path + '/' + file);
        }
    }
}

TEST_CASE("unified format spec automated tests", "[unified_format_specs]") {
    std::set<bsoncxx::stdx::string_view> const unsupported_tests = {
        // Waiting on CDRIVER-3525 and CXX-2166.
        "valid-pass/entity-client-cmap-events.json",
        // Waiting on CDRIVER-3525 and CXX-2166.
        "valid-pass/assertNumberConnectionsCheckedOut.json"};

    run_unified_format_tests_in_env_dir("UNIFIED_FORMAT_TESTS_PATH", unsupported_tests);
}

TEST_CASE("session unified format spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("SESSION_UNIFIED_TESTS_PATH");
}

TEST_CASE("CRUD unified format spec automated tests", "[unified_format_specs]") {
    std::set<bsoncxx::stdx::string_view> const unsupported_tests = {
        // Waiting on CXX-2494.
        "client-bulkWrite-replaceOne-sort.json",
        // Waiting on CXX-2494.
        "client-bulkWrite-updateOne-sort.json",
    };

    run_unified_format_tests_in_env_dir("CRUD_UNIFIED_TESTS_PATH", unsupported_tests);
}

TEST_CASE("change streams unified format spec automated tests", "[unified_format_specs]") {
    std::set<bsoncxx::stdx::string_view> const unsupported_tests = {
        // Waiting on CXX-2493 (showExpandedEvents).
        "change-streams-disambiguatedPaths.json",
        // Waiting on CXX-2493 (showExpandedEvents).
        "change-streams-nsType.json",
        // Waiting on CXX-2493 (showExpandedEvents).
        "change-streams-showExpandedEvents.json",
    };

    run_unified_format_tests_in_env_dir("CHANGE_STREAMS_UNIFIED_TESTS_PATH", unsupported_tests);
}

TEST_CASE("retryable reads unified format spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("RETRYABLE_READS_UNIFIED_TESTS_PATH");
}

TEST_CASE("retryable writes unified format spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("RETRYABLE_WRITES_UNIFIED_TESTS_PATH");
}

TEST_CASE("transactions unified format spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("TRANSACTIONS_UNIFIED_TESTS_PATH");
}

TEST_CASE("versioned API spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("VERSIONED_API_TESTS_PATH");
}

TEST_CASE("collection management spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("COLLECTION_MANAGEMENT_TESTS_PATH");
}

TEST_CASE("index management spec automated tests", "[unified_format_specs]") {
    run_unified_format_tests_in_env_dir("INDEX_MANAGEMENT_TESTS_PATH");
}

// See:
// https://github.com/mongodb/specifications/blob/master/source/client-side-encryption/client-side-encryption.md
TEST_CASE("client side encryption unified format spec automated tests", "[unified_format_specs]") {
    CLIENT_SIDE_ENCRYPTION_ENABLED_OR_SKIP();
    run_unified_format_tests_in_env_dir("CLIENT_SIDE_ENCRYPTION_UNIFIED_TESTS_PATH");
}

} // namespace
