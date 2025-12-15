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

#include <mongocxx/v1/uri.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <bsoncxx/test/v1/document/view.hh>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

using code = uri::errc;

namespace {

template <typename MemFn, typename Mock>
void test_string(MemFn mem_fn, Mock& mock) {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto fn = mock.create_instance();

    SECTION("null") {
        fn->interpose([&](mongoc_uri_t const* ptr) -> char const* {
            CHECK(ptr == identity);
            return nullptr;
        });
        CHECK((opts.*mem_fn)().data() == nullptr);
    }

    SECTION("value") {
        char const str = '\0';

        fn->interpose([&](mongoc_uri_t const* ptr) -> char const* {
            CHECK(ptr == identity);
            return &str;
        });
        CHECK((opts.*mem_fn)().data() == static_cast<void const*>(&str));
    }
}

template <typename MemFn, typename Mock>
void test_string_opt(MemFn mem_fn, Mock& mock) {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto fn = mock.create_instance();

    SECTION("empty") {
        fn->interpose([&](mongoc_uri_t const* ptr) -> char const* {
            CHECK(ptr == identity);
            return nullptr;
        });

        CHECK_FALSE((opts.*mem_fn)().has_value());
    }

    SECTION("value") {
        char const str = '\0';

        fn->interpose([&](mongoc_uri_t const* ptr) -> char const* {
            CHECK(ptr == identity);
            return &str;
        });

        auto const opt = (opts.*mem_fn)();
        REQUIRE(opt.has_value());
        CHECK(opt->data() == static_cast<void const*>(&str));
    }
}

template <typename MemFn, typename Mock>
void test_bool(MemFn mem_fn, Mock& mock) {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto const v = GENERATE(false, true);

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_uri_t const* ptr) -> bool {
        CHECK(ptr == identity);
        return v;
    });
    CHECK((opts.*mem_fn)() == v);
}

template <typename MemFn, typename Mock>
void test_doc(MemFn mem_fn, Mock& mock) {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    scoped_bson const doc{R"({"x": 1})"};

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
        CHECK(ptr == identity);
        return doc.bson();
    });
    CHECK((opts.*mem_fn)().data() == doc.view().data());
}

template <typename MemFn, typename Mock>
void test_doc_opt(MemFn mem_fn, Mock& mock) {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto fn = mock.create_instance();

    SECTION("empty") {
        fn->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
            CHECK(ptr == identity);
            return nullptr;
        });

        CHECK_FALSE((opts.*mem_fn)().has_value());
    }

    SECTION("value") {
        scoped_bson const doc{R"({"x": 1})"};

        fn->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
            CHECK(ptr == identity);
            return doc.bson();
        });

        auto const opt = (opts.*mem_fn)();
        REQUIRE(opt.has_value());
        CHECK(opt->data() == doc.view().data());
    }
}

template <typename MemFn, typename T>
void test_credentials_option(MemFn mem_fn, bsoncxx::v1::document::view doc, T const& expected) {
    CAPTURE(doc);
    CAPTURE(expected);

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto credentials = libmongoc::uri_get_credentials.create_instance();
    credentials->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
        CHECK(ptr == identity);
        return scoped_bson_view{doc}.bson(); // Never null.
    });

    CHECK((opts.*mem_fn)() == expected);
}

template <typename MemFn, typename T>
void test_option(MemFn mem_fn, bsoncxx::v1::document::view doc, T const& expected) {
    CAPTURE(doc);
    CAPTURE(expected);

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto options = libmongoc::uri_get_options.create_instance();
    options->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
        CHECK(ptr == identity);
        return scoped_bson_view{doc}.bson(); // Never null.
    });

    CHECK((opts.*mem_fn)() == expected);
}

void test_option_string(
    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> (uri::*mem_fn)() const,
    char const* field) {
    auto const v = GENERATE(values({
        "x",
        "abc",
    }));

    scoped_bson const options{BCON_NEW(field, BCON_UTF8(v))};

    test_option(mem_fn, options.view(), v);
}

void test_option_int32(bsoncxx::v1::stdx::optional<std::int32_t> (uri::*mem_fn)() const, char const* field) {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    scoped_bson const options{BCON_NEW(field, BCON_INT32(v))};

    test_option(mem_fn, options.view(), v);
}

