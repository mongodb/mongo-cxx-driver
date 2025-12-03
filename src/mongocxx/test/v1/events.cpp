//
// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/events/command_failed.hh>
#include <mongocxx/v1/events/command_started.hh>
#include <mongocxx/v1/events/command_succeeded.hh>
#include <mongocxx/v1/events/server_closed.hh>
#include <mongocxx/v1/events/server_description_changed.hh>
#include <mongocxx/v1/events/server_heartbeat_failed.hh>
#include <mongocxx/v1/events/server_heartbeat_started.hh>
#include <mongocxx/v1/events/server_heartbeat_succeeded.hh>
#include <mongocxx/v1/events/server_opening.hh>
#include <mongocxx/v1/events/topology_closed.hh>
#include <mongocxx/v1/events/topology_description_changed.hh>
#include <mongocxx/v1/events/topology_opening.hh>

//

#include <bsoncxx/v1/detail/type_traits.hpp>

#include <mongocxx/v1/events/topology_description.hpp>

#include <mongocxx/v1/events/server_description.hh>
#include <mongocxx/v1/read_preference.hh>

#include <bsoncxx/test/v1/oid.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>

namespace mongocxx {
namespace v1 {
namespace events {

namespace {

template <typename Object>
struct to_mongoc {};

// clang-format off
template <> struct to_mongoc<command_failed> { using type = mongoc_apm_command_failed_t; };
template <> struct to_mongoc<command_started> { using type = mongoc_apm_command_started_t; };
template <> struct to_mongoc<command_succeeded> { using type = mongoc_apm_command_succeeded_t; };
template <> struct to_mongoc<server_closed> { using type = mongoc_apm_server_closed_t; };
template <> struct to_mongoc<server_description_changed> { using type = mongoc_apm_server_changed_t; };
template <> struct to_mongoc<server_heartbeat_failed> { using type = mongoc_apm_server_heartbeat_failed_t; };
template <> struct to_mongoc<server_heartbeat_started> { using type = mongoc_apm_server_heartbeat_started_t; };
template <> struct to_mongoc<server_heartbeat_succeeded> { using type = mongoc_apm_server_heartbeat_succeeded_t; };
template <> struct to_mongoc<server_opening> { using type = mongoc_apm_server_opening_t; };
template <> struct to_mongoc<topology_closed> { using type = mongoc_apm_topology_closed_t; };
template <> struct to_mongoc<topology_description_changed> { using type = mongoc_apm_topology_changed_t; };
template <> struct to_mongoc<topology_opening> { using type = mongoc_apm_topology_opening_t; };

template <> struct to_mongoc<server_description> { using type = mongoc_server_description_t; };
template <> struct to_mongoc<topology_description> { using type = mongoc_topology_description_t; };
// clang-format on

struct identity_type {};

template <typename MemFn, typename Object, typename Mock>
void test_bson(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    scoped_bson doc;

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> bson_t const* {
        CHECK(static_cast<void const*>(ptr) == identity);
        return doc.bson();
    });
    CHECK((object.*mem_fn)().data() == doc.view().data());
}

template <typename MemFn, typename Object, typename Mock>
void test_bool(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    auto const v = GENERATE(false, true);

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> bool {
        CHECK(static_cast<void const*>(ptr) == identity);
        return v;
    });
    CHECK((object.*mem_fn)() == v);
}

template <typename MemFn, typename Object, typename Mock>
void test_string(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    char const str = '\0';

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> char const* {
        CHECK(static_cast<void const*>(ptr) == identity);
        return &str;
    });
    CHECK((object.*mem_fn)() == &str);
}

template <typename MemFn, typename Object, typename Mock>
void test_uint32(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    auto const v = GENERATE(values({
        std::uint32_t{0},
        std::uint32_t{1},
        std::uint32_t{UINT32_MAX},
    }));

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> std::uint32_t {
        CHECK(static_cast<void const*>(ptr) == identity);
        return v;
    });
    CHECK((object.*mem_fn)() == v);
}

template <typename MemFn, typename Object, typename Mock>
void test_int64(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    auto const v = GENERATE(values({
        std::int64_t{INT64_MIN},
        std::int64_t{-1},
        std::int64_t{0},
        std::int64_t{1},
        std::int64_t{INT64_MAX},
    }));

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> std::int64_t {
        CHECK(static_cast<void const*>(ptr) == identity);
        return v;
    });
    CHECK((object.*mem_fn)() == v);
}

template <
    typename MemFn,
    typename Object,
    typename Mock,
    bool = std::is_same<
        typename Mock::underlying_ptr,
        void (*)(typename to_mongoc<Object>::type const*, bson_oid_t*)>::value>
