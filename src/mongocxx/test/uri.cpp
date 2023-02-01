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

#include <algorithm>
#include <cctype>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>

namespace {

static bsoncxx::document::value _doc_from_file(mongocxx::stdx::string_view sub_path) {
    const char* test_path = std::getenv("URI_OPTIONS_TESTS_PATH");
    REQUIRE(test_path);

    std::string path = std::string(test_path) + sub_path.data();
    CAPTURE(path);

    std::ifstream file{path};
    REQUIRE(file);

    std::string file_contents((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    return bsoncxx::from_json(file_contents);
}

TEST_CASE("URI", "[uri]") {
    SECTION("Default URI") {
        REQUIRE_NOTHROW(mongocxx::uri{});
        REQUIRE_NOTHROW(mongocxx::uri{mongocxx::uri::k_default_uri});
        REQUIRE(mongocxx::uri{}.to_string() ==
                mongocxx::uri{mongocxx::uri::k_default_uri}.to_string());

        mongocxx::uri u{};

        // Values that should be empty with a blank URI.
        REQUIRE(u.auth_mechanism() == "");
        REQUIRE(u.auth_source() == "admin");
        REQUIRE(u.database() == "");
        REQUIRE(u.hosts().size() == 1);
        REQUIRE(u.hosts()[0].name == "localhost");
        // Don't check 'u.hosts()[0].family'.  Value is platform-dependent.
        REQUIRE(u.hosts()[0].port == 27017);
        REQUIRE(u.options().empty());
        REQUIRE(u.password() == "");
        REQUIRE(u.read_concern().acknowledge_level() ==
                mongocxx::read_concern::level::k_server_default);
        REQUIRE(u.read_concern().acknowledge_string().empty());
        REQUIRE(u.read_preference().mode() == mongocxx::read_preference::read_mode::k_primary);
        REQUIRE(!u.read_preference().tags());
        REQUIRE(!u.read_preference().max_staleness());
        REQUIRE(u.replica_set() == "");
        REQUIRE(u.tls() == false);
        REQUIRE(u.to_string() == mongocxx::uri::k_default_uri);
        REQUIRE(u.username() == "");
        REQUIRE(u.write_concern().journal() == false);
        REQUIRE(u.write_concern().majority() == false);
        REQUIRE(!u.write_concern().nodes());
        REQUIRE(u.write_concern().timeout() == std::chrono::milliseconds{0});
        REQUIRE(u.write_concern().acknowledge_level() == mongocxx::write_concern::level::k_default);
    }

    SECTION("Valid URI") {
        REQUIRE_NOTHROW(mongocxx::uri{"mongodb://example.com"});
    }

    SECTION("Invalid URI") {
        std::string invalid{"mongo://example.com"};
        REQUIRE_THROWS_AS(mongocxx::uri{invalid}, mongocxx::logic_error);
        try {
            mongocxx::uri{invalid};
        } catch (const mongocxx::logic_error& e) {
            REQUIRE(e.code() == mongocxx::error_code::k_invalid_uri);

            std::string invalid_schema =
                "Invalid URI Schema, expecting 'mongodb://' or 'mongodb+srv://': ";

            REQUIRE(e.what() == invalid_schema + e.code().message());
        }
    }

    SECTION("Multiple hosts") {
        mongocxx::uri uri{"mongodb://host1:123,host2:456"};
        REQUIRE(uri.hosts().size() == 2);
        REQUIRE(uri.hosts()[0].name == "host1");
        REQUIRE(uri.hosts()[0].port == 123);
        REQUIRE(uri.hosts()[1].name == "host2");
        REQUIRE(uri.hosts()[1].port == 456);
    }

    SECTION("Getter is case insensitive") {
        mongocxx::uri uri = mongocxx::uri{"mongodb://host/?cOnNeCtTiMeOuTmS=123"};
        REQUIRE(uri.connect_timeout_ms());
        std::int32_t connect_timeout_ms = *uri.connect_timeout_ms();
        REQUIRE(123 == connect_timeout_ms);
    }
}

template <typename fn>
static void _test_string_option(std::string optname, fn getter_fn) {
    /* Not present. */
    mongocxx::uri uri{"mongodb://host/"};
    REQUIRE(!getter_fn(uri));

    /* Present. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=abc"};
    REQUIRE(getter_fn(uri));
    REQUIRE(0 == getter_fn(uri)->compare("abc"));

    /* Present but empty. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=&connectTimeoutMS=123"};
    REQUIRE(getter_fn(uri));
    REQUIRE(0 == getter_fn(uri)->compare(""));
}

template <typename fn>
static void _test_bool_option(std::string optname, fn getter_fn) {
    /* Not present. */
    mongocxx::uri uri{"mongodb://host/"};
    REQUIRE(!getter_fn(uri));

    /* Present. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=true"};
    REQUIRE(getter_fn(uri));
    REQUIRE(true == *getter_fn(uri));

    /* Present, but false. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=false"};
    REQUIRE(getter_fn(uri));
    REQUIRE(false == *getter_fn(uri));

    /* Present but empty. libmongoc considers it unset. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "="};
    REQUIRE(!getter_fn(uri));

    /* Present, but valid numeric. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=1"};
    REQUIRE(getter_fn(uri));
    REQUIRE(true == *getter_fn(uri));

    /* Present, but invalid numeric. */
    REQUIRE_THROWS(mongocxx::uri{"mongodb://host/?" + optname + "=2"});

    /* Present, but wrong type. */
    REQUIRE_THROWS(mongocxx::uri{"mongodb://host/?" + optname + "=lol"});
}

template <typename fn>
static void _test_int32_option(std::string optname,
                               fn getter_fn,
                               std::string valid_value = "1234",
                               bool zero_allowed = true) {
    /* Not present. */
    mongocxx::uri uri{"mongodb://host/"};
    REQUIRE(!getter_fn(uri));

    /* Present. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=" + valid_value};
    REQUIRE(getter_fn(uri));
    REQUIRE(stoi(valid_value) == *getter_fn(uri));

    if (zero_allowed) {
        uri = mongocxx::uri{"mongodb://host/?" + optname + "=0"};
        REQUIRE(getter_fn(uri));
        REQUIRE(0 == *getter_fn(uri));
    } else {
        REQUIRE_THROWS(mongocxx::uri{"mongodb://host/?" + optname + "=0"});
    }

    /* Present but empty. libmongoc considers it unset. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=&tls=true"};
    REQUIRE(!getter_fn(uri));

    /* Present, but wrong type. Error. */
    REQUIRE_THROWS(mongocxx::uri{"mongodb://host/?" + optname + "=abc"});
}

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

template <typename fn>
static void _test_document_option(std::string optname, fn getter_fn) {
    /* Not present. */
    mongocxx::uri uri{"mongodb://host/"};
    REQUIRE(!getter_fn(uri));

    /* Present. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=a:b"};
    REQUIRE(getter_fn(uri));
    REQUIRE(make_document(kvp("a", "b")) == *getter_fn(uri));

    /* Present, multiple fields. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=a:b,a2:b2"};
    REQUIRE(getter_fn(uri));
    REQUIRE(make_document(kvp("a", "b"), kvp("a2", "b2")) == *getter_fn(uri));

    /* Present but empty. libmongoc considers it unset. */
    uri = mongocxx::uri{"mongodb://host/?" + optname + "=&connectTimeoutMS=123"};
    REQUIRE(getter_fn(uri));
    REQUIRE(make_document() == *getter_fn(uri));
}

TEST_CASE("uri::appname()", "[uri]") {
    _test_string_option("appname", [](mongocxx::uri& uri) { return uri.appname(); });
}

TEST_CASE("uri::auth_mechanism_properties()", "[uri]") {
    _test_document_option("authMechanismProperties",
                          [](mongocxx::uri& uri) { return uri.auth_mechanism_properties(); });
}

TEST_CASE("uri::connect_timeout_ms()", "[uri]") {
    _test_int32_option("connectTimeoutMS",
                       [](mongocxx::uri& uri) { return uri.connect_timeout_ms(); });
}

TEST_CASE("uri::direct_connection()", "[uri]") {
    _test_bool_option("directConnection",
                      [](mongocxx::uri& uri) { return uri.direct_connection(); });
}

TEST_CASE("uri::local_threshold_ms()", "[uri]") {
    _test_int32_option("localThresholdMS",
                       [](mongocxx::uri& uri) { return uri.local_threshold_ms(); });
}

TEST_CASE("uri::max_pool_size()", "[uri]") {
    _test_int32_option("maxPoolSize", [](mongocxx::uri& uri) { return uri.max_pool_size(); });
}

TEST_CASE("uri::retry_reads()", "[uri]") {
    _test_bool_option("retryReads", [](mongocxx::uri& uri) { return uri.retry_reads(); });
}

TEST_CASE("uri::retry_writes()", "[uri]") {
    _test_bool_option("retryWrites", [](mongocxx::uri& uri) { return uri.retry_writes(); });
}

TEST_CASE("uri::server_selection_timeout_ms()", "[uri]") {
    _test_int32_option("serverSelectionTimeoutMS",
                       [](mongocxx::uri& uri) { return uri.server_selection_timeout_ms(); });
}

TEST_CASE("uri::server_selection_try_once()", "[uri]") {
    _test_bool_option("serverSelectionTryOnce",
                      [](mongocxx::uri& uri) { return uri.server_selection_try_once(); });
}

TEST_CASE("uri::socket_timeout_ms()", "[uri]") {
    _test_int32_option("socketTimeoutMS",
                       [](mongocxx::uri& uri) { return uri.socket_timeout_ms(); });
}

TEST_CASE("uri::tls_allow_invalid_certificates()", "[uri]") {
    _test_bool_option("tlsAllowInvalidCertificates",
                      [](mongocxx::uri& uri) { return uri.tls_allow_invalid_certificates(); });
}

TEST_CASE("uri::tls_allow_invalid_hostnames()", "[uri]") {
    _test_bool_option("tlsAllowInvalidHostnames",
                      [](mongocxx::uri& uri) { return uri.tls_allow_invalid_hostnames(); });
}

TEST_CASE("uri::tls_ca_file()", "[uri]") {
    _test_string_option("tlsCAFile", [](mongocxx::uri& uri) { return uri.tls_ca_file(); });
}

TEST_CASE("uri::tls_certificate_key_file()", "[uri]") {
    _test_string_option("tlsCertificateKeyFile",
                        [](mongocxx::uri& uri) { return uri.tls_certificate_key_file(); });
}

TEST_CASE("uri::tls_certificate_key_file_password()", "[uri]") {
    _test_string_option("tlsCertificateKeyFilePassword",
                        [](mongocxx::uri& uri) { return uri.tls_certificate_key_file_password(); });
}

TEST_CASE("uri::tls_disable_certificate_revocation_check()", "[uri]") {
    _test_bool_option("tlsDisableCertificateRevocationCheck", [](mongocxx::uri& uri) {
        return uri.tls_disable_certificate_revocation_check();
    });
}

TEST_CASE("uri::tls_disable_ocsp_endpoint_check()", "[uri]") {
    _test_bool_option("tlsDisableOCSPEndpointCheck",
                      [](mongocxx::uri& uri) { return uri.tls_disable_ocsp_endpoint_check(); });
}

TEST_CASE("uri::tls_insecure()", "[uri]") {
    _test_bool_option("tlsInsecure", [](mongocxx::uri& uri) { return uri.tls_insecure(); });
}

TEST_CASE("uri::wait_queue_timeout_ms()", "[uri]") {
    _test_int32_option("waitQueueTimeoutMS",
                       [](mongocxx::uri& uri) { return uri.wait_queue_timeout_ms(); });
}

static void compare_elements(bsoncxx::document::element expected_option,
                             bsoncxx::document::element my_option) {
    REQUIRE(expected_option.type() == my_option.type());
    switch (expected_option.type()) {
        case bsoncxx::type::k_int32:
            REQUIRE(expected_option.get_int32() == my_option.get_int32());
            break;
        case bsoncxx::type::k_bool:
            REQUIRE(expected_option.get_bool() == my_option.get_bool());
            break;
        case bsoncxx::type::k_string:
            REQUIRE(expected_option.get_string() == my_option.get_string());
            break;
        default:
            std::string msg =
                "option type not handled: " + bsoncxx::to_string(expected_option.type());
            throw std::logic_error(msg);
    }
}

static void compare_options(bsoncxx::document::view_or_value expected_options,
                            bsoncxx::document::view_or_value my_options,
                            bsoncxx::stdx::optional<bsoncxx::document::view> creds) {
    auto my_options_view = my_options.view();
    for (const auto& expected_option : expected_options.view()) {
        auto key = std::string(expected_option.key());
        std::transform(key.begin(), key.end(), key.begin(), [](int c) { return std::tolower(c); });
        if (key == "ssl") {
            key = std::string("tls");
        }

        bsoncxx::document::element my_value = my_options_view[key];
        if (my_value) {
            compare_elements(expected_option, my_value);
        } else if (creds && creds.value()[key]) {
            compare_elements(expected_option, creds.value()[key]);
        } else {
            REQUIRE(false);
        }
    }
}

static bool hosts_are_equal(bsoncxx::document::view expected_hosts,
                            std::vector<std::string> actual,
                            std::mutex& mtx) {
    const std::lock_guard<std::mutex> lock(mtx);

    std::vector<std::string> expected;

    for (const auto& i : expected_hosts) {
        expected.push_back(std::string(i.get_string().value));
    }

    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());

    return expected == actual;
}

static void validate_srv_max_hosts(mongocxx::client client,
                                   bsoncxx::document::view test_doc,
                                   std::mutex& mtx,
                                   std::vector<std::string>& new_hosts) {
    using namespace mongocxx;

    auto admin = client["admin"];
    auto result = admin.run_command(make_document(kvp("ping", 1)));
    REQUIRE(result.begin()->get_double() == 1.0);

    auto uri = client.uri();
    auto my_options = uri.options();
    auto creds = uri.credentials();
    auto expected_options = test_doc["options"];
    if (expected_options) {
        compare_options(expected_options.get_document().view(), my_options, creds);
    }

    auto expected_hosts = test_doc["hosts"];
    if (expected_hosts) {
        size_t count = 0;
        while (!hosts_are_equal(expected_hosts.get_array().value, new_hosts, mtx)) {
            count++;
            if (count > 4) {
                REQUIRE(hosts_are_equal(expected_hosts.get_array().value, new_hosts, mtx));
            }
            std::this_thread::sleep_for(std::chrono::seconds(count));
        }
    }
}

static void run_srv_max_hosts_test_file(bsoncxx::document::view test_doc) {
    using namespace mongocxx;

    bool expect_exception = test_doc["error"] && test_doc["error"].get_bool().value;
    bool should_ping = true;
    if (test_doc["ping"] && test_doc["ping"].get_bool() == false) {
        should_ping = false;
    }
    should_ping = should_ping && !expect_exception;

    options::apm apm_opts;
    std::mutex mtx;
    std::vector<std::string> new_hosts;

    apm_opts.on_topology_changed([&](const events::topology_changed_event& event) {
        const std::lock_guard<std::mutex> lock(mtx);
        auto new_td = event.new_description();
        auto servers = new_td.servers();
        new_hosts.clear();
        for (const auto& i : servers) {
            new_hosts.push_back(std::string(i.host()) + ":" + std::to_string(i.port()));
        }
    });

    mongocxx::options::tls tls_options;
    mongocxx::options::client client_options;

    auto ca_file_path = std::getenv("MONGOCXX_TEST_TLS_CA_FILE");
    REQUIRE(ca_file_path);
    tls_options.ca_file(ca_file_path);
    tls_options.allow_invalid_certificates(true);
    client_options.tls_opts(tls_options);
    client_options.apm_opts(apm_opts);

    mongocxx::uri my_uri;
    try {
        my_uri = uri{test_doc["uri"].get_string().value};
        class client client {
            my_uri, test_util::add_test_server_api(client_options)
        };
        bool using_tls = client.uri().tls();
        if (!using_tls) {
            return;
        }
        REQUIRE(!expect_exception);
        if (should_ping) {
            validate_srv_max_hosts(std::move(client), test_doc, mtx, new_hosts);
        }
    } catch (mongocxx::exception& e) {
        REQUIRE(expect_exception);
    }
}

static void iterate_srv_max_hosts_tests(std::string dir, std::vector<std::string> files) {
    for (const auto& file : files) {
        auto test_doc = _doc_from_file("/" + dir + "/" + file);
        run_srv_max_hosts_test_file(test_doc);
    }
}

static bool is_tls_enabled(void) {
    using namespace mongocxx;

    options::tls tls_options;
    options::client client_options;

    auto ca_file_path = std::getenv("MONGOCXX_TEST_TLS_CA_FILE");
    REQUIRE(ca_file_path);
    tls_options.ca_file(ca_file_path);
    tls_options.allow_invalid_certificates(true);
    client_options.tls_opts(tls_options);

    try {
        class mongocxx::client client {
            uri{"mongodb://localhost:27017/?tls=true"},
                test_util::add_test_server_api(client_options)
        };

        auto admin = client["admin"];
        auto result = admin.run_command(make_document(kvp("ping", 1)));
        REQUIRE(!result.empty());
        REQUIRE(result.begin()->get_double() == 1.0);
        return true;
    } catch (mongocxx::exception& e) {
        return false;
    }
}

TEST_CASE("uri::test_srv_max_hosts", "[uri]") {
    mongocxx::instance::current();

    if (!is_tls_enabled()) {
        std::cerr << "TLS is not supported by server, skipping test: 'uri::test_srv_max_hosts'"
                  << std::endl;
        return;
    }

    SECTION("replica-set") {
        std::vector<std::string> files = {"srvMaxHosts-less_than_srv_records.json",
                                          "srvMaxHosts-invalid_type.json",
                                          "srvMaxHosts-invalid_integer.json",
                                          "srvMaxHosts-greater_than_srv_records.json",
                                          "srvMaxHosts-equal_to_srv_records.json",
                                          "srvMaxHosts-conflicts_with_replicaSet.json",
                                          "srvMaxHosts-conflicts_with_replicaSet-txt.json",
                                          "srvMaxHosts-zero.json",
                                          "srvMaxHosts-zero-txt.json"};

        iterate_srv_max_hosts_tests("replica-set", files);
    }

    SECTION("load-balanced") {
        std::vector<std::string> files = {
            "srvMaxHosts-conflicts_with_loadBalanced-true-txt.json",
            "srvMaxHosts-conflicts_with_loadBalanced-true.json",
            //"srvMaxHosts-zero-txt.json", // Blocked until CXX-1848 is implemented
            //"srvMaxHosts-zero.json" // Blocked until CXX-1848 is implemented
        };
        iterate_srv_max_hosts_tests("load-balanced", files);
    }

    SECTION("sharded") {
        std::vector<std::string> files = {
            //"srvMaxHosts-zero.json", // Blocked until CXX-1848 is implemented
            "srvMaxHosts-less_than_srv_records.json",
            "srvMaxHosts-invalid_type.json",
            "srvMaxHosts-invalid_integer.json",
            //"srvMaxHosts-greater_than_srv_records.json", // Blocked until CXX-1848 is implemented
            //"srvMaxHosts-equal_to_srv_records.json" // Blocked until CXX-1848 is implemented
        };
        iterate_srv_max_hosts_tests("sharded", files);
    }
}

// Begin special cases.

// Compressors is a non-optional list of strings.
TEST_CASE("uri::compressors()", "[uri]") {
    std::string zlib = "zlib", noop = "noop";
    mongocxx::uri uri{"mongodb://host/?compressors=zlib,noop"};
    std::vector<bsoncxx::stdx::string_view> compressors = uri.compressors();
    REQUIRE(compressors.size() == 2);
    REQUIRE(std::find(compressors.begin(), compressors.end(), bsoncxx::stdx::string_view(zlib)) !=
            compressors.end());
    REQUIRE(std::find(compressors.begin(), compressors.end(), bsoncxx::stdx::string_view(noop)) !=
            compressors.end());

    /* Present with some invalid, only returns valid. */
    uri = mongocxx::uri{"mongodb://host/?compressors=zlib,invalid"};
    compressors = uri.compressors();
    REQUIRE(compressors.size() == 1);
    REQUIRE(std::find(compressors.begin(), compressors.end(), bsoncxx::stdx::string_view(zlib)) !=
            compressors.end());

    /* Not present, empty list. */
    uri = mongocxx::uri{"mongodb://host/"};
    compressors = uri.compressors();
    REQUIRE(compressors.size() == 0);

    /* Present but no valid, empty list. */
    uri = mongocxx::uri{"mongodb://host/?compressors=invalid,invalid"};
    compressors = uri.compressors();
    REQUIRE(compressors.size() == 0);

    /* Present but empty, empty list. */
    uri = mongocxx::uri{"mongodb://host/?compressors=&connectTimeousMS=123"};
    compressors = uri.compressors();
    REQUIRE(compressors.size() == 0);
}

// Zero is not allowed for heartbeatFrequencyMS.
TEST_CASE("uri::heartbeat_frequency_ms()", "[uri]") {
    _test_int32_option("heartbeatFrequencyMS",
                       [](mongocxx::uri& uri) { return uri.heartbeat_frequency_ms(); },
                       "1234",
                       false);
}

// -1 to 9 are only valid values of zlib compression level.
TEST_CASE("uri::zlib_compression_level()", "[uri]") {
    _test_int32_option("zlibCompressionLevel",
                       [](mongocxx::uri& uri) { return uri.zlib_compression_level(); },
                       "5",
                       true);
}

// End special cases.

}  // namespace
