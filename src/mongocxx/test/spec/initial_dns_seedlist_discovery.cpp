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

#include <cctype>
#include <fstream>
#include <mutex>
#include <thread>

#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/pool.hpp>
#include <mongocxx/pool.hpp>

#include <bsoncxx/test/catch.hh>

#include <mongocxx/test/spec/monitoring.hh>

namespace {

struct initial_dns_seedlist_test {
    bsoncxx::stdx::string_view uri;
    bsoncxx::array::view hosts;
    bsoncxx::document::view options;
    bool error = false;
    bool ping = true;

    static initial_dns_seedlist_test parse(bsoncxx::document::view test_doc) {
        initial_dns_seedlist_test test;

        for (auto el : test_doc) {
            auto const key = el.key();
            if (key == "uri") {
                test.uri = el.get_string().value;
            } else if (key == "seeds" || key == "numSeeds") {
                // The 'seeds' and 'numSeeds' assertions are explicitly skipped. The C++ driver does
                // not have access to the initial seedlist populated in the C driver.
            } else if (key == "hosts") {
                test.hosts = el.get_array().value;
            } else if (key == "options") {
                test.options = el.get_document().value;
            } else if (key == "error") {
                test.error = el.get_bool().value;
            } else if (key == "comment") {
                // Ignore comment.
            } else if (key == "ping") {
                test.ping = el.get_bool().value;
            } else {
                FAIL("unexpected initial_dns_seedlist_test option: " << el.key());
            }
        }
        return test;
    }
};

static bsoncxx::document::value _doc_from_file(bsoncxx::stdx::string_view sub_path) {
    char const* test_path = std::getenv("INITIAL_DNS_SEEDLIST_DISCOVERY_TESTS_PATH");
    REQUIRE(test_path);

    std::string path = std::string(test_path) + sub_path.data();
    CAPTURE(path);

    std::ifstream file{path};
    REQUIRE(file);

    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return bsoncxx::from_json(file_contents);
}

static void assert_elements_equal(bsoncxx::document::element expected_option, bsoncxx::document::element my_option) {
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

        case bsoncxx::type::k_double:
        case bsoncxx::type::k_document:
        case bsoncxx::type::k_array:
        case bsoncxx::type::k_binary:
        case bsoncxx::type::k_undefined:
        case bsoncxx::type::k_oid:
        case bsoncxx::type::k_date:
        case bsoncxx::type::k_null:
        case bsoncxx::type::k_regex:
        case bsoncxx::type::k_dbpointer:
        case bsoncxx::type::k_code:
        case bsoncxx::type::k_symbol:
        case bsoncxx::type::k_codewscope:
        case bsoncxx::type::k_timestamp:
        case bsoncxx::type::k_int64:
        case bsoncxx::type::k_decimal128:
        case bsoncxx::type::k_maxkey:
        case bsoncxx::type::k_minkey:
        default:
            std::string msg = "option type not handled: " + bsoncxx::to_string(expected_option.type());
            throw std::logic_error(msg);
    }
}

static void compare_options(
    bsoncxx::document::view_or_value expected_options,
    bsoncxx::document::view_or_value my_options,
    bsoncxx::stdx::optional<bsoncxx::document::view> creds) {
    auto my_options_view = my_options.view();
    for (auto const& expected_option : expected_options.view()) {
        auto key = mongocxx::test_util::tolowercase(expected_option.key());
        if (key == "ssl") {
            key = "tls";
        }

        bsoncxx::document::element my_value = my_options_view[key];
        if (my_value) {
            assert_elements_equal(expected_option, my_value);
        } else if (creds && creds.value()[key]) {
            assert_elements_equal(expected_option, creds.value()[key]);
        } else {
            FAIL("neither options nor credentials contains the required key: " << key);
        }
    }
}

static bool hosts_are_equal(bsoncxx::array::view expected_hosts, std::vector<std::string> actual, std::mutex& mtx) {
    std::lock_guard<std::mutex> const lock(mtx);

    std::vector<std::string> expected;

    for (auto const& i : expected_hosts) {
        expected.push_back(std::string(i.get_string().value));
    }

    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());

    return expected == actual;
}

