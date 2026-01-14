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

#include <mongocxx/v1/bulk_write.hh>

//

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/server_error.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

struct op_mocks {
    using insert_one_type = decltype(libmongoc::bulk_operation_insert_with_opts.create_instance());
    using delete_one_type = decltype(libmongoc::bulk_operation_remove_one_with_opts.create_instance());
    using delete_many_type = decltype(libmongoc::bulk_operation_remove_many_with_opts.create_instance());
    using update_one_type = decltype(libmongoc::bulk_operation_update_one_with_opts.create_instance());
    using update_many_type = decltype(libmongoc::bulk_operation_update_many_with_opts.create_instance());
    using replace_one_type = decltype(libmongoc::bulk_operation_replace_one_with_opts.create_instance());

    insert_one_type insert_one = libmongoc::bulk_operation_insert_with_opts.create_instance();
    delete_one_type delete_one = libmongoc::bulk_operation_remove_one_with_opts.create_instance();
    delete_many_type delete_many = libmongoc::bulk_operation_remove_many_with_opts.create_instance();
    update_one_type update_one = libmongoc::bulk_operation_update_one_with_opts.create_instance();
    update_many_type update_many = libmongoc::bulk_operation_update_many_with_opts.create_instance();
    replace_one_type replace_one = libmongoc::bulk_operation_replace_one_with_opts.create_instance();
};

} // namespace

