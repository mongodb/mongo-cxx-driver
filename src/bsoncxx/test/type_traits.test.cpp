#include <type_traits>

#include <bsoncxx/stdx/type_traits.hpp>
#include <third_party/catch/include/catch.hpp>

#if __GNUC__
// We declare variables that are only used for compilation checking
// (applies to Clang as well)
#pragma GCC diagnostic ignored "-Wunused"
#endif

namespace {
namespace tt = bsoncxx::stdx::type_traits;

template <typename Result, typename Expect>
struct assert_same {
    static_assert(std::is_same<Result, Expect>::value, "Fail");
    using x = void;
};

template <typename Expect, typename... Args>
struct Case {
    template <bsoncxx_ttparam F>
    struct apply {
        using x = typename assert_same<F<Args...>, Expect>::x;
    };
};

template <bsoncxx_ttparam Op, typename Case>
struct one_case {
    using x = typename Case::template apply<Op>::x;
};

template <bsoncxx_ttparam Oper, typename... Cases>
struct check_cases : one_case<Oper, Cases>... {};

constexpr check_cases<  //
    tt::decay_t,
    Case<int, int>,
    Case<int, const int>,
    Case<int, const int&>,
    Case<int, const int&&>,
    Case<int, int&>,
    Case<int, int&&>,
    Case<int*, int*>,
    Case<const int*, const int*>,
    Case<const int*, const int*&>,
    Case<const int*, const int[42]>,
    Case<const int*, const int (&)[42]>,
    Case<int*, int[42]>,
    Case<int (*)(int), int (&)(int)>,
    Case<void, void>>
    decay;

constexpr check_cases<  //
    tt::remove_cvref_t,
    Case<int, int&&>,
    Case<int, const int&&>,
    Case<int(int), int (&&)(int)>,
    Case<int(int), int (&)(int)>,
    Case<int[42], const int (&)[42]>,
    Case<int[42], const int[42]>,
    Case<const int*, const int*&>,
    Case<int, const int>,
    Case<void, const void>,
    Case<void, void>,
    Case<int, int>>
    remove_cvref;

constexpr check_cases<  //
    tt::const_reference_t,
    Case<void* const&, void*>,
    Case<const int&, volatile int&&>,
    Case<const int&, int&&>,
    Case<const int&, int&>,
    Case<const void, void>,
    Case<const int&, int>>
    const_reference;

constexpr check_cases<  //
    tt::void_t,
    Case<void, struct in_situ_never_defined>,
    Case<void, int>,
    Case<void, int, int>,
    Case<void>>
    void_t;

constexpr assert_same<std::true_type, tt::bool_constant<true>> t;
constexpr assert_same<std::false_type, tt::bool_constant<false>> f;

static_assert(tt::is_detected<tt::remove_cvref_t, int>::value, "huh");

template <typename T>
struct hard_error {
    static_assert(std::is_void<T>::value, "fail");
};

struct my_false {
    static constexpr bool value = false;
};

static_assert(std::is_base_of<my_false, tt::conjunction<my_false, std::false_type, void>>  //
              ::value,
              "fail");

static_assert(
    std::is_base_of<my_false, tt::conjunction<my_false, std::false_type, void, hard_error<int>>>  //
    ::value,
    "fail");

template <typename T, tt::requires_t<std::is_integral<T>> = 0>
T add_one(T v) {
    return v + 1;
}

template <typename T, tt::requires_t<tt::negation<std::is_integral<T>>> = 0>
T add_one(T other) {
    return other + "one";
}

TEST_CASE("requires_t") {
    CHECK(add_one(1) == 2);
    CHECK(add_one(std::string("twenty-")) == "twenty-one");
}

}  // namespace
