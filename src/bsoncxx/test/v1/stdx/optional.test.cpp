#include <bsoncxx/stdx/optional.hpp>
//
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <bsoncxx/test/catch.hh>

// Each polyfill library has some set of features that are not conformant with the standard
// specification (inconsistent, missing, etc.). Limit testing to bsoncxx implementation and stdlib.
#if defined(BSONCXX_POLY_USE_IMPLS) || defined(BSONCXX_POLY_USE_STD)

using bsoncxx::stdx::in_place;
using bsoncxx::stdx::nullopt;
using bsoncxx::stdx::optional;

#if defined(_MSC_VER) && _MSC_VER < 1910 || defined(__apple_build_version__)
/// ! Prior to LWG 2543, uttering the name of an invalid std::hash is ill-formed.
/// ! This is fixed in C++11, but MSVC 2015 (19.00) and old AppleClang libc++ don't
/// !have the fix. As such, one cannot detect whether a type is hashable.
#define NO_LWG_2543
#endif

// Catch2 has trouble understanding ADL-only comparison operators defined via `equality_operators`
// and `tag_invoke` when decomposing expressions with VS 2017 despite being valid in normal code.
// Move the comparison expression out of the Catch2 macro as a workaround to avoid decomposition.
#if defined(_MSC_VER) && _MSC_VER < 1920
#define REQUIRE_VS2017(...)             \
    if (1) {                            \
        const bool res = (__VA_ARGS__); \
        CHECK(res);                     \
    } else                              \
        ((void)0)
#define CHECK_VS2017(...)               \
    if (1) {                            \
        const bool res = (__VA_ARGS__); \
        CHECK(res);                     \
    } else                              \
        ((void)0)
#define CHECK_FALSE_VS2017(...)         \
    if (1) {                            \
        const bool res = (__VA_ARGS__); \
        CHECK_FALSE(res);               \
    } else                              \
        ((void)0)
#else
#define REQUIRE_VS2017(...) REQUIRE(__VA_ARGS__)
#define CHECK_VS2017(...) CHECK(__VA_ARGS__)
#define CHECK_FALSE_VS2017(...) CHECK_FALSE(__VA_ARGS__)
#endif

