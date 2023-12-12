#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/type_traits.hpp>
#include <third_party/catch/include/catch.hpp>

using bsoncxx::stdx::nullopt;
using bsoncxx::stdx::optional;

namespace {

template <typename T>
using deref_t = decltype(*std::declval<T>());

template <typename T>
using value_t = decltype(std::declval<T>().value());

template <typename T>
using arrow_t = decltype(std::declval<T>().operator->());

struct not_default_constructible {
    explicit not_default_constructible(int);
};

struct allows_moving_explicit_conversion {
    // Requires an rvalue-reference to an unalike type
    explicit allows_moving_explicit_conversion(int&&) {}
};

struct allows_moving_implicit_conversion {
    // Requires an rvalue-reference to an unalike type
    allows_moving_implicit_conversion(int&&) {}
};

struct immobile {
    std::mutex mtx;
};

template <int>
struct noncopyable_item {
    int value;

    template <int N>
    bool operator==(const noncopyable_item<N>& o) const noexcept {
        return value == o.value;
    }
};

struct not_ordered {};

struct not_copyable {
    not_copyable(not_copyable&&) = default;
    not_copyable& operator=(not_copyable&&) = default;
};

template <bsoncxx_ttparam Trait, typename T>
bool assert_sameness() {
    static_assert(Trait<T>::value == Trait<optional<T>>::value, "Fail");
    return true;
}

template <typename From1, typename To1, typename From2, typename To2>
bool check_convert_alike() {
    static_assert(std::is_convertible<From1, To1>::value  //
                      == std::is_convertible<From2, To2>::value,
                  "fail");
    static_assert(std::is_convertible<From1&, To1>::value  //
                      == std::is_convertible<From2&, To2>::value,
                  "fail");
    static_assert(std::is_convertible<From1 const&, To1>::value  //
                      == std::is_convertible<From2 const&, To2>::value,
                  "fail");
    static_assert(std::is_convertible<From1&&, To1>::value  //
                      == std::is_convertible<From2&&, To2>::value,
                  "fail");
    static_assert(std::is_convertible<From1 const&&, To1>::value  //
                      == std::is_convertible<From2 const&&, To2>::value,
                  "fail");
    return true;
}

template <typename From1, typename To1, typename From2, typename To2>
bool check_construct_alike() {
    static_assert(std::is_constructible<To1, From1>::value  //
                      == std::is_constructible<To2, From2>::value,
                  "fail");
    static_assert(std::is_constructible<To1, From1&>::value  //
                      == std::is_constructible<To2, From2&>::value,
                  "fail");
    static_assert(std::is_constructible<To1, From1 const&>::value  //
                      == std::is_constructible<To2, From2 const&>::value,
                  "fail");
    static_assert(std::is_constructible<To1, From1&&>::value  //
                      == std::is_constructible<To2, From2&&>::value,
                  "fail");
    static_assert(std::is_constructible<To1, From1 const&&>::value  //
                      == std::is_constructible<To2, From2 const&&>::value,
                  "fail");
    return true;
}

template <typename From, typename To>
bool check_conversions() {
    return check_convert_alike<From, To, optional<From>, optional<To>>() &&
           check_convert_alike<From, To, From, optional<To>>() &&
           check_construct_alike<From, To, optional<From>, optional<To>>() &&
           check_construct_alike<From, To, From, optional<To>>();
}

template <typename T>
bool static_checks() {
    assert_sameness<std::is_copy_constructible, T>();
    assert_sameness<std::is_copy_assignable, T>();
    assert_sameness<std::is_move_constructible, T>();
    assert_sameness<std::is_move_assignable, T>();
    assert_sameness<bsoncxx::detail::is_equality_comparable, T>();
    assert_sameness<bsoncxx::detail::is_totally_ordered, T>();
    static_assert(bsoncxx::detail::is_equality_comparable<T, optional<T>>::value ==
                      bsoncxx::detail::is_equality_comparable<T>::value,
                  "fail");
    static_assert(bsoncxx::detail::is_totally_ordered_with<T, optional<T>>::value ==
                      bsoncxx::detail::is_totally_ordered<optional<T>>::value,
                  "fail");
    static_assert(
        std::is_constructible<optional<T>, T>::value == std::is_constructible<T, T>::value, "fail");
    static_assert(std::is_constructible<optional<T>, bsoncxx::stdx::nullopt_t>{}, "fail");
    // Assert we return proper reference types
    static_assert(std::is_same<deref_t<optional<T>>, T&&>{}, "fail");
    static_assert(std::is_same<deref_t<optional<T> const>, const T&&>{}, "fail");
    static_assert(std::is_same<deref_t<optional<T> const&>, const T&>{}, "fail");
    static_assert(std::is_same<deref_t<optional<T>&>, T&>{}, "fail");
    // .value()
    static_assert(std::is_same<value_t<optional<T>>, T&&>{}, "fail");
    static_assert(std::is_same<value_t<optional<T> const>, const T&&>{}, "fail");
    static_assert(std::is_same<value_t<optional<T> const&>, const T&>{}, "fail");
    static_assert(std::is_same<value_t<optional<T>&>, T&>{}, "fail");
    // operator->
    static_assert(std::is_same<arrow_t<optional<T>>, T*>{}, "fail");
    static_assert(std::is_same<arrow_t<optional<T> const>, const T*>{}, "fail");
    static_assert(std::is_same<arrow_t<optional<T> const&>, const T*>{}, "fail");
    static_assert(std::is_same<arrow_t<optional<T>&>, T*>{}, "fail");
    return check_conversions<T, T>();
}

}  // namespace

