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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/options/index.hpp>

#include <bsoncxx/private/make_unique.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <bsoncxx/test/catch.hh>

#include <mongocxx/test/catch_helpers.hh>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("index", "[index][option]") {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    instance::current();

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
