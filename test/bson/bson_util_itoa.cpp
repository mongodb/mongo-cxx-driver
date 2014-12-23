#include "catch.hpp"

#include "bson/util/itoa.hpp"

TEST_CASE("util::itoa is equivalent to to_string(int)", "[bson::util::itoa]") {
    using namespace bson;

    for (int i = 0; i <= 10000; i++) {
        util::itoa val(i);
        std::string str = std::to_string(i);
        REQUIRE(val.length() == str.length());
        REQUIRE(std::string(val.c_str()) == str);
    }
}