static void validate_srv_max_hosts(
    mongocxx::client& client,
    initial_dns_seedlist_test const& test,
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
    compare_options(test.options, my_options, creds);

    if (!test.hosts.empty()) {
        size_t count = 0;
        while (!hosts_are_equal(test.hosts, new_hosts, mtx)) {
            count++;
            if (count > 4) {
                REQUIRE(hosts_are_equal(test.hosts, new_hosts, mtx));
            }
            std::this_thread::sleep_for(std::chrono::seconds(count));
        }
    }
}

static void run_srv_max_hosts_test_file(initial_dns_seedlist_test const& test) {
    using namespace mongocxx;

    bool const should_ping = test.ping && !test.error;

    options::apm apm_opts;
    std::mutex mtx;
    std::vector<std::string> new_hosts;

    apm_opts.on_topology_changed([&](events::topology_changed_event const& event) {
        std::lock_guard<std::mutex> const lock(mtx);
        auto new_td = event.new_description();
        auto servers = new_td.servers();
        new_hosts.clear();
        for (auto const& i : servers) {
            new_hosts.push_back(std::string(i.host()) + ":" + std::to_string(i.port()));
        }
    });

    mongocxx::options::tls tls_options;
    mongocxx::options::client client_options;

    auto const ca_file_path = test_util::getenv_or_fail("MONGOCXX_TEST_TLS_CA_FILE");
    tls_options.ca_file(ca_file_path);
    tls_options.allow_invalid_certificates(true);
    client_options.tls_opts(tls_options);
    client_options.apm_opts(apm_opts);

    try {
        mongocxx::uri my_uri{test.uri};
        mongocxx::client client{my_uri, test_util::add_test_server_api(client_options)};
        bool using_tls = client.uri().tls();
        if (!using_tls) {
            return;
        }
        REQUIRE(!test.error);
        if (should_ping) {
            validate_srv_max_hosts(client, test, mtx, new_hosts);
            mongocxx::pool pool{my_uri, options::pool(test_util::add_test_server_api(client_options))};
            auto pool_client = pool.acquire();
            validate_srv_max_hosts(*pool_client, test, mtx, new_hosts);
        }
    } catch (mongocxx::exception& e) {
        if (!test.error) {
            FAIL("expected success, but got: " << e.what());
        }
    }
}

static void iterate_srv_max_hosts_tests(std::string dir, std::vector<std::string> files) {
    for (auto const& file : files) {
        auto test_doc = _doc_from_file("/" + dir + "/" + file);
        auto test = initial_dns_seedlist_test::parse(test_doc);
        DYNAMIC_SECTION(file) {
            run_srv_max_hosts_test_file(test);
        }
    }
}

static void assert_tls_enabled(void) {
    using namespace mongocxx;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    options::tls tls_options;
    options::client client_options;

    auto ca_file_path = test_util::getenv_or_fail("MONGOCXX_TEST_TLS_CA_FILE");
    tls_options.ca_file(ca_file_path);
    tls_options.allow_invalid_certificates(true);
    client_options.tls_opts(tls_options);

    try {
        mongocxx::client client{
            uri{"mongodb://localhost:27017/?tls=true"}, test_util::add_test_server_api(client_options)};

        auto admin = client["admin"];
        auto result = admin.run_command(make_document(kvp("ping", 1)));
        REQUIRE(!result.empty());
        REQUIRE(result.begin()->get_double() == 1.0);
    } catch (mongocxx::operation_exception& e) {
        FAIL("Unable to ping server with TLS: " << e.what());
    }
}

// TODO(CXX-2087) extend this test to run all Initial DNS Seedlist Discovery tests.
TEST_CASE("uri::test_srv_max_hosts", "[uri]") {
    mongocxx::instance::current();

    if (!std::getenv("MONGOCXX_TEST_DNS")) {
        SKIP("initial DNS seedlist discovery tests require MONGOCXX_TEST_DNS to be set");
    }

    assert_tls_enabled();

    SECTION("replica-set") {
        std::vector<std::string> files = {
            "srvMaxHosts-less_than_srv_records.json",
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

} // namespace