TEST_CASE("exceptions", "[mongocxx][v1][bulk_write]") {
    identity_type id;

    auto const identity = reinterpret_cast<mongoc_bulk_operation_t*>(&id);

    auto destroy = libmongoc::bulk_operation_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulk_operation_t* bulk) -> void {
            if (bulk != identity) {
                FAIL("unexpected mongoc_bulk_operation_t");
            }
        })
        .forever();

    auto bulk = v1::bulk_write::internal::make(identity);

    bsoncxx::v1::document::value const empty;

    SECTION("append") {
        static constexpr std::size_t size = 6u;

        std::array<bulk_write::single, size> const ops = {{
            bulk_write::insert_one{empty},
            bulk_write::delete_one{empty},
            bulk_write::delete_many{empty},
            bulk_write::update_one{empty, empty},
            bulk_write::update_many{empty, empty},
            bulk_write::replace_one{empty, empty},
        }};

        auto const set_error = [&](bson_error_t* error) {
            REQUIRE(error != nullptr);
            bson_set_error(error, MONGOC_ERROR_COMMAND, 123, "append failure");
            error->reserved = 2; // MONGOC_ERROR_CATEGORY
        };

        auto const insert_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(opts == nullptr);
            set_error(error);
            return false;
        };

        auto const delete_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            set_error(error);
            return false;
        };

        auto const delete_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            set_error(error);
            return false;
        };

        auto const update_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            set_error(error);
            return false;
        };

        auto const update_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            set_error(error);
            return false;
        };

        auto const replace_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            set_error(error);
            return false;
        };

        op_mocks mocks;

        mocks.insert_one->interpose(insert_one_fn);
        mocks.delete_one->interpose(delete_one_fn);
        mocks.delete_many->interpose(delete_many_fn);
        mocks.update_one->interpose(update_one_fn);
        mocks.update_many->interpose(update_many_fn);
        mocks.replace_one->interpose(replace_one_fn);

        auto const idx = GENERATE(range(std::size_t{0}, size));
        CAPTURE(idx);

        REQUIRE(idx < ops.size());

        try {
            bulk.append(ops[idx]);
            FAIL("should not reach this point");
        } catch (v1::exception const& ex) {
            CHECK(ex.code() == v1::source_errc::mongoc);
            CHECK(ex.code().value() == 123);
            CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("append failure"));
        }

        CHECK(bulk.empty());
    }

    SECTION("execute") {
        auto execute = libmongoc::bulk_operation_execute.create_instance();

        SECTION("v1::exception") {
            execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) -> std::uint32_t {
                CHECK(bulk == identity);
                CHECK(reply != nullptr);
                CHECK(error != nullptr);

                bson_set_error(error, 0, 123, "execute failure");
                error->reserved = 2u; // MONGOC_ERROR_CATEGORY

                return 0u;
            });

            try {
                bulk.execute();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == 123);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("execute failure"));
            }
        }

        SECTION("v1::server_error") {
            scoped_bson const error_doc{R"({"code": 123, "x": 1})"};

            execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) -> std::uint32_t {
                CHECK(bulk == identity);
                CHECK(reply != nullptr);
                CHECK(error != nullptr);

                bson_set_error(error, 0, 456, "execute failure");
                error->reserved = 2u; // MONGOC_ERROR_CATEGORY

                bson_copy_to(error_doc.bson(), reply);

                return 0u;
            });

            try {
                bulk.execute();
                FAIL("should not reach this point");
            } catch (v1::server_error const& ex) {
                CHECK(ex.code() == v1::source_errc::server);
                CHECK(ex.code().value() == 123);
                CHECK(ex.client_code() == v1::source_errc::mongoc);
                CHECK(ex.client_code().value() == 456);
                CHECK(ex.raw()["x"].get_int32().value == 1);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("execute failure"));
            }
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write]") {
    identity_type id1;
    identity_type id2;

    auto const bulk1 = reinterpret_cast<mongoc_bulk_operation_t*>(&id1);
    auto const bulk2 = reinterpret_cast<mongoc_bulk_operation_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::bulk_operation_destroy.create_instance();

    destroy
        ->interpose([&](mongoc_bulk_operation_t* bulk) -> void {
            if (bulk) {
                if (bulk != bulk1 && bulk != bulk2) {
                    FAIL("unexpected mongoc_bulk_operation_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    auto source = bulk_write::internal::make(bulk1);
    auto target = bulk_write::internal::make(bulk2);

    REQUIRE(bulk_write::internal::as_mongoc(source) == bulk1);
    REQUIRE(bulk_write::internal::as_mongoc(target) == bulk2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-move-only state.

            CHECK(bulk_write::internal::as_mongoc(move) == bulk1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-move-only state.

            CHECK(bulk_write::internal::as_mongoc(target) == bulk1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

namespace {

template <typename Op>
void test_ownership(Op& target, Op& source) {
    REQUIRE(source.filter()["source"]);
    REQUIRE(target.filter()["target"]);

    auto const source_value = source.filter();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.filter() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.filter() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.filter() == source_value);
        CHECK(copy.filter() == source_value);

        target = copy;

        CHECK(copy.filter() == source_value);
        CHECK(target.filter() == source_value);
    }
}

template <>
void test_ownership(bulk_write::insert_one&, bulk_write::insert_one&) {
    // Avoid unnecessarily re-testing the ownership semantics of `bsoncxx::v1::document::value`.
    // static_assert(std::is_aggregate_v<bulk_write::insert_one>); // Requires C++17 or newer.
}

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][insert_one]") {
    bulk_write::insert_one source{scoped_bson{R"({"source": 1})"}.value()};
    bulk_write::insert_one target{scoped_bson{R"({"target": 2})"}.value()};

    test_ownership<bulk_write::insert_one>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][update_one]") {
    bulk_write::update_one source{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()};
    bulk_write::update_one target{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()};

    test_ownership<bulk_write::update_one>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][update_many]") {
    bulk_write::update_many source{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()};
    bulk_write::update_many target{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()};

    test_ownership<bulk_write::update_many>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][replace_one]") {
    bulk_write::replace_one source{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()};
    bulk_write::replace_one target{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()};

    test_ownership<bulk_write::replace_one>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][delete_one]") {
    bulk_write::delete_one source{scoped_bson{R"({"source": 1})"}.value()};
    bulk_write::delete_one target{scoped_bson{R"({"target": 2})"}.value()};

    test_ownership<bulk_write::delete_one>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][delete_many]") {
    bulk_write::delete_many source{scoped_bson{R"({"source": 1})"}.value()};
    bulk_write::delete_many target{scoped_bson{R"({"target": 2})"}.value()};

    test_ownership<bulk_write::delete_many>(target, source);
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][single]") {
    static std::size_t constexpr size = 6u;

    auto const source_type = GENERATE(range(std::size_t{0}, size));
    CAPTURE(source_type);

    auto const target_type = GENERATE(range(std::size_t{0}, size));
    CAPTURE(target_type);

    REQUIRE(source_type < size);
    REQUIRE(target_type < size);

    std::array<bulk_write::single, size> sources = {{
        bulk_write::insert_one{scoped_bson{R"({"source": 1})"}.value()},
        bulk_write::delete_one{scoped_bson{R"({"source": 1})"}.value()},
        bulk_write::delete_many{scoped_bson{R"({"source": 1})"}.value()},
        bulk_write::update_one{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()},
        bulk_write::update_many{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()},
        bulk_write::replace_one{scoped_bson{R"({"source": 1})"}.value(), scoped_bson{}.value()},
    }};

    std::array<bulk_write::single, size> targets = {{
        bulk_write::insert_one{scoped_bson{R"({"target": 2})"}.value()},
        bulk_write::delete_one{scoped_bson{R"({"target": 2})"}.value()},
        bulk_write::delete_many{scoped_bson{R"({"target": 2})"}.value()},
        bulk_write::update_one{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()},
        bulk_write::update_many{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()},
        bulk_write::replace_one{scoped_bson{R"({"target": 2})"}.value(), scoped_bson{}.value()},
    }};

    auto& source = sources[source_type];
    auto& target = targets[target_type];

    REQUIRE(source.type() == static_cast<bulk_write::type>(source_type));
    REQUIRE(target.type() == static_cast<bulk_write::type>(target_type));

    auto const get_value = [](bulk_write::single const& op) -> bsoncxx::v1::document::view {
        switch (op.type()) {
            case bulk_write::type::k_insert_one:
                return op.get_insert_one().value;
            case bulk_write::type::k_update_one:
                return op.get_update_one().filter();
            case bulk_write::type::k_update_many:
                return op.get_update_many().filter();
            case bulk_write::type::k_delete_one:
                return op.get_delete_one().filter();
            case bulk_write::type::k_delete_many:
                return op.get_delete_many().filter();
            case bulk_write::type::k_replace_one:
                return op.get_replace_one().filter();
            default:
                FAIL("should not reach this point");
                return {};
        }
    };

    auto const source_data = get_value(source).data();
    auto const source_value = bsoncxx::v1::document::value{get_value(source)};

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(get_value(move).data() == source_data);
        CHECK(get_value(move) == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(get_value(target).data() == source_data);
        CHECK(get_value(target) == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(get_value(source).data() == source_data);
        CHECK(get_value(source) == source_value);

        CHECK(get_value(copy).data() != source_data);
        CHECK(get_value(copy) == source_value);

        auto const copy_data = get_value(copy).data();

        target = copy;

        CHECK(get_value(copy).data() == copy_data);
        CHECK(get_value(copy) == source_value);

        CHECK(get_value(target).data() != source_data);
        CHECK(get_value(target).data() != copy_data);
        CHECK(get_value(target) == source_value);
    }

    SECTION("getters") {
        SECTION("move") {
            switch (source.type()) {
                case bulk_write::type::k_insert_one: {
                    auto const v = std::move(source).get_insert_one();
                    CHECK(v.value.data() == source_data);
                    CHECK(v.value == source_value);
                } break;

                case bulk_write::type::k_update_one: {
                    auto const v = std::move(source).get_update_one();
                    CHECK(v.filter().data() == source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_update_many: {
                    auto const v = std::move(source).get_update_many();
                    CHECK(v.filter().data() == source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_delete_one: {
                    auto const v = std::move(source).get_delete_one();
                    CHECK(v.filter().data() == source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_delete_many: {
                    auto const v = std::move(source).get_delete_many();
                    CHECK(v.filter().data() == source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_replace_one: {
                    auto const v = std::move(source).get_replace_one();
                    CHECK(v.filter().data() == source_data);
                    CHECK(v.filter() == source_value);
                } break;

                default:
                    FAIL("should not reach this point");
            }
        }

        SECTION("copy") {
            switch (source.type()) {
                case bulk_write::type::k_insert_one: {
                    auto const v = source.get_insert_one();
                    CHECK(v.value.data() != source_data);
                    CHECK(v.value == source_value);
                } break;

                case bulk_write::type::k_update_one: {
                    auto const v = source.get_update_one();
                    CHECK(v.filter().data() != source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_update_many: {
                    auto const v = source.get_update_many();
                    CHECK(v.filter().data() != source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_delete_one: {
                    auto const v = source.get_delete_one();
                    CHECK(v.filter().data() != source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_delete_many: {
                    auto const v = source.get_delete_many();
                    CHECK(v.filter().data() != source_data);
                    CHECK(v.filter() == source_value);
                } break;

                case bulk_write::type::k_replace_one: {
                    auto const v = source.get_replace_one();
                    CHECK(v.filter().data() != source_data);
                    CHECK(v.filter() == source_value);
                } break;

                default:
                    FAIL("should not reach this point");
            }
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][options]") {
    bulk_write::options source;
    bulk_write::options target;

    bsoncxx::v1::types::value source_value{"source"};
    bsoncxx::v1::types::value target_value{"target"};

    source.comment(source_value);
    target.comment(target_value);

    REQUIRE(source.comment() == source_value);
    REQUIRE(target.comment() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.comment() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.comment() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.comment() == source_value);
        CHECK(copy.comment() == source_value);

        target = copy;

        CHECK(copy.comment() == source_value);
        CHECK(target.comment() == source_value);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][bulk_write][result]") {
    auto const source_value = scoped_bson{R"({"source": 1})"}.value();
    auto const target_value = scoped_bson{R"({"target": 2})"}.value();

    auto source = bulk_write::result::internal::make(source_value);
    auto target = bulk_write::result::internal::make(target_value);

    auto const reply = [](bulk_write::result const& result) { return bulk_write::result::internal::reply(result); };

    REQUIRE(reply(source) == source_value);
    REQUIRE(reply(target) == target_value);

    auto const source_data = reply(source).data();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(reply(move).data() == source_data);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(reply(target).data() == source_data);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(reply(source).data() == source_data);
        CHECK(reply(source) == source_value);

        CHECK(reply(copy).data() != source_data);
        CHECK(reply(copy) == source_value);

        auto const copy_data = reply(copy).data();

        target = copy;

        CHECK(reply(copy).data() == copy_data);
        CHECK(reply(copy) == source_value);

        CHECK(reply(target).data() != source_data);
        CHECK(reply(target).data() != copy_data);
        CHECK(reply(target) == source_value);
    }
}

TEST_CASE("empty", "[mongocxx][v1][bulk_write]") {
    identity_type id;

    auto const identity = reinterpret_cast<mongoc_bulk_operation_t*>(&id);

    auto destroy = libmongoc::bulk_operation_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulk_operation_t* bulk) -> void {
            if (bulk != identity) {
                FAIL("unexpected mongoc_bulk_operation_t");
            }
        })
        .forever();

    auto bulk = v1::bulk_write::internal::make(identity);

    SECTION("default") {
        CHECK(bulk.empty());
    }

    SECTION("append") {
        static constexpr std::size_t size = 6u;

        bsoncxx::v1::document::value const empty;

        std::array<bulk_write::single, size> const ops = {{
            bulk_write::insert_one{empty},
            bulk_write::delete_one{empty},
            bulk_write::delete_many{empty},
            bulk_write::update_one{empty, empty},
            bulk_write::update_many{empty, empty},
            bulk_write::replace_one{empty, empty},
        }};
        auto const insert_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(opts == nullptr);
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const update_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const update_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const replace_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto insert_one = libmongoc::bulk_operation_insert_with_opts.create_instance();
        auto delete_one = libmongoc::bulk_operation_remove_one_with_opts.create_instance();
        auto delete_many = libmongoc::bulk_operation_remove_many_with_opts.create_instance();
        auto update_one = libmongoc::bulk_operation_update_one_with_opts.create_instance();
        auto update_many = libmongoc::bulk_operation_update_many_with_opts.create_instance();
        auto replace_one = libmongoc::bulk_operation_replace_one_with_opts.create_instance();

        insert_one->interpose(insert_one_fn);
        delete_one->interpose(delete_one_fn);
        delete_many->interpose(delete_many_fn);
        update_one->interpose(update_one_fn);
        update_many->interpose(update_many_fn);
        replace_one->interpose(replace_one_fn);

        auto const idx = GENERATE(range(std::size_t{0}, size));
        CAPTURE(idx);

        CHECK_NOTHROW(bulk.append(ops[idx]));

        CHECK_FALSE(bulk.empty());
    }
}

namespace {

template <typename Fn, typename... Args>
bool op_visit(bulk_write::single& op, Fn fn, Args&&... args) {
    switch (op.type()) {
        case bulk_write::type::k_insert_one:
            return fn(v1::bulk_write::single::internal::get_insert_one(op), std::forward<Args>(args)...);
        case bulk_write::type::k_update_one:
            return fn(v1::bulk_write::single::internal::get_update_one(op), std::forward<Args>(args)...);
        case bulk_write::type::k_update_many:
            return fn(v1::bulk_write::single::internal::get_update_many(op), std::forward<Args>(args)...);
        case bulk_write::type::k_delete_one:
            return fn(v1::bulk_write::single::internal::get_delete_one(op), std::forward<Args>(args)...);
        case bulk_write::type::k_delete_many:
            return fn(v1::bulk_write::single::internal::get_delete_many(op), std::forward<Args>(args)...);
        case bulk_write::type::k_replace_one:
            return fn(v1::bulk_write::single::internal::get_replace_one(op), std::forward<Args>(args)...);

        default:
            FAIL("should not reach this point");
            return false;
    }
}

#define TEST_APPEND_OP_FIELD(_field, _type)                                                    \
    struct BSONCXX_PRIVATE_CONCAT(test_append_op_, _field) {                                   \
        using T = _type;                                                                       \
                                                                                               \
        template <typename Op>                                                                 \
        using set_expr = decltype(std::declval<Op&>()._field(std::declval<T>()));              \
                                                                                               \
        template <typename Op>                                                                 \
        using get_expr = decltype(std::declval<Op const&>()._field());                         \
                                                                                               \
        template <typename Op>                                                                 \
        struct has_setter : bsoncxx::detail::is_detected<set_expr, Op> {};                     \
                                                                                               \
        template <typename Op>                                                                 \
        struct has_getter : bsoncxx::detail::is_detected<get_expr, Op> {};                     \
                                                                                               \
        template <typename Op>                                                                 \
        struct has_field : bsoncxx::detail::conjunction<has_setter<Op>, has_getter<Op>> {};    \
                                                                                               \
        template <typename Op, bsoncxx::detail::enable_if_t<has_field<Op>::value>* = nullptr>  \
        bool operator()(Op& op, T v) {                                                         \
            CHECK(op._field(v)._field() == v);                                                 \
            return true;                                                                       \
        }                                                                                      \
                                                                                               \
        template <typename Op, bsoncxx::detail::enable_if_t<!has_field<Op>::value>* = nullptr> \
        bool operator()(Op& op, T v) {                                                         \
            (void)op;                                                                          \
            (void)v;                                                                           \
            return false;                                                                      \
        }                                                                                      \
    }

TEST_APPEND_OP_FIELD(collation, bsoncxx::v1::document::value);
TEST_APPEND_OP_FIELD(hint, v1::hint);
TEST_APPEND_OP_FIELD(sort, bsoncxx::v1::document::value);
TEST_APPEND_OP_FIELD(upsert, bool);
TEST_APPEND_OP_FIELD(array_filters, bsoncxx::v1::array::value);

static_assert(!test_append_op_collation::has_field<bulk_write::insert_one>::value, "");
static_assert(test_append_op_collation::has_field<bulk_write::update_one>::value, "");
static_assert(test_append_op_collation::has_field<bulk_write::update_many>::value, "");
static_assert(test_append_op_collation::has_field<bulk_write::replace_one>::value, "");
static_assert(test_append_op_collation::has_field<bulk_write::delete_one>::value, "");
static_assert(test_append_op_collation::has_field<bulk_write::delete_many>::value, "");

static_assert(!test_append_op_hint::has_field<bulk_write::insert_one>::value, "");
static_assert(test_append_op_hint::has_field<bulk_write::update_one>::value, "");
static_assert(test_append_op_hint::has_field<bulk_write::update_many>::value, "");
static_assert(test_append_op_hint::has_field<bulk_write::replace_one>::value, "");
static_assert(test_append_op_hint::has_field<bulk_write::delete_one>::value, "");
static_assert(test_append_op_hint::has_field<bulk_write::delete_many>::value, "");

static_assert(!test_append_op_sort::has_field<bulk_write::insert_one>::value, "");
static_assert(test_append_op_sort::has_field<bulk_write::update_one>::value, "");
static_assert(!test_append_op_sort::has_field<bulk_write::update_many>::value, "");
static_assert(test_append_op_sort::has_field<bulk_write::replace_one>::value, "");
static_assert(!test_append_op_sort::has_field<bulk_write::delete_one>::value, "");
static_assert(!test_append_op_sort::has_field<bulk_write::delete_many>::value, "");

static_assert(!test_append_op_upsert::has_field<bulk_write::insert_one>::value, "");
static_assert(test_append_op_upsert::has_field<bulk_write::update_one>::value, "");
static_assert(test_append_op_upsert::has_field<bulk_write::update_many>::value, "");
static_assert(test_append_op_upsert::has_field<bulk_write::replace_one>::value, "");
static_assert(!test_append_op_upsert::has_field<bulk_write::delete_one>::value, "");
static_assert(!test_append_op_upsert::has_field<bulk_write::delete_many>::value, "");

static_assert(!test_append_op_array_filters::has_field<bulk_write::insert_one>::value, "");
static_assert(test_append_op_array_filters::has_field<bulk_write::update_one>::value, "");
static_assert(test_append_op_array_filters::has_field<bulk_write::update_many>::value, "");
static_assert(!test_append_op_array_filters::has_field<bulk_write::replace_one>::value, "");
static_assert(!test_append_op_array_filters::has_field<bulk_write::delete_one>::value, "");
static_assert(!test_append_op_array_filters::has_field<bulk_write::delete_many>::value, "");

} // namespace

TEST_CASE("append", "[mongocxx][v1][bulk_write]") {
    identity_type id;
    auto const identity = reinterpret_cast<mongoc_bulk_operation_t*>(&id);

    auto destroy = libmongoc::bulk_operation_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulk_operation_t* bulk) -> void {
            if (bulk != identity) {
                FAIL("unexpected mongoc_bulk_operation_t");
            }
        })
        .forever();

    auto bulk = v1::bulk_write::internal::make(identity);

    CHECK(bulk.empty());

    bsoncxx::v1::document::value const empty;

    static constexpr std::size_t size = 6u;

    std::array<bulk_write::single, size> ops = {{
        bulk_write::insert_one{empty},
        bulk_write::delete_one{empty},
        bulk_write::delete_many{empty},
        bulk_write::update_one{empty, empty},
        bulk_write::update_many{empty, empty},
        bulk_write::replace_one{empty, empty},
    }};

    auto const idx = GENERATE(range(std::size_t{0}, size));
    CAPTURE(idx);
    auto& op = ops[idx];

    SECTION("required fields") {
        scoped_bson one{R"({"x": 1})"};
        scoped_bson two{R"({"y": 2})"};

        auto const one_data = one.data();
        auto const two_data = two.data();

        auto const insert_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{document}.data() == one_data);
            CHECK(opts == nullptr);
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == one_data);
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == one_data);
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const update_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == one_data);
            CHECK(scoped_bson_view{document}.data() == two_data);
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const update_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == one_data);
            CHECK(scoped_bson_view{document}.data() == two_data);
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        auto const replace_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == one_data);
            CHECK(scoped_bson_view{document}.data() == two_data);
            CHECK(scoped_bson_view{opts}.data() == empty.data());
            CHECK(error != nullptr);
            return true;
        };

        op_mocks mocks;

        mocks.insert_one->interpose(insert_one_fn);
        mocks.delete_one->interpose(delete_one_fn);
        mocks.delete_many->interpose(delete_many_fn);
        mocks.update_one->interpose(update_one_fn);
        mocks.update_many->interpose(update_many_fn);
        mocks.replace_one->interpose(replace_one_fn);

        switch (static_cast<bulk_write::type>(idx)) {
            case bulk_write::type::k_insert_one: {
                auto& v = v1::bulk_write::single::internal::get_insert_one(op);
                v.value = std::move(one).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;

            case bulk_write::type::k_delete_one: {
                auto& v = v1::bulk_write::single::internal::get_delete_one(op);
                v1::bulk_write::delete_one::internal::filter(v) = std::move(one).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;

            case bulk_write::type::k_delete_many: {
                auto& v = v1::bulk_write::single::internal::get_delete_many(op);
                v1::bulk_write::delete_many::internal::filter(v) = std::move(one).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;

            case bulk_write::type::k_update_one: {
                auto& v = v1::bulk_write::single::internal::get_update_one(op);
                v1::bulk_write::update_one::internal::filter(v) = std::move(one).value();
                v1::bulk_write::update_one::internal::update(v) = std::move(two).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;

            case bulk_write::type::k_update_many: {
                auto& v = v1::bulk_write::single::internal::get_update_many(op);
                v1::bulk_write::update_many::internal::filter(v) = std::move(one).value();
                v1::bulk_write::update_many::internal::update(v) = std::move(two).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;

            case bulk_write::type::k_replace_one: {
                auto& v = v1::bulk_write::single::internal::get_replace_one(op);
                v1::bulk_write::replace_one::internal::filter(v) = std::move(one).value();
                v1::bulk_write::replace_one::internal::replacement(v) = std::move(two).value();
                CHECK_NOTHROW(bulk.append(op));
            } break;
        }
    }

    SECTION("options") {
        bsoncxx::v1::stdx::string_view field_name;

        auto const insert_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(opts == nullptr);
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.view()[field_name]);
            CHECK(error != nullptr);
            return true;
        };

        auto const delete_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.view()[field_name]);
            CHECK(error != nullptr);
            return true;
        };

        auto const update_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                       bson_t const* selector,
                                       bson_t const* document,
                                       bson_t const* opts,
                                       bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.view()[field_name]);
            CHECK(error != nullptr);
            return true;
        };

        auto const update_many_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.view()[field_name]);
            CHECK(error != nullptr);
            return true;
        };

        auto const replace_one_fn = [&](mongoc_bulk_operation_t* bulk,
                                        bson_t const* selector,
                                        bson_t const* document,
                                        bson_t const* opts,
                                        bson_error_t* error) -> bool {
            CHECK(bulk == identity);
            CHECK(scoped_bson_view{selector}.data() == empty.data());
            CHECK(scoped_bson_view{document}.data() == empty.data());
            CHECK(scoped_bson_view{opts}.view()[field_name]);
            CHECK(error != nullptr);
            return true;
        };

        op_mocks mocks;

        mocks.insert_one->interpose(insert_one_fn);
        mocks.delete_one->interpose(delete_one_fn);
        mocks.delete_many->interpose(delete_many_fn);
        mocks.update_one->interpose(update_one_fn);
        mocks.update_many->interpose(update_many_fn);
        mocks.replace_one->interpose(replace_one_fn);

        SECTION("collation") {
            field_name = "collation";

            auto const v = GENERATE(values({
                scoped_bson{},
                scoped_bson{R"({"x": 1})"},
            }));

            switch (static_cast<bulk_write::type>(idx)) {
                case bulk_write::type::k_update_one:
                case bulk_write::type::k_update_many:
                case bulk_write::type::k_delete_one:
                case bulk_write::type::k_delete_many:
                case bulk_write::type::k_replace_one:
                    CHECK(op_visit(op, test_append_op_collation{}, v.value()));
                    CHECK_NOTHROW(bulk.append(op));
                    break;

                // Not applicable.
                case bulk_write::type::k_insert_one:
                    break;
            }
        }

        SECTION("hint") {
            field_name = "hint";

            auto const v = GENERATE(values({
                v1::hint{"abc"},
                v1::hint{scoped_bson{R"({"x": 1})"}.value()},
            }));

            switch (static_cast<bulk_write::type>(idx)) {
                case bulk_write::type::k_update_one:
                case bulk_write::type::k_update_many:
                case bulk_write::type::k_delete_one:
                case bulk_write::type::k_delete_many:
                case bulk_write::type::k_replace_one:
                    CHECK(op_visit(op, test_append_op_hint{}, v));
                    CHECK_NOTHROW(bulk.append(op));
                    break;

                // Not applicable.
                case bulk_write::type::k_insert_one:
                    break;
            }
        }

        SECTION("sort") {
            field_name = "sort";

            auto const v = GENERATE(values({
                scoped_bson{},
                scoped_bson{R"({"x": 1})"},
            }));

            switch (static_cast<bulk_write::type>(idx)) {
                case bulk_write::type::k_update_one:
                case bulk_write::type::k_replace_one:
                    CHECK(op_visit(op, test_append_op_sort{}, v.value()));
                    CHECK_NOTHROW(bulk.append(op));
                    break;

                // Not applicable.
                case bulk_write::type::k_insert_one:
                case bulk_write::type::k_update_many:
                case bulk_write::type::k_delete_one:
                case bulk_write::type::k_delete_many:
                    break;
            }
        }

        SECTION("upsert") {
            field_name = "upsert";

            auto const v = GENERATE(false, true);

            switch (static_cast<bulk_write::type>(idx)) {
                case bulk_write::type::k_update_one:
                case bulk_write::type::k_update_many:
                case bulk_write::type::k_replace_one:
                    CHECK(op_visit(op, test_append_op_upsert{}, v));
                    CHECK_NOTHROW(bulk.append(op));
                    break;

                // Not applicable.
                case bulk_write::type::k_insert_one:
                case bulk_write::type::k_delete_one:
                case bulk_write::type::k_delete_many:
                    break;
            }
        }

        SECTION("array_filters") {
            field_name = "arrayFilters";

            auto const doc = GENERATE(values({
                scoped_bson{},
                scoped_bson{R"([1, 2.0, "three"])"},
            }));
            auto const v = bsoncxx::v1::array::value{doc.array_view()};

            switch (static_cast<bulk_write::type>(idx)) {
                case bulk_write::type::k_update_one:
                case bulk_write::type::k_update_many:
                    CHECK(op_visit(op, test_append_op_array_filters{}, v));
                    CHECK_NOTHROW(bulk.append(op));
                    break;

                // Not applicable.
                case bulk_write::type::k_insert_one:
                case bulk_write::type::k_delete_one:
                case bulk_write::type::k_delete_many:
                case bulk_write::type::k_replace_one:
                    break;
            }
        }
    }
}

