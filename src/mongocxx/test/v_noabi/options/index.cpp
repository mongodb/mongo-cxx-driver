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

#include <mongocxx/options/index.hh>

//

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/private/make_unique.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <bsoncxx/test/catch.hh>

#include <catch2/generators/catch_generators.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("index", "[index][option]") {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    options::index idx;
    auto storage = bsoncxx::from_json(R"({"wiredTiger": {"configString": null}})");

    auto idx_as_doc = [&idx] { return static_cast<bsoncxx::document::view_or_value>(idx); };

    auto collation = make_document(kvp("locale", "en_US"));
    auto partial_filter_expression = make_document(kvp("x", true));
    auto weights = make_document(kvp("a", 100));

    SECTION("check options applied") {
        CHECK_OPTIONAL_ARGUMENT(idx, background, true);
        CHECK_OPTIONAL_ARGUMENT(idx, unique, true);
        CHECK_OPTIONAL_ARGUMENT(idx, name, "name");
        CHECK_OPTIONAL_ARGUMENT(idx, collation, collation.view());
        CHECK_OPTIONAL_ARGUMENT(idx, sparse, true);
        CHECK_OPTIONAL_ARGUMENT(idx, expire_after, std::chrono::seconds(3600));
        CHECK_OPTIONAL_ARGUMENT(idx, version, 540);
        CHECK_OPTIONAL_ARGUMENT(idx, default_language, "en");
        CHECK_OPTIONAL_ARGUMENT(idx, language_override, "lang");
        CHECK_OPTIONAL_ARGUMENT(idx, twod_sphere_version, 4);
        CHECK_OPTIONAL_ARGUMENT(idx, twod_bits_precision, 4);
        CHECK_OPTIONAL_ARGUMENT(idx, twod_location_min, 90.0);
        CHECK_OPTIONAL_ARGUMENT(idx, twod_location_max, 90.0);
        CHECK_OPTIONAL_ARGUMENT(idx, haystack_bucket_size_deprecated, 90.0);
        CHECK_OPTIONAL_ARGUMENT(idx, weights, weights.view());
        CHECK_OPTIONAL_ARGUMENT(idx, partial_filter_expression, partial_filter_expression.view());
        CHECK_OPTIONAL_ARGUMENT(idx, storage_engine, storage.view());
    }

    SECTION("check cast to document") {
        using namespace bsoncxx::types;

        bsoncxx::builder::basic::document options_doc;
        options_doc.append(kvp("name", b_string{"name"}));
        options_doc.append(kvp("background", b_bool{false}));
        options_doc.append(kvp("unique", b_bool{true}));
        options_doc.append(kvp("partialFilterExpression", partial_filter_expression.view()));
        options_doc.append(kvp("sparse", b_bool{false}));
        options_doc.append(kvp("expireAfterSeconds", b_int32{3600}));
        options_doc.append(kvp("weights", weights.view()));
        options_doc.append(kvp("default_language", "en"));
        options_doc.append(kvp("language_override", "lang"));
        options_doc.append(kvp("2dsphereIndexVersion", b_int32{4}));
        options_doc.append(kvp("bits", b_int32{4}));
        options_doc.append(kvp("min", b_double{90.0}));
        options_doc.append(kvp("max", b_double{90.0}));
        options_doc.append(kvp("bucketSize", b_double{90.0}));
        options_doc.append(kvp("collation", collation.view()));
        options_doc.append(
            kvp("storageEngine",
                make_document(kvp("wiredTiger", make_document(kvp("configString", bsoncxx::types::b_null{}))))));

        idx.name("name");
        idx.background(false);
        idx.unique(true);
        idx.partial_filter_expression(partial_filter_expression.view());
        idx.sparse(false);
        idx.expire_after(std::chrono::seconds(3600));
        idx.collation(collation.view());
        idx.version(540);
        idx.default_language("en");
        idx.language_override("lang");
        idx.twod_sphere_version(4);
        idx.twod_bits_precision(4);
        idx.twod_location_max(90.0);
        idx.twod_location_min(90.0);
        idx.haystack_bucket_size_deprecated(90.0);
        idx.weights(weights.view());
        idx.storage_engine(storage.view());

        auto doc = idx_as_doc();

        REQUIRE(doc.view().length() == options_doc.view().length());
        REQUIRE(doc.view() == options_doc.view());
    }

    SECTION("storage_engine overrides storage_options when set") {
        auto engine = bsoncxx::from_json(R"({"wiredTiger": {"configString": "override"}})");
        auto options = bsoncxx::make_unique<options::index::wiredtiger_storage_options>(); // configString: null

        CHECK(idx_as_doc() == bsoncxx::from_json(R"({})"));

        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
        idx.storage_options(std::move(options));
        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

        CHECK(idx_as_doc() == bsoncxx::from_json(R"({"storageEngine": {"wiredTiger": {"configString": null}}})"));

        idx.storage_engine(engine.view());

        CHECK(idx_as_doc() == make_document(kvp("storageEngine", engine.view())));
    }
}
} // namespace

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][index]") {
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<bool> background;
    bsoncxx::v1::stdx::optional<bool> unique;
    bsoncxx::v1::stdx::optional<bool> hidden;
    bsoncxx::v1::stdx::optional<std::string> name;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> collation;
    bsoncxx::v1::stdx::optional<bool> sparse;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> storage_engine;
    bsoncxx::v1::stdx::optional<std::chrono::seconds> expire_after;
    bsoncxx::v1::stdx::optional<std::int32_t> version;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> weights;
    bsoncxx::v1::stdx::optional<std::string> default_language;
    bsoncxx::v1::stdx::optional<std::string> language_override;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> partial_filter_expression;
    bsoncxx::v1::stdx::optional<std::uint8_t> twod_sphere_version;
    bsoncxx::v1::stdx::optional<std::uint8_t> twod_bits_precision;
    bsoncxx::v1::stdx::optional<double> twod_location_min;
    bsoncxx::v1::stdx::optional<double> twod_location_max;

    if (has_value) {
        background.emplace();
        unique.emplace();
        hidden.emplace();
        name.emplace();
        collation.emplace();
        sparse.emplace();
        storage_engine.emplace();
        expire_after.emplace();
        version.emplace();
        weights.emplace();
        default_language.emplace();
        language_override.emplace();
        partial_filter_expression.emplace();
        twod_sphere_version.emplace();
        twod_bits_precision.emplace();
        twod_location_min.emplace();
        twod_location_max.emplace();
    }

    using bsoncxx::v_noabi::from_v1;
    using mongocxx::v_noabi::from_v1;
    using mongocxx::v_noabi::to_v1;

    using v_noabi = v_noabi::options::index;
    using v1 = v1::indexes::options;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.background(*background);
            from.unique(*unique);
            from.hidden(*hidden);
            from.name(*name);
            from.collation(*collation);
            from.sparse(*sparse);
            from.storage_engine(*storage_engine);
            from.expire_after(*expire_after);
            from.version(*version);
            from.weights(*weights);
            from.default_language(*default_language);
            from.language_override(*language_override);
            from.partial_filter_expression(*partial_filter_expression);
            from.twod_sphere_version(*twod_sphere_version);
            from.twod_bits_precision(*twod_bits_precision);
            from.twod_location_min(*twod_location_min);
            from.twod_location_max(*twod_location_max);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.background() == *background);
            CHECK(to.unique() == *unique);
            CHECK(to.hidden() == *hidden);
            CHECK(to.name() == *name);
            CHECK(to.collation() == collation->view());
            CHECK(to.sparse() == *sparse);
            CHECK(to.storage_engine() == storage_engine->view());
            CHECK(to.expire_after() == *expire_after);
            CHECK(to.version() == *version);
            CHECK(to.weights() == weights->view());
            CHECK(to.default_language() == *default_language);
            CHECK(to.language_override() == *language_override);
            CHECK(to.partial_filter_expression() == partial_filter_expression->view());
            CHECK(to.twod_sphere_version() == *twod_sphere_version);
            CHECK(to.twod_bits_precision() == *twod_bits_precision);
            CHECK(to.twod_location_min() == *twod_location_min);
            CHECK(to.twod_location_max() == *twod_location_max);
        } else {
            CHECK_FALSE(to.background().has_value());
            CHECK_FALSE(to.unique().has_value());
            CHECK_FALSE(to.hidden().has_value());
            CHECK_FALSE(to.name().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.sparse().has_value());
            CHECK_FALSE(to.storage_engine().has_value());
            CHECK_FALSE(to.expire_after().has_value());
            CHECK_FALSE(to.version().has_value());
            CHECK_FALSE(to.weights().has_value());
            CHECK_FALSE(to.default_language().has_value());
            CHECK_FALSE(to.language_override().has_value());
            CHECK_FALSE(to.partial_filter_expression().has_value());
            CHECK_FALSE(to.twod_sphere_version().has_value());
            CHECK_FALSE(to.twod_bits_precision().has_value());
            CHECK_FALSE(to.twod_location_min().has_value());
            CHECK_FALSE(to.twod_location_max().has_value());
        }

        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
        CHECK_FALSE(v_noabi::internal::storage_options(to));
        CHECK_FALSE(to.haystack_bucket_size().has_value());
        BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.background(*background);
            from.unique(*unique);
            from.hidden(*hidden);
            from.name(*name);
            from.collation(from_v1(collation->view()));
            from.sparse(*sparse);
            from.storage_engine(from_v1(storage_engine->view()));
            from.expire_after(*expire_after);
            from.version(*version);
            from.weights(from_v1(weights->view()));
            from.default_language(*default_language);
            from.language_override(*language_override);
            from.partial_filter_expression(from_v1(partial_filter_expression->view()));
            from.twod_sphere_version(*twod_sphere_version);
            from.twod_bits_precision(*twod_bits_precision);
            from.twod_location_min(*twod_location_min);
            from.twod_location_max(*twod_location_max);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.background() == *background);
            CHECK(to.unique() == *unique);
            CHECK(to.hidden() == *hidden);
            CHECK(to.name() == *name);
            CHECK(to.collation() == collation->view());
            CHECK(to.sparse() == *sparse);
            CHECK(to.storage_engine() == storage_engine->view());
            CHECK(to.expire_after() == *expire_after);
            CHECK(to.version() == *version);
            CHECK(to.weights() == weights->view());
            CHECK(to.default_language() == *default_language);
            CHECK(to.language_override() == *language_override);
            CHECK(to.partial_filter_expression() == partial_filter_expression->view());
            CHECK(to.twod_sphere_version() == *twod_sphere_version);
            CHECK(to.twod_bits_precision() == *twod_bits_precision);
            CHECK(to.twod_location_min() == *twod_location_min);
            CHECK(to.twod_location_max() == *twod_location_max);
        } else {
            CHECK_FALSE(to.background().has_value());
            CHECK_FALSE(to.unique().has_value());
            CHECK_FALSE(to.hidden().has_value());
            CHECK_FALSE(to.name().has_value());
            CHECK_FALSE(to.collation().has_value());
            CHECK_FALSE(to.sparse().has_value());
            CHECK_FALSE(to.storage_engine().has_value());
            CHECK_FALSE(to.expire_after().has_value());
            CHECK_FALSE(to.version().has_value());
            CHECK_FALSE(to.weights().has_value());
            CHECK_FALSE(to.default_language().has_value());
            CHECK_FALSE(to.language_override().has_value());
            CHECK_FALSE(to.partial_filter_expression().has_value());
            CHECK_FALSE(to.twod_sphere_version().has_value());
            CHECK_FALSE(to.twod_bits_precision().has_value());
            CHECK_FALSE(to.twod_location_min().has_value());
            CHECK_FALSE(to.twod_location_max().has_value());
        }

        CHECK_FALSE(to.text_index_version().has_value());
        CHECK_FALSE(to.wildcard_projection().has_value());
    }
}

} // namespace mongocxx