static_assert(bsoncxx::detail::is_totally_ordered<std::string>{}, "fail");
static_assert(bsoncxx::detail::is_totally_ordered<int>{}, "fail");
static_assert(!bsoncxx::detail::is_totally_ordered<not_ordered>{}, "fail");

TEST_CASE("Trait checks") {
    CHECK(static_checks<int>());
    CHECK(static_checks<std::string>());
    CHECK(static_checks<std::unique_ptr<int>>());
    CHECK(static_checks<immobile>());
    CHECK(static_checks<not_copyable>());
    CHECK(static_checks<not_default_constructible>());
    CHECK(static_checks<allows_moving_explicit_conversion>());
    CHECK(static_checks<allows_moving_implicit_conversion>());
    CHECK(check_conversions<int, int>());
    CHECK(check_conversions<int, double>());
    CHECK(check_conversions<double, std::string>());
    CHECK(check_conversions<std::string, double>());
    CHECK(check_conversions<std::string, const char*>());
    CHECK(check_conversions<const char*, std::string>());
    CHECK(check_conversions<int*, std::unique_ptr<int>>());
    CHECK(check_conversions<int, allows_moving_explicit_conversion>());
    CHECK(check_conversions<int, allows_moving_implicit_conversion>());
    optional<int> v(21);
    CHECK(*v == 21);
    CHECK_FALSE(v < v);
}

TEST_CASE("optional: Nontrivial contents") {
    optional<std::string> str = "abcd1234";
    CHECK(str == "abcd1234");
    {
        auto dup = str;
        bool b = dup == str;
        (void)b;
        CHECK(dup == str);
    }
    CHECK(str == "abcd1234");

    optional<std::unique_ptr<int>> aptr;
    CHECK(aptr != nullptr);
    CHECK(aptr == bsoncxx::stdx::nullopt);
    {
        auto dup = std::move(aptr);
        CHECK(aptr == dup);
    }
    aptr = bsoncxx::stdx::make_unique<int>(31);
    CHECK(aptr != nullopt);
    REQUIRE(aptr != nullptr);
    CHECK(**aptr == 31);
    {
        auto dup = std::move(aptr);
        CHECK(aptr);
        CHECK(aptr == nullptr);
        REQUIRE(dup);
        REQUIRE(dup != nullptr);
        CHECK(**dup == 31);
    }
    CHECK(aptr == nullptr);
}

TEST_CASE("Comparisons") {
    optional<int> a = 21;
    optional<int> b = 23;
    optional<int> c;
    CHECK(a != nullopt);
    CHECK(a == 21);
    CHECK(b != nullopt);
    CHECK(b == 23);
    CHECK(c == nullopt);
    // Null compares less-than values:
    CHECK(c < 42);
    CHECK(a < b);
    CHECK(c < a);
    CHECK(c < b);
    CHECK(a != b);
    CHECK(c != a);
    CHECK(c != b);
    CHECK(c == c);

    optional<double> dbl = 3.14;
    CHECK(dbl != a);
}

struct in_place_convertible {
    bool constructed_from_in_place = false;
    in_place_convertible() = default;
    in_place_convertible(bsoncxx::stdx::in_place_t) : constructed_from_in_place(true) {}
};

TEST_CASE("optional<T> conversions") {
    static_assert(!std::is_constructible<optional<bool>, optional<std::string>>{}, "fail");

    optional<std::string> s1(bsoncxx::stdx::in_place);
    CHECK(s1 == "");

    optional<in_place_convertible> q(bsoncxx::stdx::in_place);
    REQUIRE(q.has_value());
    CHECK_FALSE(q->constructed_from_in_place);
}