TEST_CASE("execute", "[mongocxx][v1][bulk_write]") {
    identity_type id;

    auto const identity = reinterpret_cast<mongoc_bulk_operation_t*>(&id);

    auto destroy = libmongoc::bulk_operation_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulk_operation_t* bulk) -> void {
            if (bulk != identity) {
                FAIL("unexpected mongoc_bulk_operation_t");
            }
        })
        .forever();

    auto bulk = v1::bulk_write::internal::make(identity);

    auto execute = libmongoc::bulk_operation_execute.create_instance();

    SECTION("acknowledged") {
        auto const server_id = GENERATE(as<std::uint32_t>{}, 1u, UINT32_MAX);

        auto const v = GENERATE(values({
            scoped_bson{R"({"x": 1})"},
            scoped_bson{R"({"y": 2})"},
        }));

        execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) -> std::uint32_t {
            CHECK(bulk == identity);
            CHECK(reply != nullptr);
            CHECK(error != nullptr);

            bson_copy_to(v.bson(), reply);

            return server_id;
        });

        auto const ret = bulk.execute();

        REQUIRE(ret);
        CHECK(v1::bulk_write::result::internal::reply(*ret) == v.view());
    }

    SECTION("unacknowledged") {
        auto const server_id = GENERATE(as<std::uint32_t>{}, 1u, UINT32_MAX);

        execute->interpose([&](mongoc_bulk_operation_t* bulk, bson_t* reply, bson_error_t* error) -> std::uint32_t {
            CHECK(bulk == identity);
            CHECK(reply != nullptr);
            CHECK(error != nullptr);

            bson_copy_to(scoped_bson_view{}.bson(), reply);

            return server_id;
        });

        CHECK_FALSE(bulk.execute().has_value());
    }
}

