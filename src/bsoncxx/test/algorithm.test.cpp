#include <iterator>
#include <type_traits>
#include <vector>

#include <bsoncxx/stdx/algorithm.hpp>
#include <bsoncxx/stdx/iterator.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/ranges.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/stdx/type_traits.hpp>
#include <third_party/catch/include/catch.hpp>

namespace ranges = bsoncxx::detail;

TEST_CASE("Algorithm: Advance an iterator") {
    int arr[] = {1, 2, 3};
    auto it = ranges::begin(arr);
    auto dup = it;
    CHECK(it == dup);
    CHECK(*it == 1);
    CHECK(ranges::advance(dup, bsoncxx::detail::unreachable_sentinel) == 1);
    CHECK(*dup == 2);
    CHECK(dup == ranges::next(it, bsoncxx::detail::unreachable_sentinel));
    dup = it;
    CHECK(ranges::advance(dup, 2, bsoncxx::detail::unreachable_sentinel) == 2);
    CHECK(*dup == 3);
    CHECK(dup == ranges::next(it, 2, bsoncxx::detail::unreachable_sentinel));
    dup = it;
    CHECK(ranges::advance(dup, 400, ranges::end(arr)) == 3);
    CHECK(dup == ranges::end(arr));
    CHECK(dup == ranges::next(it, 3, bsoncxx::detail::unreachable_sentinel));
}

TEST_CASE("Algorithm: equal()") {
    std::vector<int> a = {1, 2, 3, 4, 5};
    std::vector<int> b = {1, 2, 3, 4, 5};
    CHECK(ranges::equal(a, b));
}

TEST_CASE("Algorithm: Simple search") {
    auto arr = {1, 2, 3, 4};
    auto needle = {2, 3};
    auto searcher =
        bsoncxx::detail::default_searcher<const int*, const int*, bsoncxx::detail::equal_to>{
            bsoncxx::detail::begin(needle), bsoncxx::detail::end(needle), {}};
    auto found = searcher(bsoncxx::detail::begin(arr), bsoncxx::detail::end(arr));
    CHECK(found.first == bsoncxx::detail::begin(arr) + 1);
    auto found2 = bsoncxx::detail::search(arr, needle);
    CHECK(found2.begin() == bsoncxx::detail::begin(arr) + 1);
}
