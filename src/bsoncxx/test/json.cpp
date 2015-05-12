#include "catch.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

namespace {
    const auto k_invalid_json = R"({])";
    const auto k_valid_json = R"({ "a" : 1, "b" : 2.0 })";
}

TEST_CASE("invalid json returns disengaged optional") {
    using namespace bsoncxx;
    REQUIRE(!from_json(k_invalid_json));
}

TEST_CASE("valid json returns an engaged optional") {
    using namespace bsoncxx;
    REQUIRE(from_json(k_valid_json));
}

TEST_CASE("valid json is converted to equivalent BSON") {
    using namespace bsoncxx;

    const auto expected = builder::stream::document{} << "a" << 1 << "b" << 2.0 << builder::stream::finalize;
    const auto expected_view = expected.view();

    const auto actual = from_json(k_valid_json);
    const auto actual_view = actual->view();

    REQUIRE(expected_view.length() == actual_view.length());
    REQUIRE(0 == memcmp(expected_view.data(), actual_view.data(), expected_view.length()));
}