void test_option_bool(bsoncxx::v1::stdx::optional<bool> (uri::*mem_fn)() const, char const* field) {
    auto const v = GENERATE(false, true);

    scoped_bson const options{BCON_NEW(field, BCON_BOOL(v))};

    test_option(mem_fn, options.view(), v);
}

} // namespace

TEST_CASE("error code", "[mongocxx][v1][uri][error]") {
    using mongocxx::v1::source_errc;
    using mongocxx::v1::type_errc;

    auto const& category = mongocxx::v1::uri::error_category();
    CHECK_THAT(category.name(), Catch::Matchers::Equals("mongocxx::v1::uri"));

    auto const zero_errc = make_error_condition(static_cast<std::errc>(0));

    SECTION("unknown") {
        std::error_code const ec = static_cast<code>(-1);

        CHECK(ec.category() == category);
        CHECK(ec.value() == -1);
        CHECK(ec);
        CHECK(ec.message() == std::string(category.name()) + ":-1");
    }

    SECTION("zero") {
        std::error_code const ec = code::zero;

        CHECK(ec.category() == category);
        CHECK(ec.value() == 0);
        CHECK_FALSE(ec);
        CHECK(ec.message() == "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("non-zero") {
        std::error_code const ec = code::set_failure;

        CHECK(ec.category() == category);
        CHECK(ec.value() != static_cast<int>(code::zero));
        CHECK(ec);
        CHECK(ec.message() != "zero");

        CHECK(ec != zero_errc);
        CHECK(ec != source_errc::zero);
        CHECK(ec != type_errc::zero);
    }

    SECTION("source") {
        CHECK(make_error_code(code::set_failure) == source_errc::mongocxx);
    }

    SECTION("type") {
        CHECK(make_error_code(code::set_failure) == type_errc::invalid_argument);
    }
}

TEST_CASE("exceptions", "[mongocxx][v1][uri]") {
    SECTION("initialization") {
        try {
            (void)uri{"invalid"};
            FAIL("should not reach this point");
        } catch (v1::exception const& ex) {
            CHECK(ex.code() == v1::source_errc::mongoc);
            CHECK(ex.code().value() == MONGOC_ERROR_COMMAND_INVALID_ARG);
        }
    }

    SECTION("set_failure") {
        auto const v = GENERATE(false, true);

        uri opts;

        auto const ptr = uri::internal::as_mongoc(opts);

        auto set_option_as_bool = libmongoc::uri_set_option_as_bool.create_instance();

        set_option_as_bool->interpose([&](mongoc_uri_t* uri, char const* option_orig, bool value) -> bool {
            CHECK(uri == ptr);
            CHECK_THAT(option_orig, Catch::Matchers::Equals(MONGOC_URI_SERVERSELECTIONTRYONCE));
            CHECK(value == v);

            return false;
        });

        try {
            opts.server_selection_try_once(v);
            FAIL("should not reach this point");
        } catch (v1::exception const& ex) {
            CHECK(ex.code() == code::set_failure);
            CHECK_THAT(
                ex.what(),
                Catch::Matchers::ContainsSubstring("could not set the requested URI option") &&
                    Catch::Matchers::ContainsSubstring(MONGOC_URI_SERVERSELECTIONTRYONCE));
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][uri]") {
    uri source{"mongodb://localhost:27017/source"};
    uri target{"mongodb://localhost:27017/target"};

    REQUIRE(source.database() == "source");
    REQUIRE(target.database() == "target");

    auto const source_value = source.database();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.database() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.database() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.database() == source_value);
        CHECK(copy.database() == source_value);

        target = copy;

        CHECK(copy.database() == source_value);
        CHECK(target.database() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][uri]") {
    uri const opts;

    CHECK(opts.auth_mechanism().empty());
    CHECK(opts.auth_source() == "admin");
    CHECK(opts.hosts().size() == 1u); // "localhost:27017"
    CHECK(opts.database().empty());
    CHECK(opts.options().empty());
    CHECK(opts.password().empty());
    CHECK(opts.read_concern() == v1::read_concern{});
    CHECK(opts.read_preference() == v1::read_preference{});
    CHECK(opts.replica_set().empty());
    CHECK_FALSE(opts.tls());
    CHECK(opts.to_string() == uri::k_default_uri);
    CHECK(opts.username().empty());
    CHECK(opts.write_concern() == v1::write_concern{});
    CHECK_FALSE(opts.appname().has_value());
    CHECK_FALSE(opts.auth_mechanism_properties().has_value());
    CHECK_FALSE(opts.credentials() == scoped_bson{R"({"authMechanism": {}})"}.view()); // Never empty.
    CHECK_FALSE(opts.srv_max_hosts().has_value());
    CHECK(opts.compressors().empty());
    CHECK_FALSE(opts.connect_timeout_ms().has_value());
    CHECK_FALSE(opts.direct_connection().has_value());
    CHECK_FALSE(opts.heartbeat_frequency_ms().has_value());
    CHECK_FALSE(opts.local_threshold_ms().has_value());
    CHECK_FALSE(opts.max_pool_size().has_value());
    CHECK_FALSE(opts.retry_reads().has_value());
    CHECK_FALSE(opts.retry_writes().has_value());
    CHECK_FALSE(opts.server_selection_timeout_ms().has_value());
    CHECK_FALSE(opts.server_selection_try_once().has_value());
    CHECK_FALSE(opts.server_selection_try_once().has_value());
    CHECK_FALSE(opts.socket_timeout_ms().has_value());
    CHECK_FALSE(opts.tls_allow_invalid_certificates().has_value());
    CHECK_FALSE(opts.tls_allow_invalid_hostnames().has_value());
    CHECK_FALSE(opts.tls_ca_file().has_value());
    CHECK_FALSE(opts.tls_certificate_key_file().has_value());
    CHECK_FALSE(opts.tls_certificate_key_file_password().has_value());
    CHECK_FALSE(opts.tls_disable_certificate_revocation_check().has_value());
    CHECK_FALSE(opts.tls_disable_ocsp_endpoint_check().has_value());
    CHECK_FALSE(opts.tls_insecure().has_value());
    CHECK_FALSE(opts.wait_queue_timeout_ms().has_value());
    CHECK_FALSE(opts.zlib_compression_level().has_value());
}

TEST_CASE("auth_mechanism", "[mongocxx][v1][uri]") {
    test_string(&uri::auth_mechanism, libmongoc::uri_get_auth_mechanism);
}

TEST_CASE("auth_source", "[mongocxx][v1][uri]") {
    test_string(&uri::auth_source, libmongoc::uri_get_auth_source);
}

TEST_CASE("hosts", "[mongocxx][v1][uri]") {
    std::array<mongoc_host_list_t, 3> v = {{
        {
            nullptr,
            "mongodb://localhost",
            "mongodb://localhost:27017",
            27017u,
            {}, // family
            {}, // padding
        },
        {
            nullptr,
            "mongodb://localhost",
            "mongodb://localhost:27018",
            27018u,
            {}, // family
            {}, // padding
        },
        {
            nullptr,
            "mongodb://localhost",
            "mongodb://localhost:27019",
            27019u,
            {}, // family
            {}, // padding
        },
    }};

    auto const len = GENERATE(as<std::size_t>{}, 0u, 1u, 2u, 3u);
    REQUIRE(len <= v.size());

    if (len > 1) {
        v[0].next = &v[1];
    }

    if (len > 2) {
        v[1].next = &v[2];
    }

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto hosts = libmongoc::uri_get_hosts.create_instance();
    hosts->interpose([&](mongoc_uri_t const* ptr) -> mongoc_host_list_t const* {
        CHECK(ptr == identity);
        return len == 0 ? nullptr : v.data();
    });

    auto const list = opts.hosts();
    REQUIRE(list.size() == len);
    for (std::size_t i = 0u; i < len; ++i) {
        auto const& actual = list[i];
        auto const& expected = v[i];

        CHECK(actual.port == expected.port);
    }
}

TEST_CASE("database", "[mongocxx][v1][uri]") {
    test_string(&uri::database, libmongoc::uri_get_database);
}

TEST_CASE("options", "[mongocxx][v1][uri]") {
    test_doc(&uri::options, libmongoc::uri_get_options);
}

TEST_CASE("password", "[mongocxx][v1][uri]") {
    test_string(&uri::password, libmongoc::uri_get_password);
}

TEST_CASE("read_concern", "[mongocxx][v1][uri]") {
    using T = v1::read_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
    }));

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto read_concern = libmongoc::uri_get_read_concern.create_instance();
    read_concern->interpose([&](mongoc_uri_t const* ptr) -> mongoc_read_concern_t const* {
        CHECK(ptr == identity);
        return v1::read_concern::internal::as_mongoc(v);
    });

    CHECK(opts.read_concern() == v);
}