TEST_CASE("default", "[mongocxx][v1][bulk_write][options]") {
    bulk_write::options const opts;

    CHECK_FALSE(opts.bypass_document_validation().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK(opts.ordered());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("bypass_document_validation", "[mongocxx][v1][bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(bulk_write::options{}.bypass_document_validation(v).bypass_document_validation() == v);
}

TEST_CASE("comment", "[mongocxx][v1][bulk_write][options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(bulk_write::options{}.comment(v).comment() == v);
}

TEST_CASE("let", "[mongocxx][v1][bulk_write][options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(bulk_write::options{}.let(v.value()).let() == v.view());
}

TEST_CASE("ordered", "[mongocxx][v1][bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(bulk_write::options{}.ordered(v).ordered() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][bulk_write][options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(bulk_write::options{}.write_concern(v).write_concern() == v);
}

TEST_CASE("inserted_count", "[mongocxx][v1][bulk_write][result]") {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    auto reply = scoped_bson{BCON_NEW("nInserted", BCON_INT32(v))}.value();
    auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

    CHECK(ret.inserted_count() == v);
}

TEST_CASE("matched_count", "[mongocxx][v1][bulk_write][result]") {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    auto reply = scoped_bson{BCON_NEW("nMatched", BCON_INT32(v))}.value();
    auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

    CHECK(ret.matched_count() == v);
}

TEST_CASE("modified_count", "[mongocxx][v1][bulk_write][result]") {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    auto reply = scoped_bson{BCON_NEW("nModified", BCON_INT32(v))}.value();
    auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

    CHECK(ret.modified_count() == v);
}

TEST_CASE("deleted_count", "[mongocxx][v1][bulk_write][result]") {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    auto reply = scoped_bson{BCON_NEW("nRemoved", BCON_INT32(v))}.value();
    auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

    CHECK(ret.deleted_count() == v);
}

TEST_CASE("upserted_count", "[mongocxx][v1][bulk_write][result]") {
    using T = std::int32_t;

    auto const v = GENERATE(values({
        T{INT32_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT32_MAX},
    }));

    auto reply = scoped_bson{BCON_NEW("nUpserted", BCON_INT32(v))}.value();
    auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

    CHECK(ret.upserted_count() == v);
}

TEST_CASE("upserted_ids", "[mongocxx][v1][bulk_write][result]") {
    using namespace bsoncxx::v1::types;
    using id_map = bulk_write::result::id_map;

    SECTION("invalid") {
        SECTION("upserted is missing") {
            auto const ret = v1::bulk_write::result::internal::make(scoped_bson{}.value());
            CHECK(ret.upserted_ids() == id_map{});
        }

        SECTION("upserted is not an array") {
            auto reply = scoped_bson{R"({"upserted": {"0": {"_id": 1, "index": 1}}})"}.value();
            auto const ret = v1::bulk_write::result::internal::make(std::move(reply));
            CHECK(ret.upserted_ids() == id_map{});
        }

        SECTION("index is missing") {
            auto reply = scoped_bson{R"({"upserted": [{"_id": 1}]})"}.value();
            auto const ret = v1::bulk_write::result::internal::make(std::move(reply));
            CHECK(ret.upserted_ids() == id_map{});
        }

        SECTION("_id is missing") {
            auto reply = scoped_bson{R"({"upserted": [{"index": 1}]})"}.value();
            auto const ret = v1::bulk_write::result::internal::make(std::move(reply));
            CHECK(ret.upserted_ids() == id_map{});
        }

        SECTION("index is not an int32") {
            auto reply = scoped_bson{R"({"upserted": [{"_id": 1, "index": 1.0}]})"}.value();
            auto const ret = v1::bulk_write::result::internal::make(std::move(reply));
            CHECK(ret.upserted_ids() == id_map{});
        }
    }

    SECTION("valid") {
        scoped_bson v;
        id_map expected;

        std::tie(v, expected) = GENERATE(map(
            [](std::tuple<char const*, id_map> i) {
                return std::make_tuple(scoped_bson{std::get<0>(i)}, std::get<1>(std::move(i)));
            },
            table<char const*, id_map>({
                {R"([])", id_map{}},
                {R"([{"_id": "one", "index": 1}])", id_map{{1, b_string{"one"}}}},
                {R"([{"_id": 1, "index": 1}, {"_id": 2.0, "index": 2}, {"_id": "three", "index": 3}])",
                 id_map{{1, b_int32{1}}, {2, b_double{2.0}}, {3, b_string{"three"}}}},
            })));
        CAPTURE(v.view());

        auto reply = scoped_bson{BCON_NEW("upserted", BCON_ARRAY(v.bson()))}.value();
        auto const ret = v1::bulk_write::result::internal::make(std::move(reply));

        CHECK(ret.upserted_ids() == expected);
    }
}

TEST_CASE("equality", "[mongocxx][v1][bulk_write][result]") {
    auto const x = v1::bulk_write::result::internal::make(scoped_bson{R"({"x": 1})"}.value());
    auto const y = v1::bulk_write::result::internal::make(scoped_bson{R"({"y": 2})"}.value());

    CHECK(x == x);
    CHECK(y == y);
    CHECK(x != y);
}

} // namespace v1
} // namespace mongocxx