struct test_oid_impl;

template <typename MemFn, typename Object, typename Mock>
struct test_oid_impl<MemFn, Object, Mock, false> {
    static void fn(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
        using mongoc_type = typename to_mongoc<Object>::type;

        bsoncxx::v1::oid const oid{"386e94ff1112131415212223"};

        bson_oid_t bson_oid = {};
        std::memcpy(&bson_oid, oid.bytes(), oid.size());

        auto fn = mock.create_instance();
        fn->interpose([&](mongoc_type const* ptr) -> bson_oid_t const* {
            CHECK(static_cast<void const*>(ptr) == identity);
            return &bson_oid;
        });
        CHECK((object.*mem_fn)() == oid);
    }
};

template <typename MemFn, typename Object, typename Mock>
struct test_oid_impl<MemFn, Object, Mock, true> {
    static void fn(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
        using mongoc_type = typename to_mongoc<Object>::type;

        bsoncxx::v1::oid const oid{"386e94ff1112131415212223"};

        auto fn = mock.create_instance();
        fn->interpose([&](mongoc_type const* ptr, bson_oid_t* out) -> void {
            CHECK(static_cast<void const*>(ptr) == identity);

            REQUIRE(out);
            std::memcpy(out, oid.bytes(), oid.size());
        });
        CHECK((object.*mem_fn)() == oid);
    }
};

template <typename MemFn, typename Object, typename Mock>
void test_oid(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    test_oid_impl<MemFn, Object, Mock>::fn(mem_fn, mock, object, identity);
}