TEST_CASE("read_preference", "[mongocxx][v1][uri]") {
    using T = mongocxx::v1::read_preference;

    auto const v = GENERATE(values({
        T{},
        T{}.mode(T::read_mode::k_secondary),
        T{}.tags(scoped_bson{R"([1, 2.0, "3"])"}.array_view()),
    }));

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto read_preference = libmongoc::uri_get_read_prefs_t.create_instance();
    read_preference->interpose([&](mongoc_uri_t const* ptr) -> mongoc_read_prefs_t const* {
        CHECK(ptr == identity);
        return v1::read_preference::internal::as_mongoc(v);
    });

    CHECK(opts.read_preference() == v);
}

TEST_CASE("replica_set", "[mongocxx][v1][uri]") {
    test_string(&uri::replica_set, libmongoc::uri_get_replica_set);
}

TEST_CASE("tls", "[mongocxx][v1][uri]") {
    test_bool(&uri::tls, libmongoc::uri_get_tls);
}

TEST_CASE("to_string", "[mongocxx][v1][uri]") {
    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    char const str = '\0';

    auto fn = libmongoc::uri_get_string.create_instance();
    fn->interpose([&](mongoc_uri_t const* ptr) -> char const* {
        CHECK(ptr == identity);
        return &str; // Never null.
    });
    CHECK(opts.to_string().data() == static_cast<void const*>(&str));
}

