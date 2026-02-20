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

#include <mongocxx/v1/bulk_write.hpp>
#include <mongocxx/v1/hint.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <array>
#include <cstddef>
#include <utility>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/bulk_write.hpp>
#include <mongocxx/model/delete_many.hpp>
#include <mongocxx/model/delete_one.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/model/replace_one.hpp>
#include <mongocxx/model/update_many.hpp>
#include <mongocxx/model/update_one.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/write_type.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][model][delete_many]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"filter": 1})").value();
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<v1::hint> hint;

    if (has_value) {
        collation.emplace();
        hint.emplace("hint");
    }

    using v_noabi = v_noabi::model::delete_many;
    using v1 = v1::bulk_write::delete_many;

    SECTION("from_v1") {
        v1 from{filter};

        if (has_value) {
            from.collation(*collation);
            from.hint(*hint);
        }

        v_noabi const to{from};

        CHECK(to.filter() == filter.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{from_v1(filter.view())};

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.hint(*hint);
        }

        v1 const to{from};

        CHECK(to.filter() == filter.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }
}

TEST_CASE("v1", "[mongocxx][v_noabi][model][delete_one]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"filter": 1})").value();
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<v1::hint> hint;

    if (has_value) {
        collation.emplace();
        hint.emplace(v1::hint{"hint"});
    }

    using v_noabi = v_noabi::model::delete_one;
    using v1 = v1::bulk_write::delete_one;

    SECTION("from_v1") {
        v1 from{filter};

        if (has_value) {
            from.collation(*collation);
            from.hint(*hint);
        }

        v_noabi const to{from};

        CHECK(to.filter() == filter.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{from_v1(filter.view())};

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.hint(*hint);
        }

        v1 const to{from};

        CHECK(to.filter() == filter.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }
}

TEST_CASE("v1", "[mongocxx][v_noabi][model][replace_one]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"filter": 1})").value();
    auto const replacement = GENERATE(as<scoped_bson>(), R"({})", R"({"replacement": 2})").value();
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bool> upsert;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> sort;

    if (has_value) {
        collation.emplace();
        upsert.emplace();
        hint.emplace("hint");
        sort.emplace();
    }

    using v_noabi = v_noabi::model::replace_one;
    using v1 = v1::bulk_write::replace_one;

    SECTION("from_v1") {
        v1 from{filter, replacement};

        if (has_value) {
            from.collation(*collation);
            from.upsert(*upsert);
            from.hint(*hint);
            from.sort(*sort);
        }

        v_noabi const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.replacement() == replacement.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.sort().value() == sort->view());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.sort().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{from_v1(filter.view()), from_v1(replacement.view())};

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.upsert(*upsert);
            from.hint(*hint);
            from.sort(from_v1(sort->view()));
        }

        v1 const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.replacement() == replacement.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.sort().value() == sort->view());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.sort().has_value());
        }
    }
}

TEST_CASE("v1", "[mongocxx][v_noabi][model][update_many]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"filter": 1})").value();
    auto const update = GENERATE(as<scoped_bson>(), R"({})", R"({"update": 2})").value();
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> array_filters;
    bsoncxx::v1::stdx::optional<bool> upsert;
    bsoncxx::v1::stdx::optional<v1::hint> hint;

    if (has_value) {
        collation.emplace();
        array_filters.emplace();
        upsert.emplace();
        hint.emplace("hint");
    }

    using v_noabi = v_noabi::model::update_many;
    using v1 = v1::bulk_write::update_many;

    SECTION("from_v1") {
        v1 from{filter, update};

        if (has_value) {
            from.collation(*collation);
            from.array_filters(*array_filters);
            from.upsert(*upsert);
            from.hint(*hint);
        }

        v_noabi const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.update() == update.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.array_filters().value() == array_filters->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.array_filters().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{from_v1(filter.view()), from_v1(update.view())};

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.array_filters(from_v1(array_filters->view()));
            from.upsert(*upsert);
            from.hint(*hint);
        }

        v1 const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.update() == update.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.array_filters().value() == array_filters->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.array_filters().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
        }
    }
}