namespace {

template <typename T>
using deref_t = decltype(*std::declval<T>());

template <typename T>
using value_t = decltype(std::declval<T>().value());

template <typename T>
using arrow_t = decltype(std::declval<T>().operator->());

#ifndef NO_LWG_2543
template <typename T, typename Td = bsoncxx::detail::remove_const_t<T>>
struct is_hashable : bsoncxx::detail::conjunction<
                         std::is_default_constructible<std::hash<Td>>,
                         bsoncxx::detail::is_invocable<std::hash<Td>, T const&>> {};
#else

template <typename T>
struct is_hashable : std::false_type {};
#endif

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wunused"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunused-member-function"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunneeded-member-function"));

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

struct not_ordered {};

struct not_copyable {
    not_copyable(not_copyable&&) = default;
    not_copyable& operator=(not_copyable&&) = default;
};

BSONCXX_PRIVATE_WARNINGS_POP();

// Improve quality of error messages on failure (in particular for MSVC).
#define STATIC_ASSERT_EXPR(expr) static_assert((expr), "expected: " #expr)
#define STATIC_ASSERT_EXPR_EQUAL(a, b) static_assert((a) == (b), "expected: " #a " == " #b)

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wunused-macros"));
#define STATIC_ASSERT_EXPR_IMPLIES(a, b) static_assert((!(a) || (b)), "expected: " #a " -> " #b)
BSONCXX_PRIVATE_WARNINGS_POP();

#if defined(BSONCXX_POLY_USE_STD)
// Deliberately weaken assertions for stdlib implementations to accomodate for differences in
// behavior.
#define STATIC_ASSERT_EXPR_ALIKE(a, b) STATIC_ASSERT_EXPR_IMPLIES(a, b)
#else
#define STATIC_ASSERT_EXPR_ALIKE(a, b) STATIC_ASSERT_EXPR_EQUAL(a, b)
#endif

template <template <class...> class Trait, typename T>
bool assert_alikeness() {
    STATIC_ASSERT_EXPR_ALIKE(Trait<T>::value, Trait<optional<T>>::value);
    return true;
}

template <typename From1, typename To1, typename From2, typename To2>
bool check_convert_alike() {
    STATIC_ASSERT_EXPR_ALIKE((std::is_convertible<From1, To1>::value), (std::is_convertible<From2, To2>::value));
    STATIC_ASSERT_EXPR_ALIKE((std::is_convertible<From1&, To1>::value), (std::is_convertible<From2&, To2>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (std::is_convertible<From1 const&, To1>::value), (std::is_convertible<From2 const&, To2>::value));
    STATIC_ASSERT_EXPR_ALIKE((std::is_convertible<From1&&, To1>::value), (std::is_convertible<From2&&, To2>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (std::is_convertible<From1 const&&, To1>::value), (std::is_convertible<From2 const&&, To2>::value));
    return true;
}

template <typename From1, typename To1, typename From2, typename To2>
bool check_construct_alike() {
    STATIC_ASSERT_EXPR_ALIKE((std::is_constructible<To1, From1>::value), (std::is_constructible<To2, From2>::value));
    STATIC_ASSERT_EXPR_ALIKE((std::is_constructible<To1, From1&>::value), (std::is_constructible<To2, From2&>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (std::is_constructible<To1, From1 const&>::value), (std::is_constructible<To2, From2 const&>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (std::is_constructible<To1, From1&&>::value), (std::is_constructible<To2, From2&&>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (std::is_constructible<To1, From1 const&&>::value), (std::is_constructible<To2, From2 const&&>::value));
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
    assert_alikeness<std::is_copy_constructible, T>();
    assert_alikeness<std::is_copy_assignable, T>();
    assert_alikeness<std::is_move_constructible, T>();
    assert_alikeness<std::is_move_assignable, T>();
    assert_alikeness<is_hashable, T>();
    assert_alikeness<bsoncxx::detail::is_equality_comparable, T>();
    assert_alikeness<bsoncxx::detail::is_totally_ordered, T>();
    assert_alikeness<std::is_trivially_destructible, T>();
    STATIC_ASSERT_EXPR_ALIKE(
        (bsoncxx::detail::is_equality_comparable<T>::value),
        (bsoncxx::detail::is_equality_comparable<T, optional<T>>::value));
    STATIC_ASSERT_EXPR_ALIKE(
        (bsoncxx::detail::is_totally_ordered<T>::value),
        (bsoncxx::detail::is_totally_ordered_with<T, optional<T>>::value));
    STATIC_ASSERT_EXPR_ALIKE((std::is_constructible<T, T>::value), (std::is_constructible<optional<T>, T>::value));
    STATIC_ASSERT_EXPR((std::is_constructible<optional<T>, bsoncxx::stdx::nullopt_t>::value));
    // Assert we return proper reference types
    STATIC_ASSERT_EXPR((std::is_same<deref_t<optional<T>>, T&&>::value));
    STATIC_ASSERT_EXPR((std::is_same<deref_t<optional<T> const>, T const&&>::value));
    STATIC_ASSERT_EXPR((std::is_same<deref_t<optional<T> const&>, T const&>::value));
    STATIC_ASSERT_EXPR((std::is_same<deref_t<optional<T>&>, T&>::value));
    // .value()
    STATIC_ASSERT_EXPR((std::is_same<value_t<optional<T>>, T&&>::value));
    STATIC_ASSERT_EXPR((std::is_same<value_t<optional<T> const>, T const&&>::value));
    STATIC_ASSERT_EXPR((std::is_same<value_t<optional<T> const&>, T const&>::value));
    STATIC_ASSERT_EXPR((std::is_same<value_t<optional<T>&>, T&>::value));
    // operator->
    STATIC_ASSERT_EXPR((std::is_same<arrow_t<optional<T>>, T*>::value));
    STATIC_ASSERT_EXPR((std::is_same<arrow_t<optional<T> const>, T const*>::value));
    STATIC_ASSERT_EXPR((std::is_same<arrow_t<optional<T> const&>, T const*>::value));
    STATIC_ASSERT_EXPR((std::is_same<arrow_t<optional<T>&>, T*>::value));
    return check_conversions<T, T>();
}

} // namespace

STATIC_ASSERT_EXPR(bsoncxx::detail::is_totally_ordered<std::string>::value);
STATIC_ASSERT_EXPR(bsoncxx::detail::is_totally_ordered<int>::value);
STATIC_ASSERT_EXPR(!bsoncxx::detail::is_totally_ordered<not_ordered>::value);

#ifndef NO_LWG_2543
STATIC_ASSERT_EXPR(is_hashable<optional<int>>::value);
STATIC_ASSERT_EXPR(!is_hashable<optional<immobile>>::value);
#endif

// Having this static_assert appear prior to static_checks<int> prevents a later static assert error
// that occurs only on MSVC 19.29 (VS2019). Obviously.
STATIC_ASSERT_EXPR(bsoncxx::detail::is_totally_ordered<optional<int>>::value);
// It's a useful check on its own, but now you are cursed with this knowledge just as I have been.
// pain.

TEST_CASE("Trait checks") {
    CHECK(static_checks<int>());
    CHECK(static_checks<int const>());
    CHECK(static_checks<std::string>());
    CHECK(static_checks<std::string const>());
    CHECK(static_checks<std::unique_ptr<int>>());
    CHECK(static_checks<std::unique_ptr<int> const>());
    CHECK(static_checks<immobile>());
    CHECK(static_checks<immobile const>());
    CHECK(static_checks<not_copyable>());
    CHECK(static_checks<not_copyable const>());
    CHECK(static_checks<not_default_constructible>());
    CHECK(static_checks<not_default_constructible const>());
    CHECK(static_checks<allows_moving_explicit_conversion>());
    CHECK(static_checks<allows_moving_explicit_conversion const>());
    CHECK(static_checks<allows_moving_implicit_conversion>());
    CHECK(static_checks<allows_moving_implicit_conversion const>());
    CHECK(check_conversions<int, int>());
    CHECK(check_conversions<int const, int>());
    CHECK(check_conversions<int, int const>());
    CHECK(check_conversions<int, double>());
    CHECK(check_conversions<double, std::string>());
    CHECK(check_conversions<std::string, double>());
    CHECK(check_conversions<std::string, char const*>());
    CHECK(check_conversions<char const*, std::string>());
    CHECK(check_conversions<int*, std::unique_ptr<int>>());
    CHECK(check_conversions<int, allows_moving_explicit_conversion>());
    CHECK(check_conversions<int, allows_moving_implicit_conversion>());
}

TEST_CASE("optional constructors") {
    // (1)
    {
        optional<int> opt = optional<int>();
        CHECK_FALSE(opt);

        optional<int> opt2 = optional<int>(nullopt);
        CHECK_FALSE(opt2);
    }
    // (2)
    {
        optional<int> opt1 = optional<int>();
        optional<int> opt2 = optional<int>(opt1);
        CHECK_FALSE(opt2);
    }

    // (3)
    {
        optional<int> opt1 = optional<int>(123);
        optional<int> opt2 = optional<int>(std::move(opt1));
        CHECK_VS2017(*opt2 == 123);
    }

    // (4)
    {
        struct Src {
            int s;
        };
        struct Dest {
            // Can construct a Dest from Src.
            Dest(Src s) {
                this->d = s.s;
            }
            int d;
        };

        Src s;
        s.s = 123;
        optional<Src> opt_src = optional<Src>(s);
        optional<Dest> opt_dest = opt_src;
        CHECK_VS2017(opt_dest->d == 123);
    }

    // (5)
    {
        struct Src {
            int s;
        };
        struct Dest {
            // Can construct a Dest from Src.
            Dest(Src s) {
                this->d = s.s;
            }
            int d;
        };

        Src s;
        s.s = 123;
        optional<Src> opt_src = optional<Src>(s);
        optional<Dest> opt_dest = std::move(opt_src);
        CHECK_VS2017(opt_dest->d == 123);
    }

    // (6)
    {
        struct Foo {
            Foo(int a, int b) {
                this->c = a + b;
            }
            int c;
        };
        optional<Foo> opt = optional<Foo>(in_place, 1, 2);
        CHECK_VS2017(opt->c == 3);
    }

    // (7)
    {
        struct Foo {
            Foo(std::initializer_list<int>) {}
        };
        std::initializer_list<int> il = {1, 2};
        optional<Foo> opt = optional<Foo>(in_place, il);
        (void)opt;
    }

    // (8)
    {
        struct Foo {
            int f;
        };
        Foo f{0};
        optional<Foo> opt = optional<Foo>(std::move(f));
        (void)opt;
    }
}

TEST_CASE("optional assignment operator") {
    // (1)
    {
        optional<int> foo;
        optional<int>& ref = (foo = nullopt);
        CHECK(!foo);
        CHECK(!ref);
    }

    // (2)
    {
        optional<int> foo;
        optional<int> other = 123;
        optional<int>& ref = (foo = other);
        CHECK(foo);
        CHECK_VS2017(*foo == 123);
        CHECK(ref);
        CHECK_VS2017(*ref == 123);
    }

    // (3)
    {
        optional<int> foo;
        optional<int> other = 123;
        optional<int>& ref = (foo = std::move(other));
        CHECK(foo);
        CHECK_VS2017(*foo == 123);
        CHECK(ref);
        CHECK_VS2017(*ref == 123);
    }

    // (4)
    {
        optional<int> foo = 123;
        CHECK_VS2017(*foo == 123);
    }

    // (5)
    {
        struct Src {
            int s;
        };
        struct Dest {
            // Can construct a Dest from Src.
            Dest(Src s) {
                this->d = s.s;
            }
            int d;
        };

        Src s;
        s.s = 123;
        optional<Src> opt_src = optional<Src>(s);
        optional<Dest> opt_dest = opt_src;
        CHECK_VS2017(opt_dest->d == 123);
    }

    // (6)
    {
        struct Src {};
        struct Dest {
            // Can construct a Dest from Src.
            Dest(Src) {}
        };

        Src s;
        optional<Src> opt_src = optional<Src>(s);
        optional<Dest> opt_dest = std::move(opt_src);
        (void)opt_dest;
    }
}

TEST_CASE("optional operator->") {
    struct Foo {
        int x;
    };
    optional<Foo> opt = Foo();
    opt->x = 123;
    CHECK_VS2017(opt->x == 123);
}

TEST_CASE("optional operator bool") {
    struct Foo {
        int x;
    };
    optional<Foo> opt = Foo();
    CHECK(opt);
    CHECK(opt.has_value());
    opt = nullopt;
    CHECK(!opt);
    CHECK(!opt.has_value());
}

TEST_CASE("optional value()") {
    optional<int> opt = 123;
    CHECK_VS2017(opt.value() == 123);
}

TEST_CASE("optional value_or()") {
    struct Src {
        Src(int x) : x(x) {}
        int x;
    };
    struct Dest {
        // Can construct a Dest from Src.
        Dest(Src s) : x(s.x) {}
        int x;
    };

    optional<Dest> opt = nullopt;
    Dest d = opt.value_or(Src(123));
    CHECK_VS2017(d.x == 123);
}

TEST_CASE("optional reset()") {
    optional<int> opt = 123;
    opt.reset();
    CHECK(!opt);
}

TEST_CASE("optional emplace()") {
    optional<int> opt = 123;
    opt.emplace(456);
    CHECK_VS2017(*opt == 456);
}

TEST_CASE("make_optional") {
    auto opt = bsoncxx::stdx::make_optional(123);
    CHECK(opt);
    CHECK_VS2017(*opt == 123);
}

TEST_CASE("optional swap") {
    optional<int> opt1 = 123;
    optional<int> opt2 = nullopt;
    CHECK(opt1.has_value());
    CHECK(!opt2.has_value());
    opt1.swap(opt2);
    CHECK(!opt1.has_value());
    CHECK(opt2.has_value());
}

TEST_CASE("optional: Nontrivial contents") {
    optional<std::string> str = "abcd1234";
    CHECK_VS2017(str == "abcd1234");
    {
        auto dup = str;
        CHECK_VS2017(dup == str);
    }
    CHECK_VS2017(str == "abcd1234");

    optional<std::unique_ptr<int>> aptr;
    CHECK_VS2017(aptr != nullptr);
    CHECK_VS2017(aptr == bsoncxx::stdx::nullopt);
    {
        auto dup = std::move(aptr);
        CHECK_VS2017(aptr == dup);
    }
    aptr = bsoncxx::make_unique<int>(31);
    CHECK_VS2017(aptr != nullopt);
    REQUIRE_VS2017(aptr != nullptr);
    CHECK_VS2017(**aptr == 31);
    {
        auto dup = std::move(aptr);
        CHECK(aptr);
        CHECK_VS2017(aptr == nullptr);
        REQUIRE(dup);
        REQUIRE_VS2017(dup != nullptr);
        CHECK_VS2017(**dup == 31);
    }
    CHECK_VS2017(aptr == nullptr);
}

TEST_CASE("Comparisons") {
    optional<int> a = 21;
    optional<int> b = 23;
    optional<int> c;
    CHECK_VS2017(a != nullopt);
    CHECK_VS2017(a == 21);
    CHECK_VS2017(b != nullopt);
    CHECK_VS2017(b == 23);
    CHECK_VS2017(c == nullopt);
    // Null compares less-than values:
    CHECK_VS2017(c < 42);
    CHECK_VS2017(a < b);
    CHECK_VS2017(c < a);
    CHECK_VS2017(c < b);
    CHECK_VS2017(c == nullopt);
    CHECK_VS2017(nullopt < a);
    CHECK_VS2017(nullopt == c);
    CHECK_VS2017(a != b);
    CHECK_VS2017(c != a);
    CHECK_VS2017(c != b);
    CHECK_VS2017(c == c);

    optional<double> dbl = 3.14;
    CHECK_VS2017(dbl != a);
}

template <typename T, typename U>
void check_ordered(T const& lesser, U const& greater, std::string desc) {
    CAPTURE(__func__, desc);
    CHECK_VS2017(lesser < greater);
    CHECK_VS2017(greater > lesser);
    CHECK_VS2017(lesser <= greater);
    CHECK_VS2017(greater >= lesser);
    CHECK_FALSE_VS2017(greater < lesser);
    CHECK_FALSE_VS2017(lesser > greater);
    CHECK_VS2017(lesser != greater);
    CHECK_FALSE_VS2017(lesser == greater);
}

template <typename T, typename U>
void check_equivalent(T const& a, U const& b, std::string desc) {
    CAPTURE(__func__, desc);
    CHECK_VS2017(a == b);
    CHECK_VS2017(b == a);
    CHECK_FALSE_VS2017(a != b);
    CHECK_FALSE_VS2017(b != a);
    CHECK_VS2017(a <= b);
    CHECK_VS2017(b >= a);
    CHECK_FALSE_VS2017(a < b);
    CHECK_FALSE_VS2017(b > a);
    CHECK_FALSE_VS2017(b < a);
    CHECK_FALSE_VS2017(a > b);
}

template <typename T, typename U>
void regular_cases(T low_value, U high_value) {
    optional<T> a, b;
    check_equivalent(a, b, "Null optionals");
    check_equivalent(a, nullopt, "Compare with nullopt");
    a = low_value;
    check_equivalent(a, a, "Self-compare");
    check_equivalent(a, low_value, "Engaged == value");
    check_ordered(a, high_value, "low enganged < high value");
    check_ordered(b, a, "Value > null optional");
    check_ordered(nullopt, a, "Value > nullopt");
    using std::swap;
    swap(a, b);
    check_equivalent(b, low_value, "low-value after swap");
    check_ordered(b, high_value, "High-value compare after swap");
    check_ordered(a, b, "Optional compare after swap");
    swap(a, b);
    b.emplace(high_value);
    check_ordered(a, b, "Engaged compare");
    swap(a, b);
    check_ordered(b, a, "Engaged compare after swap");
    swap(a, b);

    a = b;
    check_equivalent(a, b, "Equal after assignment");
    check_equivalent(a, high_value, "'a' received high value");
    check_ordered(low_value, a, "'a' is greater than high-value after assignment");
}

TEST_CASE("Optional: Cross-comparisons") {
    regular_cases<int>(2, 4);
    regular_cases<double>(2, 4.0);
    regular_cases(std::string("abc"), std::string("xyz"));
    regular_cases(std::string("abc"), "xyz");
    regular_cases(std::string("abc"), bsoncxx::stdx::string_view("xyz"));
    regular_cases(bsoncxx::stdx::string_view("abc"), std::string("xyz"));
}

template <typename T>
std::size_t hashit(T const& what) {
    return std::hash<T>{}(what);
}

TEST_CASE("Optional-of-const-T") {
    optional<int const> a;
    check_equivalent(a, nullopt, "Null of const");
    a.emplace(21);
    check_equivalent(a, 21, "Const 21");
    CHECK_VS2017(hashit(a) == hashit(21));
    auto b = a;
    CHECK_VS2017(a == b);
}

TEST_CASE("Optional: Hashing") {
    optional<int> a, b;
    CHECK_VS2017(hashit(a) == hashit(a));
    CHECK_VS2017(hashit(a) == hashit(b));
    b.emplace(41);
    CHECK_VS2017(hashit(41) == hashit(b));
    CHECK_VS2017(hashit(a) != hashit(b)); // (Extremely probable, but not certain)
    a.emplace(41);
    CHECK_VS2017(hashit(a) == hashit(b));
    optional<int const> c = b;
    CHECK_VS2017(hashit(c) == hashit(a));
}

struct in_place_convertible {
    bool constructed_from_in_place = false;
    in_place_convertible() = default;
    in_place_convertible(bsoncxx::stdx::in_place_t) : constructed_from_in_place(true) {}
};

TEST_CASE("optional<T> conversions") {
    // Some stdlib implementations do not forbid this ctor correctly.
#if defined(BSONCXX_POLY_USE_IMPLS)
    STATIC_ASSERT_EXPR((!std::is_constructible<optional<bool>, optional<std::string>>::value));
#endif // defined(BSONCXX_POLY_USE_IMPLS)

    optional<std::string> s1(bsoncxx::stdx::in_place);
    CHECK_VS2017(s1 == "");

    optional<in_place_convertible> q(bsoncxx::stdx::in_place);
    REQUIRE(q.has_value());
    CHECK_FALSE(q->constructed_from_in_place);

    optional<char const*> c_str = "foo";
    optional<std::string> string = c_str;
    optional<std::string> string2 = std::move(c_str);
    CHECK_VS2017(string == c_str);
    CHECK_VS2017(string2 == c_str);
}

#endif // defined(BSONCXX_POLY_USE_IMPLS) || defined(BSONCXX_POLY_USE_STD)
