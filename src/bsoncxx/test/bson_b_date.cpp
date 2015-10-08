#include "catch.hpp"

#include <bsoncxx/types.hpp>
#include <chrono>

TEST_CASE("time_point is converted to b_date and back", "[bsoncxx::types::b_date]") {
    using bsoncxx::types::b_date;
    using std::chrono::system_clock;
    using std::chrono::time_point_cast;
    using std::chrono::milliseconds;

    system_clock::time_point now1, now2;

    now1 = system_clock::now();
    b_date d{now1};
    now2 = d;

    REQUIRE(time_point_cast<milliseconds>(now1) == time_point_cast<milliseconds>(now2));
}

TEST_CASE("time_point is converted to b_date consistently", "[bsoncxx::types::b_date]") {
    using bsoncxx::types::b_date;
    using std::chrono::system_clock;

    system_clock::time_point now;

    now = system_clock::now();
    b_date d1{now};
    b_date d2{now};

    REQUIRE(d1.value == d2.value);
}
