#include <cstddef>
#include <deque>
#include <vector>

#include <bsoncxx/stdx/iterator.hpp>
#include <bsoncxx/stdx/ranges.hpp>
#include <third_party/catch/include/catch.hpp>

namespace ranges = bsoncxx::detail;

static_assert(ranges::is_range<std::vector<int>>{}, "fail");
static_assert(ranges::is_contiguous_range<std::vector<int>>{}, "fail");
static_assert(ranges::is_contiguous_range<std::vector<int>&>{}, "fail");
static_assert(ranges::is_contiguous_range<std::vector<int> const&>{}, "fail");

static_assert(
    std::is_same<ranges::range_difference_t<std::vector<int>>, std::vector<int>::difference_type>{},
    "fail");

static_assert(ranges::is_range<int (&)[2]>{}, "fail");
static_assert(ranges::is_contiguous_range<int (&)[2]>{}, "fail");
static_assert(std::is_same<ranges::range_difference_t<int[21]>, std::ptrdiff_t>{}, "fail");

static_assert(ranges::is_range<std::deque<int>>{}, "fail");
static_assert(!ranges::is_contiguous_range<std::deque<int>>{}, "fail");

TEST_CASE("Range from vector") {
    std::vector<int> nums = {1, 2, 3, 4};
    auto it = ranges::begin(nums);
    REQUIRE(it != ranges::end(nums));
    static_assert(std::is_same<decltype(it), std::vector<int>::iterator>::value, "fail");
    CHECK(*it == 1);
    ++it;
    CHECK(it == std::next(ranges::begin(nums)));
    CHECK(*it == 2);
    CHECK(ranges::size(nums) == 4);
    CHECK(ranges::ssize(nums) == 4);
    CHECK(ranges::data(nums) == nums.data());
    CHECK(ranges::to_address(ranges::begin(nums)) == nums.data());
}

TEST_CASE("Range from array") {
    int array[] = {1, 2, 3, 4};
    auto it = ranges::begin(array);
    CHECK(it == array + 0);
    CHECK(ranges::end(array) == array + 4);
    CHECK(ranges::size(array) == 4);
    CHECK(ranges::ssize(array) == 4);
    CHECK(ranges::data(array) == ranges::begin(array));
}