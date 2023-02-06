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

#include <cctype>
#include <fstream>
#include <mutex>
#include <thread>

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/monitoring.hh>

namespace {

static bsoncxx::document::value _doc_from_file(mongocxx::stdx::string_view sub_path) {
    const char* test_path = std::getenv("INITIAL_DNS_SEEDLIST_DISCOVERY_TESTS_PATH");
    REQUIRE(test_path);

    std::string path = std::string(test_path) + sub_path.data();
    CAPTURE(path);

    std::ifstream file{path};
    REQUIRE(file);

    std::string file_contents((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    return bsoncxx::from_json(file_contents);
}

static void assert_elements_equal(bsoncxx::document::element expected_option,
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
            assert_elements_equal(expected_option, my_value);
        } else if (creds && creds.value()[key]) {
            assert_elements_equal(expected_option, creds.value()[key]);
        } else {
            FAIL("neither options nor credentials contains the required key: " + key);
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
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

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
    auto num_hosts = test_doc["numHosts"];
    if (num_hosts || expected_hosts) {
        size_t count = 0;
        while (!hosts_are_equal(expected_hosts.get_array().value, new_hosts, mtx)) {
            count++;
            if (count > 4) {
                REQUIRE(hosts_are_equal(expected_hosts.get_array().value, new_hosts, mtx));
            }
            std::this_thread::sleep_for(std::chrono::seconds(count));
        }
    }

    if (num_hosts) {
        REQUIRE((size_t)num_hosts.get_int32().value == new_hosts.size());
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

static void assert_tls_enabled(void) {
    using namespace mongocxx;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

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
    } catch (mongocxx::operation_exception& e) {
        FAIL("Unable to ping server with TLS. Is TLS enabled on the server?");
    }
}

// TODO(CXX-2087) extend this test to run all Initial DNS Seedlist Discovery tests.
TEST_CASE("uri::test_srv_max_hosts", "[uri]") {
    mongocxx::instance::current();

    if (!std::getenv("MONGOCXX_TEST_TLS_CA_FILE")) {
        std::cerr << "The environment variable 'MONGOCXX_TEST_TLS_CA_FILE' must be present to run "
                     "the test 'uri::test_srv_max_hosts'"
                  << std::endl;
        return;
    }

    assert_tls_enabled();

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
            //"srvMaxHosts-less_than_srv_records.json", // Blocked until CXX-1848 is implemented
            "srvMaxHosts-invalid_type.json",
            "srvMaxHosts-invalid_integer.json",
            //"srvMaxHosts-greater_than_srv_records.json", // Blocked until CXX-1848 is implemented
            //"srvMaxHosts-equal_to_srv_records.json" // Blocked until CXX-1848 is implemented
        };
        iterate_srv_max_hosts_tests("sharded", files);
    }
}

}  // namespace
