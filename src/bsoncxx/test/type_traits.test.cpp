#include <string>
#include <type_traits>

#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/test/catch.hh>

// We declare variables that are only used for compilation checking
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wunused"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunused-template"));
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunneeded-member-function"));

namespace {

namespace tt = bsoncxx::detail;

template <typename Result, typename Expect>
struct assert_same {
    static_assert(std::is_same<Result, Expect>::value, "Fail");
    using x = void;
};

template <typename Expect, typename... Args>
struct Case {
    template <template <class...> class F>
    struct apply {
        using x = typename assert_same<F<Args...>, Expect>::x;
    };
};

template <template <class...> class Op, typename Case>
struct one_case {
    using x = typename Case::template apply<Op>::x;
};

template <template <class...> class Oper, typename... Cases>
struct check_cases : one_case<Oper, Cases>... {};

constexpr check_cases<
    tt::decay_t,
    Case<int, int>,
    Case<int, int const>,
    Case<int, int const&>,
    Case<int, int const&&>,
    Case<int, int&>,
    Case<int, int&&>,
    Case<int*, int*>,
    Case<int const*, int const*>,
    Case<int const*, int const*&>,
    Case<int const*, int const[42]>,
    Case<int const*, int const (&)[42]>,
    Case<int*, int[42]>,
    Case<int (*)(int), int (&)(int)>,
    Case<void, void>>
    decay;

constexpr check_cases<
    tt::remove_cvref_t,
    Case<int, int&&>,
    Case<int, int const&&>,
    Case<int(int), int (&&)(int)>,
    Case<int(int), int (&)(int)>,
    Case<int[42], int const (&)[42]>,
    Case<int[42], int const[42]>,
    Case<int const*, int const*&>,
    Case<int, int const>,
    Case<void, void const>,
    Case<void, void>,
    Case<int, int>>
    remove_cvref;

constexpr check_cases<
    tt::const_reference_t,
    Case<void* const&, void*>,
    Case<int const&, int volatile&&>,
    Case<int const&, int&&>,
    Case<int const&, int&>,
    Case<void const, void>,
    Case<int const&, int>>
    const_reference;

constexpr check_cases<
    tt::void_t,
    Case<void, struct in_situ_never_defined>,
    Case<void, int>,
    Case<void, int, int>,
    Case<void>>
    void_t;

constexpr assert_same<std::true_type, tt::bool_constant<true>> t;
constexpr assert_same<std::false_type, tt::bool_constant<false>> f;

static_assert(tt::is_detected<tt::remove_cvref_t, int>::value, "fail");

template <typename T>
struct hard_error {
    static_assert(std::is_void<T>::value, "fail");
};

struct my_false {
    static constexpr bool value = false;
};

static_assert(std::is_base_of<my_false, tt::conjunction<my_false, std::false_type, void>>::value, "fail");

static_assert(
    std::is_base_of<my_false, tt::conjunction<my_false, std::false_type, void, hard_error<int>>>::value,
    "fail");

template <typename T>
tt::requires_t<T, std::is_integral<T>> add_one(T v) {
    return v + 1;
}

template <typename T>
tt::requires_t<T, tt::negation<std::is_integral<T>>> add_one(T other) {
    return other + "one";
}

TEST_CASE("requires_t") {
    CHECK(add_one(1) == 2);
    CHECK(add_one(std::string("twenty-")) == "twenty-one");
}

struct something {
    int value;

    int memfn(int, std::string);
};

static_assert(tt::is_detected<tt::invoke_result_t, decltype(&something::value), something>::value, "fail");

static_assert(std::is_same<tt::invoke_result_t<decltype(&something::value), something&>, int&>::value, "fail");

static_assert(std::is_same<tt::invoke_result_t<decltype(&something::value), something&&>, int&&>::value, "fail");

static_assert(
    std::is_same<tt::invoke_result_t<decltype(&something::value), something const&>, int const&>::value,
    "fail");

static_assert(
    std::is_same<tt::invoke_result_t<decltype(&something::memfn), something&&, int, char const*>, int>::value,
    "fail");

// invoke_result_t disappears when given wrong argument types:
static_assert(!tt::is_detected<tt::invoke_result_t, decltype(&something::memfn), something&&, int, int>::value, "fail");

struct constrained_callable {
    // Viable only if F is callable as F(int, Arg)
    template <typename F, typename Arg>
    tt::requires_t<double, tt::is_invocable<F, int, Arg>> operator()(F&&, Arg) const {
        return 0.0;
    }
};

static_assert(
    !tt::is_detected<tt::invoke_result_t, constrained_callable, void (*)(int, std::string), double>::value,
    "fail");

static_assert(
    tt::is_detected<tt::invoke_result_t, constrained_callable, void (*)(int, std::string), char const*>::value,
    "fail");

static_assert(tt::is_detected<tt::invoke_result_t, constrained_callable, void (*)(int, double), double>::value, "fail");

struct rank_test {
    template <typename T>
    constexpr int val(T x, bsoncxx::detail::rank<0>) const = delete;
    template <typename T>
    constexpr int val(T x, bsoncxx::detail::rank<1>) const {
        return x + 30;
    }
    template <typename T>
    constexpr int operator()(T v) const {
        return this->val(v, bsoncxx::detail::rank<20>{});
    }
};

static_assert(rank_test{}(12) == 42, "fail");

} // namespace