template <typename Object, typename Mock>
void test_database_name(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("database_name") {
        test_string(&Object::database_name, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_command_name(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("command_name") {
        test_string(&Object::command_name, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_reply(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("reply") {
        test_bson(&Object::reply, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_duration(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("duration") {
        test_int64(&Object::duration, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_request_id(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("request_id") {
        test_int64(&Object::request_id, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_operation_id(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("operation_id") {
        test_int64(&Object::operation_id, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_service_id(Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    SECTION("service_id") {
        SECTION("null") {
            auto fn = mock.create_instance();
            fn->interpose([&](mongoc_type const* ptr) -> bson_oid_t const* {
                CHECK(static_cast<void const*>(ptr) == identity);
                return nullptr;
            });
            CHECK_FALSE(object.service_id().has_value());
        }

        SECTION("value") {
            test_oid(&Object::service_id, mock, object, identity);
        }
    }
}

template <typename Object, typename Mock>
void test_host_and_port(Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    SECTION("host") {
        mongoc_host_list_t host_list = {};

        auto get_host = mock.create_instance();
        get_host->interpose([&](mongoc_type const* ptr) -> mongoc_host_list_t const* {
            CHECK(static_cast<void const*>(ptr) == identity);
            return &host_list;
        });
        CHECK(object.host().data() == host_list.host);
    }

    SECTION("port") {
        mongoc_host_list_t host_list = {};
        host_list.port = GENERATE(values({
            std::uint16_t{0},
            std::uint16_t{1},
            std::uint16_t{UINT16_MAX - 1u},
            std::uint16_t{UINT16_MAX},
        }));

        auto get_host = mock.create_instance();
        get_host->interpose([&](mongoc_type const* ptr) -> mongoc_host_list_t const* {
            CHECK(static_cast<void const*>(ptr) == identity);
            return &host_list;
        });
        CHECK(object.port() == host_list.port);
    }
}

template <typename Object, typename Mock>
void test_topology_id(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("topology_id") {
        test_oid(&Object::topology_id, mock, object, identity);
    }
}

template <typename Object, typename Mock>
void test_awaited(Mock& mock, Object const& object, identity_type const* identity) {
    SECTION("awaited") {
        test_bool(&Object::awaited, mock, object, identity);
    }
}

template <typename MemFn, typename Object, typename Mock>
void test_server_description(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    identity_type const sd1;
    identity_type sd2;

    auto destroy = libmongoc::server_description_destroy.create_instance();
    auto new_copy = libmongoc::server_description_new_copy.create_instance();

    destroy
        ->interpose([&](mongoc_server_description_t* sd) -> void {
            auto const ptr = static_cast<void const*>(sd);
            if (ptr && ptr != &sd1 && ptr != &sd2) {
                FAIL("unexpected mongoc_server_description_t");
            }
        })
        .forever();

    new_copy
        ->interpose([&](mongoc_server_description_t const* ptr) -> mongoc_server_description_t* {
            CHECK(static_cast<void const*>(ptr) == &sd1);
            return reinterpret_cast<mongoc_server_description_t*>(&sd2);
        })
        .forever();

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> mongoc_server_description_t const* {
        CHECK(static_cast<void const*>(ptr) == identity);
        return reinterpret_cast<mongoc_server_description_t const*>(&sd1);
    });

    server_description const sd = (object.*mem_fn)();

    SECTION("id") {
        test_uint32(&server_description::id, libmongoc::server_description_id, sd, &sd2);
    }

    SECTION("round_trip_time") {
        test_int64(&server_description::round_trip_time, libmongoc::server_description_round_trip_time, sd, &sd2);
    }

    SECTION("type") {
        test_string(&server_description::type, libmongoc::server_description_type, sd, &sd2);
    }

    SECTION("hello") {
        test_bson(&server_description::hello, libmongoc::server_description_hello_response, sd, &sd2);
    }

    test_host_and_port(libmongoc::server_description_host, sd, &sd2);
}

template <typename MemFn, typename Object, typename Mock>
void test_topology_description(MemFn mem_fn, Mock& mock, Object const& object, identity_type const* identity) {
    using mongoc_type = typename to_mongoc<Object>::type;

    identity_type const tdid;

    auto fn = mock.create_instance();
    fn->interpose([&](mongoc_type const* ptr) -> mongoc_topology_description_t const* {
        CHECK(static_cast<void const*>(ptr) == identity);
        return reinterpret_cast<mongoc_topology_description_t const*>(&tdid);
    });

    topology_description const td = (object.*mem_fn)();

    SECTION("type") {
        auto type = libmongoc::topology_description_type.create_instance();

        char const str = '\0';

        type->interpose([&](mongoc_topology_description_t const* ptr) -> char const* {
            CHECK(static_cast<void const*>(ptr) == &tdid);
            return &str;
        });

        CHECK(td.type() == &str);
    }

    SECTION("has_readable_server") {
        auto has_readable_server = libmongoc::topology_description_has_readable_server.create_instance();

        v1::read_preference rp;

        auto const v = GENERATE(false, true);

        has_readable_server->interpose(
            [&](mongoc_topology_description_t const* ptr, mongoc_read_prefs_t const* prefs) -> bool {
                CHECK(static_cast<void const*>(ptr) == &tdid);
                CHECK(prefs == v1::read_preference::internal::as_mongoc(rp));
                return v;
            });

        CHECK(td.has_readable_server(rp) == v);
    }

    SECTION("has_writable_server") {
        test_bool(
            &topology_description::has_writable_server, libmongoc::topology_description_has_writable_server, td, &tdid);
    }

    SECTION("servers") {
        auto get_servers = libmongoc::topology_description_get_servers.create_instance();

        auto destroy = libmongoc::server_description_destroy.create_instance();
        auto new_copy = libmongoc::server_description_new_copy.create_instance();
        auto id = libmongoc::server_description_id.create_instance();

        static constexpr std::size_t max_count = {3u};

        std::array<identity_type, max_count> sdids = {};
        auto const count = GENERATE(range(std::size_t{0}, max_count));
        CAPTURE(count);

        std::size_t destroy_count = 0;
        std::size_t copy_count = 0;

        destroy
            ->interpose([&](mongoc_server_description_t* sd) -> void {
                if (sd) {
                    ++destroy_count;
                }
            })
            .forever();

        get_servers->interpose(
            [&](mongoc_topology_description_t const* ptr, std::size_t* n) -> mongoc_server_description_t** {
                CHECK(static_cast<void const*>(ptr) == &tdid);
                REQUIRE(n != nullptr);

                auto const ret = static_cast<mongoc_server_description_t**>(
                    bson_malloc0(max_count * sizeof(mongoc_server_description_t*)));

                for (std::size_t i = 0u; i < count; ++i) {
                    ret[i] = reinterpret_cast<mongoc_server_description_t*>(&sdids[i]);
                }

                *n = count;

                return ret;
            });

        {
            auto const servers = td.servers();

            REQUIRE(servers.size() == count);

            for (std::size_t i = 0u; i < count; ++i) {
                CAPTURE(i);
                auto const ptr = server_description::internal::as_mongoc(servers[i]);
                CHECK(static_cast<void const*>(ptr) == &sdids[i]);
            }
        }

        CHECK(destroy_count == count); // Owning object destruction.
        CHECK(copy_count == 0);        // Ownership transfer of each element.
    }
}

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][events][server_description]") {
    identity_type id1;
    identity_type id2;
    identity_type id3;

    auto const sd1 = reinterpret_cast<mongoc_server_description_t*>(&id1);
    auto const sd2 = reinterpret_cast<mongoc_server_description_t*>(&id2);
    auto const sd3 = reinterpret_cast<mongoc_server_description_t*>(&id3);

    auto destroy = libmongoc::server_description_destroy.create_instance();
    auto new_copy = libmongoc::server_description_new_copy.create_instance();

    int destroy_count = 0;
    int copy_count = 0;

    destroy
        ->interpose([&](mongoc_server_description_t* sd) -> void {
            if (sd) {
                if (sd != sd1 && sd != sd2 && sd != sd3) {
                    FAIL("unexpected mongoc_server_description_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    new_copy
        ->interpose([&](mongoc_server_description_t const* sd) -> mongoc_server_description_t* {
            if (sd == sd1) {
                ++copy_count;
                return sd2;
            } else if (sd == sd2) {
                ++copy_count;
                return sd3;
            } else {
                FAIL("unexpected mongoc_server_description_t");
                return nullptr;
            }
        })
        .forever();

    auto source = server_description::internal::make(sd1);
    auto target = server_description::internal::make(sd2);

    REQUIRE(server_description::internal::as_mongoc(source) == sd1);
    REQUIRE(server_description::internal::as_mongoc(target) == sd2);

    SECTION("move") {
        {
            auto move = std::move(source);

            CHECK(server_description::internal::as_mongoc(source) == nullptr);
            CHECK(server_description::internal::as_mongoc(move) == sd1);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 0);

            target = std::move(move);

            CHECK(server_description::internal::as_mongoc(move) == nullptr);
            CHECK(server_description::internal::as_mongoc(target) == sd1);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 0);
        }

        CHECK(destroy_count == 1);
    }

    SECTION("copy") {
        {
            auto copy = source;

            CHECK(server_description::internal::as_mongoc(source) == sd1);
            CHECK(server_description::internal::as_mongoc(copy) == sd2);
            CHECK(destroy_count == 0);
            CHECK(copy_count == 1);

            target = copy;

            CHECK(server_description::internal::as_mongoc(copy) == sd2);
            CHECK(server_description::internal::as_mongoc(target) == sd3);
            CHECK(destroy_count == 1);
            CHECK(copy_count == 2);
        }

        CHECK(destroy_count == 2);
    }
}

TEST_CASE("command_started", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        command_started::internal::make(reinterpret_cast<mongoc_apm_command_started_t const*>(&identity));

    SECTION("command") {
        test_bson(&command_started::command, libmongoc::apm_command_started_get_command, event, &identity);
    }

    test_database_name(libmongoc::apm_command_started_get_database_name, event, &identity);
    test_command_name(libmongoc::apm_command_started_get_command_name, event, &identity);
    test_request_id(libmongoc::apm_command_started_get_request_id, event, &identity);
    test_operation_id(libmongoc::apm_command_started_get_operation_id, event, &identity);
    test_service_id(libmongoc::apm_command_started_get_service_id, event, &identity);
    test_host_and_port(libmongoc::apm_command_started_get_host, event, &identity);
}

TEST_CASE("command_failed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = command_failed::internal::make(reinterpret_cast<mongoc_apm_command_failed_t const*>(&identity));

    SECTION("failure") {
        test_bson(&command_failed::failure, libmongoc::apm_command_failed_get_reply, event, &identity);
    }

    test_command_name(libmongoc::apm_command_failed_get_command_name, event, &identity);
    test_duration(libmongoc::apm_command_failed_get_duration, event, &identity);
    test_request_id(libmongoc::apm_command_failed_get_request_id, event, &identity);
    test_operation_id(libmongoc::apm_command_failed_get_operation_id, event, &identity);
    test_service_id(libmongoc::apm_command_failed_get_service_id, event, &identity);
    test_host_and_port(libmongoc::apm_command_failed_get_host, event, &identity);
}

TEST_CASE("command_succeeded", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        command_succeeded::internal::make(reinterpret_cast<mongoc_apm_command_succeeded_t const*>(&identity));

    test_reply(libmongoc::apm_command_succeeded_get_reply, event, &identity);
    test_command_name(libmongoc::apm_command_succeeded_get_command_name, event, &identity);
    test_duration(libmongoc::apm_command_succeeded_get_duration, event, &identity);
    test_request_id(libmongoc::apm_command_succeeded_get_request_id, event, &identity);
    test_operation_id(libmongoc::apm_command_succeeded_get_operation_id, event, &identity);
    test_service_id(libmongoc::apm_command_succeeded_get_service_id, event, &identity);
    test_host_and_port(libmongoc::apm_command_succeeded_get_host, event, &identity);
}

TEST_CASE("server_opening", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = server_opening::internal::make(reinterpret_cast<mongoc_apm_server_opening_t const*>(&identity));

    test_host_and_port(libmongoc::apm_server_opening_get_host, event, &identity);
    test_topology_id(libmongoc::apm_server_opening_get_topology_id, event, &identity);
}

TEST_CASE("server_closed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = server_closed::internal::make(reinterpret_cast<mongoc_apm_server_closed_t const*>(&identity));

    test_host_and_port(libmongoc::apm_server_closed_get_host, event, &identity);
    test_topology_id(libmongoc::apm_server_closed_get_topology_id, event, &identity);
}

TEST_CASE("server_description_changed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        server_description_changed::internal::make(reinterpret_cast<mongoc_apm_server_changed_t const*>(&identity));

    test_host_and_port(libmongoc::apm_server_changed_get_host, event, &identity);
    test_topology_id(libmongoc::apm_server_changed_get_topology_id, event, &identity);

    SECTION("previous_description") {
        test_server_description(
            &server_description_changed::previous_description,
            libmongoc::apm_server_changed_get_previous_description,
            event,
            &identity);
    }

    SECTION("new_description") {
        test_server_description(
            &server_description_changed::new_description,
            libmongoc::apm_server_changed_get_new_description,
            event,
            &identity);
    }
}

TEST_CASE("topology_opening", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        topology_opening::internal::make(reinterpret_cast<mongoc_apm_topology_opening_t const*>(&identity));

    test_topology_id(libmongoc::apm_topology_opening_get_topology_id, event, &identity);
}

TEST_CASE("topology_closed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        topology_closed::internal::make(reinterpret_cast<mongoc_apm_topology_closed_t const*>(&identity));

    test_topology_id(libmongoc::apm_topology_closed_get_topology_id, event, &identity);
}

TEST_CASE("topology_description_changed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event =
        topology_description_changed::internal::make(reinterpret_cast<mongoc_apm_topology_changed_t const*>(&identity));

    test_topology_id(libmongoc::apm_topology_changed_get_topology_id, event, &identity);

    SECTION("previous_description") {
        test_topology_description(
            &topology_description_changed::previous_description,
            libmongoc::apm_topology_changed_get_previous_description,
            event,
            &identity);
    }

    SECTION("new_description") {
        test_topology_description(
            &topology_description_changed::new_description,
            libmongoc::apm_topology_changed_get_new_description,
            event,
            &identity);
    }
}

TEST_CASE("server_heartbeat_started", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = server_heartbeat_started::internal::make(
        reinterpret_cast<mongoc_apm_server_heartbeat_started_t const*>(&identity));

    test_host_and_port(libmongoc::apm_server_heartbeat_started_get_host, event, &identity);
    test_awaited(libmongoc::apm_server_heartbeat_started_get_awaited, event, &identity);
}

TEST_CASE("server_heartbeat_failed", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = server_heartbeat_failed::internal::make(
        reinterpret_cast<mongoc_apm_server_heartbeat_failed_t const*>(&identity));

    SECTION("message") {
        auto const v = GENERATE("", "x", "abc");

        auto get_error = libmongoc::apm_server_heartbeat_failed_get_error.create_instance();
        get_error->interpose([&](mongoc_apm_server_heartbeat_failed_t const* ptr, bson_error_t* error) -> void {
            CHECK(static_cast<void const*>(ptr) == &identity);
            REQUIRE(error);
            bson_set_error(error, 0, 0, "%s", v);
        });
        CHECK(event.message() == v);
    }

    test_duration(libmongoc::apm_server_heartbeat_failed_get_duration, event, &identity);
    test_host_and_port(libmongoc::apm_server_heartbeat_failed_get_host, event, &identity);
    test_awaited(libmongoc::apm_server_heartbeat_failed_get_awaited, event, &identity);
}

TEST_CASE("server_heartbeat_succeeded", "[mongocxx][v1][events]") {
    identity_type const identity;

    auto const event = server_heartbeat_succeeded::internal::make(
        reinterpret_cast<mongoc_apm_server_heartbeat_succeeded_t const*>(&identity));

    test_reply(libmongoc::apm_server_heartbeat_succeeded_get_reply, event, &identity);
    test_duration(libmongoc::apm_server_heartbeat_succeeded_get_duration, event, &identity);
    test_host_and_port(libmongoc::apm_server_heartbeat_succeeded_get_host, event, &identity);
    test_awaited(libmongoc::apm_server_heartbeat_succeeded_get_awaited, event, &identity);
}

} // namespace events
} // namespace v1
} // namespace mongocxx