TEST_CASE("v1", "[mongocxx][v_noabi][model][update_one]") {
    using bsoncxx::v1::stdx::nullopt;
    using bsoncxx::v_noabi::from_v1;

    auto const filter = GENERATE(as<scoped_bson>(), R"({})", R"({"filter": 1})").value();
    auto const update = GENERATE(as<scoped_bson>(), R"({})", R"({"update": 2})").value();
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> array_filters;
    bsoncxx::v1::stdx::optional<bool> upsert;
    bsoncxx::v1::stdx::optional<v1::hint> hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> sort;

    if (has_value) {
        collation.emplace();
        array_filters.emplace();
        upsert.emplace();
        hint.emplace("hint");
        sort.emplace();
    }

    using v_noabi = v_noabi::model::update_one;
    using v1 = v1::bulk_write::update_one;

    SECTION("from_v1") {
        v1 from{filter, update};

        if (has_value) {
            from.collation(*collation);
            from.array_filters(*array_filters);
            from.upsert(*upsert);
            from.hint(*hint);
            from.sort(*sort);
        }

        v_noabi const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.update() == update.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.array_filters().value() == array_filters->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.sort().value() == sort->view());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.array_filters().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.sort().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from{from_v1(filter.view()), from_v1(update.view())};

        if (has_value) {
            from.collation(from_v1(collation->view()));
            from.array_filters(from_v1(array_filters->view()));
            from.upsert(*upsert);
            from.hint(*hint);
            from.sort(from_v1(sort->view()));
        }

        v1 const to{from};

        CHECK(to.filter() == filter.view());
        CHECK(to.update() == update.view());

        if (has_value) {
            CHECK(to.collation().value() == collation->view());
            CHECK(to.array_filters().value() == array_filters->view());
            CHECK(to.upsert() == upsert);
            CHECK(to.hint().value().to_value() == hint->to_value());
            CHECK(to.sort().value() == sort->view());
        } else {
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.array_filters().has_value());
            CHECK_FALSE(to.upsert().has_value());
            CHECK_FALSE(to.hint().has_value());
            CHECK_FALSE(to.sort().has_value());
        }
    }
}

