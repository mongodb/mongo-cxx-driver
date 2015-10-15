#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/stdx.hpp>

using namespace mongocxx;
using namespace mongocxx::options;

TEST_CASE("index", "[index][option]") {
    options::index idx;
    std::unique_ptr<index::wiredtiger_storage_options> storage =
        stdx::make_unique<index::wiredtiger_storage_options>();

    CHECK_OPTIONAL_ARGUMENT(idx, background, true);
    CHECK_OPTIONAL_ARGUMENT(idx, unique, true);
    CHECK_OPTIONAL_ARGUMENT(idx, name, "name");
    CHECK_OPTIONAL_ARGUMENT(idx, sparse, true);
    CHECK_OPTIONAL_ARGUMENT(idx, expire_after_seconds, 3600);
    CHECK_OPTIONAL_ARGUMENT(idx, version, 540);
    CHECK_OPTIONAL_ARGUMENT(idx, default_language, "en");
    CHECK_OPTIONAL_ARGUMENT(idx, language_override, "lang");
    CHECK_OPTIONAL_ARGUMENT(idx, twod_sphere_version, 4);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_bits_precision, 4);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_location_min, 90.0);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_location_max, 90.0);
    CHECK_OPTIONAL_ARGUMENT(idx, haystack_bucket_size, 90.0);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(idx, weights, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(idx, partial_filter_expression,
                                             bsoncxx::document::view{});
    REQUIRE_NOTHROW(idx.storage_options(std::move(storage)));
}