TEST_CASE("username", "[mongocxx][v1][uri]") {
    test_string(&uri::username, libmongoc::uri_get_username);
}

TEST_CASE("write_concern", "[mongocxx][v1][uri]") {
    using T = mongocxx::v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto write_concern = libmongoc::uri_get_write_concern.create_instance();
    write_concern->interpose([&](mongoc_uri_t const* ptr) -> mongoc_write_concern_t const* {
        CHECK(ptr == identity);
        return v1::write_concern::internal::as_mongoc(v);
    });

    CHECK(opts.write_concern() == v);
}

TEST_CASE("appname", "[mongocxx][v1][uri]") {
    test_string_opt(&uri::appname, libmongoc::uri_get_appname);
}

TEST_CASE("auth_mechanism_properties", "[mongocxx][v1][uri]") {
    auto const v = GENERATE(R"({"x": 1})", R"({"y": 2})");

    scoped_bson const creds{BCON_NEW(MONGOC_URI_AUTHMECHANISMPROPERTIES, BCON_DOCUMENT(scoped_bson{v}.bson()))};
    scoped_bson const expected{v};

    test_credentials_option(&uri::auth_mechanism_properties, creds.view(), expected.view());
}

TEST_CASE("credentials", "[mongocxx][v1][uri]") {
    test_doc_opt(&uri::credentials, libmongoc::uri_get_credentials);
}

TEST_CASE("srv_max_hosts", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::srv_max_hosts, MONGOC_URI_SRVMAXHOSTS);
}