TEST_CASE("v1", "[mongocxx][v_noabi][model][write]") {
    static std::size_t constexpr size = 6u;

    auto const value_type = GENERATE(range(std::size_t{0}, size));
    CAPTURE(value_type);
    REQUIRE(value_type < size);

    using bsoncxx::v_noabi::from_v1;

    auto const insert_one_value = scoped_bson{R"({"insert_one": 1})"}.value();
    auto const delete_one_value = scoped_bson{R"({"delete_one": 1})"}.value();
    auto const delete_many_value = scoped_bson{R"({"delete_many": 1})"}.value();
    auto const update_one_value = scoped_bson{R"({"update_one": 1})"}.value();
    auto const update_many_value = scoped_bson{R"({"update_many": 1})"}.value();
    auto const replace_one_value = scoped_bson{R"({"replace_one": 1})"}.value();

    std::array<v1::bulk_write::single, size> sources_v1 = {{
        v1::bulk_write::insert_one{insert_one_value},
        v1::bulk_write::delete_one{delete_one_value},
        v1::bulk_write::delete_many{delete_many_value},
        v1::bulk_write::update_one{update_one_value, bsoncxx::v1::document::value{}},
        v1::bulk_write::update_many{update_many_value, bsoncxx::v1::document::value{}},
        v1::bulk_write::replace_one{replace_one_value, bsoncxx::v1::document::value{}},
    }};

    std::array<v_noabi::model::write, size> sources_v_noabi = {{
        v_noabi::model::insert_one{from_v1(insert_one_value)},
        v_noabi::model::delete_one{from_v1(delete_one_value)},
        v_noabi::model::delete_many{from_v1(delete_many_value)},
        v_noabi::model::update_one{from_v1(update_one_value), bsoncxx::v_noabi::document::view{}},
        v_noabi::model::update_many{from_v1(update_many_value), bsoncxx::v_noabi::document::view{}},
        v_noabi::model::replace_one{from_v1(replace_one_value), bsoncxx::v_noabi::document::view{}},
    }};

    std::array<v_noabi::model::write, size> const sources_v_noabi_copy = {{
        v_noabi::model::insert_one{from_v1(insert_one_value)},
        v_noabi::model::delete_one{from_v1(delete_one_value)},
        v_noabi::model::delete_many{from_v1(delete_many_value)},
        v_noabi::model::update_one{from_v1(update_one_value), bsoncxx::v_noabi::document::view{}},
        v_noabi::model::update_many{from_v1(update_many_value), bsoncxx::v_noabi::document::view{}},
        v_noabi::model::replace_one{from_v1(replace_one_value), bsoncxx::v_noabi::document::view{}},
    }};

    auto& value_v1 = sources_v1[value_type];
    auto& value_v_noabi = sources_v_noabi[value_type];
    auto const& value_v_noabi_copy = sources_v_noabi_copy[value_type];

    REQUIRE(value_v1.type() == static_cast<v1::bulk_write::type>(value_type));
    REQUIRE(value_v_noabi.type() == static_cast<v_noabi::write_type>(value_type));
    REQUIRE(value_v_noabi.type() == value_v_noabi_copy.type());

    auto const get_value_v1 = [](v1::bulk_write::single const& op) -> bsoncxx::v_noabi::document::view {
        switch (op.type()) {
            case v1::bulk_write::type::k_insert_one:
                return op.get_insert_one().document().view();
            case v1::bulk_write::type::k_update_one:
                return op.get_update_one().filter();
            case v1::bulk_write::type::k_update_many:
                return op.get_update_many().filter();
            case v1::bulk_write::type::k_delete_one:
                return op.get_delete_one().filter();
            case v1::bulk_write::type::k_delete_many:
                return op.get_delete_many().filter();
            case v1::bulk_write::type::k_replace_one:
                return op.get_replace_one().filter();
            default:
                FAIL("should not reach this point");
                return {};
        }
    };

    auto const get_value_v_noabi = [](v_noabi::model::write const& op) -> bsoncxx::v_noabi::document::view {
        switch (op.type()) {
            case v_noabi::write_type::k_insert_one:
                return op.get_insert_one().document();
            case v_noabi::write_type::k_update_one:
                return op.get_update_one().filter();
            case v_noabi::write_type::k_update_many:
                return op.get_update_many().filter();
            case v_noabi::write_type::k_delete_one:
                return op.get_delete_one().filter();
            case v_noabi::write_type::k_delete_many:
                return op.get_delete_many().filter();
            case v_noabi::write_type::k_replace_one:
                return op.get_replace_one().filter();
            default:
                FAIL("should not reach this point");
                return {};
        }
    };

    REQUIRE(get_value_v_noabi(value_v_noabi) == get_value_v1(value_v1));
    REQUIRE(get_value_v_noabi(value_v_noabi) == get_value_v_noabi(value_v_noabi_copy));

    using v_noabi = v_noabi::model::write;
    using v1 = v1::bulk_write::single;

    SECTION("from_v1") {
        v_noabi from{std::move(value_v_noabi)};

        SECTION("move") {
            v1 const to{std::move(from)};

            CHECK(get_value_v1(to) == get_value_v_noabi(value_v_noabi_copy));
        }

        SECTION("copy") {
            v1 const to{from};

            CHECK(get_value_v1(to) == get_value_v_noabi(value_v_noabi_copy));
        }
    }

    SECTION("to_v1") {
        v1 from{value_v1};

        SECTION("move") {
            v_noabi const to{std::move(from)};

            CHECK(get_value_v_noabi(to) == get_value_v1(value_v1));
        }

        SECTION("copy") {
            v_noabi const to{from};

            CHECK(get_value_v_noabi(to) == get_value_v1(value_v1));
        }
    }
}

} // namespace mongocxx
