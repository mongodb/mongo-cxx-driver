#include "catch.hpp"

#include <bsoncxx/private/itoa.hpp>

TEST_CASE("util::itoa is equivalent to to_string(int)", "[bsoncxx::util::itoa]") {
    using namespace bsoncxx;

    for (int i = 0; i <= 10000; i++) {
        itoa val(i);
        std::string str = std::to_string(i);
        REQUIRE(val.length() == str.length());
        REQUIRE(std::string(val.c_str()) == str);
    }
}