TEST_CASE("compressors", "[mongocxx][v1][uri]") {
    scoped_bson v;
    std::size_t len = {};

    std::tie(v, len) = GENERATE(
        table<scoped_bson, std::size_t>({
            {scoped_bson{}, 0u},
            {scoped_bson{R"({"x": 1})"}, 1u},
            {scoped_bson{R"({"a": 1, "b": 2.0, "c": "three"})"}, 3u},
        }));

    uri const opts;
    auto const identity = uri::internal::as_mongoc(opts);

    auto compressors = libmongoc::uri_get_compressors.create_instance();
    compressors->interpose([&](mongoc_uri_t const* ptr) -> bson_t const* {
        CHECK(ptr == identity);
        return len == 0u ? nullptr : v.bson();
    });

    auto const list = opts.compressors();
    REQUIRE(list.size() == len);

    auto iter = v.view().begin();
    for (auto const e : list) {
        CHECK(static_cast<void const*>(e.data()) == static_cast<void const*>(iter->key().data()));
        ++iter;
    }
}

TEST_CASE("connect_timeout_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::connect_timeout_ms, MONGOC_URI_CONNECTTIMEOUTMS);
}

TEST_CASE("direct_connection", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::direct_connection, MONGOC_URI_DIRECTCONNECTION);
}

TEST_CASE("heartbeat_frequency_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::heartbeat_frequency_ms, MONGOC_URI_HEARTBEATFREQUENCYMS);
}

TEST_CASE("local_threshold_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::local_threshold_ms, MONGOC_URI_LOCALTHRESHOLDMS);
}

TEST_CASE("max_pool_size", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::max_pool_size, MONGOC_URI_MAXPOOLSIZE);
}

TEST_CASE("retry_reads", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::retry_reads, MONGOC_URI_RETRYREADS);
}

TEST_CASE("retry_writes", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::retry_writes, MONGOC_URI_RETRYWRITES);
}

TEST_CASE("server_selection_timeout_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::server_selection_timeout_ms, MONGOC_URI_SERVERSELECTIONTIMEOUTMS);
}

TEST_CASE("server_selection_try_once", "[mongocxx][v1][uri]") {
    SECTION("get") {
        test_option_bool(&uri::server_selection_try_once, MONGOC_URI_SERVERSELECTIONTRYONCE);
    }

    SECTION("set") {
        auto const v = GENERATE(false, true);

        CHECK(uri{}.server_selection_try_once(v).server_selection_try_once() == v);
    }
}

TEST_CASE("socket_timeout_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::socket_timeout_ms, MONGOC_URI_SOCKETTIMEOUTMS);
}

TEST_CASE("tls_allow_invalid_certificates", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::tls_allow_invalid_certificates, MONGOC_URI_TLSALLOWINVALIDCERTIFICATES);
}

TEST_CASE("tls_allow_invalid_hostnames", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::tls_allow_invalid_hostnames, MONGOC_URI_TLSALLOWINVALIDHOSTNAMES);
}

TEST_CASE("tls_ca_file", "[mongocxx][v1][uri]") {
    test_option_string(&uri::tls_ca_file, MONGOC_URI_TLSCAFILE);
}

TEST_CASE("tls_certificate_key_file", "[mongocxx][v1][uri]") {
    test_option_string(&uri::tls_certificate_key_file, MONGOC_URI_TLSCERTIFICATEKEYFILE);
}

TEST_CASE("tls_certificate_key_file_password", "[mongocxx][v1][uri]") {
    test_option_string(&uri::tls_certificate_key_file_password, MONGOC_URI_TLSCERTIFICATEKEYFILEPASSWORD);
}

TEST_CASE("tls_disable_certificate_revocation_check", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::tls_disable_certificate_revocation_check, MONGOC_URI_TLSDISABLECERTIFICATEREVOCATIONCHECK);
}

TEST_CASE("tls_disable_ocsp_endpoint_check", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::tls_disable_ocsp_endpoint_check, MONGOC_URI_TLSDISABLEOCSPENDPOINTCHECK);
}

TEST_CASE("tls_insecure", "[mongocxx][v1][uri]") {
    test_option_bool(&uri::tls_insecure, MONGOC_URI_TLSINSECURE);
}

TEST_CASE("wait_queue_timeout_ms", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::wait_queue_timeout_ms, MONGOC_URI_WAITQUEUETIMEOUTMS);
}

TEST_CASE("zlib_compression_level", "[mongocxx][v1][uri]") {
    test_option_int32(&uri::zlib_compression_level, MONGOC_URI_ZLIBCOMPRESSIONLEVEL);
}

} // namespace v1
} // namespace mongocxx
