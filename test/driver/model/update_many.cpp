#include "catch.hpp"
#include "helpers.hpp"

#include "driver/model/update_many.hpp"

using namespace mongo::driver;

TEST_CASE("update_many", "[update_many][model]") {
    const bson::document::view a((std::uint8_t *)"", 0);
    const bson::document::view b((std::uint8_t *)"", 0);

    model::update_many um(a, b);

    SECTION("stores required arguments") {
        REQUIRE(um.filter().get_buf() == a.get_buf());
        REQUIRE(um.update().get_buf() == b.get_buf());
    }

    CHECK_OPTIONAL_ARGUMENT(um, upsert, true)
}
