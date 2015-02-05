#include "catch.hpp"
#include "helpers.hpp"

#include <mongo/driver/options/aggregate.hpp>

using namespace mongo::driver;

TEST_CASE("aggregate", "[aggregate][option]") {
    options::aggregate agg;

    CHECK_OPTIONAL_ARGUMENT(agg, allow_disk_use, true);
    CHECK_OPTIONAL_ARGUMENT(agg, batch_size, 500);
    CHECK_OPTIONAL_ARGUMENT(agg, max_time_ms, 1000);
    CHECK_OPTIONAL_ARGUMENT(agg, use_cursor, true);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(agg, read_preference, read_preference{});
}
