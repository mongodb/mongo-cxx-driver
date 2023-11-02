#pragma once

#include <type_traits>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {

BSONCXX_INLINE_NAMESPACE_BEGIN
namespace _traits {
#define bsoncxx_ttparam \
    template <class...> \
    class

/// Obtain the nested ::type of the given type argument
template <typename T>
using type_t = typename T::type;

/// Obtain the value_type member type of the given argument
template <typename T>
using value_type_t = typename T::value_type;

template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

#pragma push_macro("DECL_ALIAS")
#define DECL_ALIAS(Name)  \
    template <typename T> \
    using Name##_t = type_t<std::Name<T>>
DECL_ALIAS(decay);
DECL_ALIAS(make_signed);
DECL_ALIAS(make_unsigned);
DECL_ALIAS(remove_reference);
DECL_ALIAS(remove_const);
DECL_ALIAS(remove_volatile);
DECL_ALIAS(remove_cv);
DECL_ALIAS(add_const);
DECL_ALIAS(add_volatile);
DECL_ALIAS(add_lvalue_reference);
DECL_ALIAS(add_rvalue_reference);
#pragma pop_macro("DECL_ALIAS")

template <typename... Ts>
using common_type_t = type_t<std::common_type<Ts...>>;

/**
 * @brief Remove top-level const+volatile+reference qualifiers from the given type.
 */
template <typename T>
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;

/**
 * @brief Create a reference-to-const for the given type
 */
template <typename T>
using const_reference_t = add_lvalue_reference_t<const remove_cvref_t<T>>;

/**
 * @brief A "do-nothing" alias template that always evaluates to void
 *
 * @tparam Ts Zero or more type arguments, all discarded
 */
template <typename... Ts>
using void_t = void;

/**
 * @brief Alias for integral_constant<bool, B>
 */
template <bool B>
using bool_constant = std::integral_constant<bool, B>;

/**
 * @brief Holds a list of types.
 *
 * This template is never defined, so cannot be used in contexts that require a complete type.
 */
template <typename...>
struct mp_list;

/// ## Implementation of the C++11 detection idiom
namespace detail {

// Implementation of detection idiom for is_detected: true case
template <
    // A metafunction to try and apply
    bsoncxx_ttparam Oper,
    // The arguments to be given. These are deduced from the mp_list argument
    typename... Args,
    // Apply the arguments to the metafunction. If this yields a type, this function
    // will be viable. If substitution fails, this function is discarded from the
    // overload set.
    typename SfinaeHere = Oper<Args...>>
std::true_type is_detected_f(mp_list<Args...>*);

// Failure case for is_detected. Because this function takes an elipsis, this is
// less preferred than the above overload that accepts a pointer type directly.
template <bsoncxx_ttparam Oper, typename... Args>
std::false_type is_detected_f(...);

// Provides the detected_or impl
template <bool IsDetected>
struct detection;

// Non-detected case:
template <>
struct detection<false> {
    // We just return the default, since the metafunction will not apply
    template <typename Default, bsoncxx_ttparam, typename...>
    using f = Default;
};

// Detected case:
template <>
struct detection<true> {
    template <typename, bsoncxx_ttparam Oper, typename... Args>
    using f = Oper<Args...>;
};

}  // namespace detail

/**
 * @brief The type yielded by detected_t if the given type operator does not
 * yield a type.
 */
struct nonesuch;

/**
 * @brief Results in true_type if the given metafunction yields a valid type when applied to the
 * given arguments, otherwise yields false_type
 *
 * @tparam Oper A template that evaluates to a type
 * @tparam Args Some number of arguments to apply to Oper
 */
template <template <class...> class Oper, typename... Args>
struct is_detected
    : decltype(detail::is_detected_f<Oper>(static_cast<mp_list<Args...>*>(nullptr))) {};

/**
 * @brief If Oper<Args...> evaluates to a type, yields that type. Otherwise, yields
 * the Dflt type
 *
 * @tparam Dflt The default type to return if the metafunction does not apply
 * @tparam Oper A metafunction to speculatively apply
 * @tparam Args The arguments to give to the Oper metafunction
 */
template <typename Dflt, template <class...> class Oper, typename... Args>
using detected_or =
    typename detail::detection<is_detected<Oper, Args...>::value>::template f<Dflt, Oper, Args...>;

/**
 * @brief If Oper<Args...> evaluates to a type, yields that type. Otherwise, yields
 * the sentinel type `nonesuch`
 *
 * @tparam Oper A metafunction to try to apply
 * @tparam Args The metafunction arguments to apply to Oper
 */
template <template <class...> class Oper, typename... Args>
using detected_t = detected_or<nonesuch, Oper, Args...>;

/**
 * @brief Impl of conditional_t
 *
 * Separating the boolean from the type arguments results in significant speedup to compilation
 * due to type memoization
 */
namespace detail {

template <bool B>
struct conditional {
    template <typename IfTrue, typename>
    using f = IfTrue;
};

template <>
struct conditional<false> {
    template <typename, typename IfFalse>
    using f = IfFalse;
};

}  // namespace detail

/**
 * @brief Pick one of two types based on a boolean
 *
 * @tparam B A boolean value
 * @tparam T If `B` is true, pick this type
 * @tparam F If `B` is false, pick this type
 */
template <bool B, typename T, typename F>
using conditional_t = typename detail::conditional<B>::template f<T, F>;

// impl for conjunction+disjunction
namespace detail {

template <typename FalseType, typename Opers>
struct conj;

template <typename H, typename... Tail>
struct conj<bool_constant<H::value || !sizeof...(Tail)>, mp_list<H, Tail...>> : H {};

template <typename F, typename H, typename... Tail>
struct conj<F, mp_list<H, Tail...>> : conj<F, mp_list<Tail...>> {};

template <typename H>
struct conj<std::false_type, mp_list<H>> : H {};

template <>
struct conj<std::false_type, mp_list<>> : std::true_type {};

template <typename TrueType, typename Opers>
struct disj;

template <typename H, typename... Tail>
struct disj<bool_constant<H::value && sizeof...(Tail)>, mp_list<H, Tail...>> : H {};

template <typename F, typename H, typename... Tail>
struct disj<F, mp_list<H, Tail...>> : disj<F, mp_list<Tail...>> {};

template <typename H>
struct disj<std::true_type, mp_list<H>> : H {};

template <>
struct disj<std::true_type, mp_list<>> : std::false_type {};

}  // namespace detail

/**
 * @brief inherits unambiguously from the first of `Ts...` for which
 * `Ts::value` is a valid expression equal to `false`, or the last of `Ts...` otherwise.
 *
 * conjunction<> (given no arguments) inherits from std::true_type.
 *
 * If any of `Ts::value == false`, then no subsequent `Ts::value` will be instantiated.
 */
template <typename... Cond>
struct conjunction : detail::conj<std::false_type, mp_list<Cond...>> {};

/**
 * @brief Inherits unambiguous from the first of `Ts...` where `Ts::value` is `true`,
 * or the last of `Ts...` otherwise.
 *
 * Given no arguments, inherits from std::false_type;
 *
 * If any of `Ts::value == true`, then no subsequent `Ts::value` will be instantiated.
 */
template <typename... Cond>
struct disjunction : detail::disj<std::true_type, mp_list<Cond...>> {};

/**
 * @brief Given a boolean type trait, returns a type trait which is the logical negation thereof
 *
 * @tparam T A type trait with a static member ::value
 */
template <typename T>
struct negation : bool_constant<!T::value> {};

/**
 * @brief Yields std::true_type, regardless of type arguments.
 *
 * Useful for wrapping potential decltype() substitution failures in positions
 * that expect a bool_constant type.
 */
template <typename...>
using true_t = std::true_type;

namespace requires_detail {

template <typename R>
R norm_conjunction(...);

template <typename R, typename... Cs>
conjunction<Cs...> norm_conjunction(const conjunction<Cs...>&);

template <typename T>
using norm_conjunction_t = decltype(norm_conjunction<T>(std::declval<const T&>()));

template <typename Constraint, typename = void>
struct requirement;

template <typename FailingRequirement>
struct failed_requirement {
    failed_requirement(int) = delete;

    template <typename T>
    static T explain(failed_requirement);
};

template <typename... SubRequirements>
struct failed_requirement<conjunction<SubRequirements...>> {
    failed_requirement(int) = delete;

    template <typename T>
    static auto explain(int)
        -> common_type_t<decltype(requirement<SubRequirements>::test::template explain<T>(0))...>;
};

template <typename Constraint, typename>
struct requirement {
    using test = failed_requirement<requires_detail::norm_conjunction_t<Constraint>>;
};

template <typename Constraint>
struct requirement<Constraint, enable_if_t<Constraint::value>> {
    struct test {
        template <typename T>
        static T explain(int);
    };
};

}  // namespace requires_detail

/**
 * @brief If none of `Ts::value is 'false'`, yields the type `Type`, otherwise
 * this type is undefined.
 *
 * Use this to perform enable-if style template constraints.
 *
 * @tparam Type The type to return upon success
 * @tparam Traits A list of type traits with nested ::value members
 */
template <typename Type, typename... Traits>
#if defined _MSC_VER && _MSC_VER < 1910
// VS 2015 has trouble with expression SFINAE.
using requires_t = enable_if_t<conjunction<Traits...>::value, Type>;
#else
// Generates better error messages in case of substitution failure than a plain enable_if_t:
using requires_t =
    decltype(requires_detail::requirement<conjunction<Traits...>>::test::template explain<Type>(0));
#endif

/**
 * @brief If any of `Ts::value` is 'true', this type is undefined, otherwise
 * yields the type `Type`.
 *
 * Use this to perform enable-if template contraints.
 *
 * @tparam Type The type to return upon success
 * @tparam Traits A list of type traits with nested ::value members
 */
template <typename Type, typename... Traits>
using requires_not_t = requires_t<Type, negation<disjunction<Traits...>>>;

// Impl: invoke/is_invocable
namespace detail {

#pragma push_macro("RETURNS")
#define RETURNS(...)                                         \
    noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { \
        return __VA_ARGS__;                                  \
    }                                                        \
    static_assert(true, "")

template <bool IsMemberObject, bool IsMemberFunction>
struct invoker {
    template <typename F, typename... Args>
    constexpr static auto apply(F&& fun, Args&&... args)
        RETURNS(static_cast<F&&>(fun)(static_cast<Args&&>(args)...));
};

template <>
struct invoker<false, true> {
    template <typename F, typename Self, typename... Args>
    constexpr static auto apply(F&& fun, Self&& self, Args&&... args)
        RETURNS((static_cast<Self&&>(self).*fun)(static_cast<Args&&>(args)...));
};

template <>
struct invoker<true, false> {
    template <typename F, typename Self>
    constexpr static auto apply(F&& fun, Self&& self) RETURNS(static_cast<Self&&>(self).*fun);
};

}  // namespace detail

static constexpr struct invoke_fn {
    /**
     * @brief Invoke the given object with the given arguments.
     *
     * @param fn An invocable: A callable, member object pointer, or member function pointer.
     * @param args The arguments to use for invocation.
     */

    template <typename F, typename... Args, typename Fd = remove_cvref_t<F>>
    constexpr auto operator()(F&& fn, Args&&... args) const
        RETURNS(detail::invoker<std::is_member_object_pointer<Fd>::value,
                                std::is_member_function_pointer<Fd>::value>  //
                ::apply(static_cast<F&&>(fn), static_cast<Args&&>(args)...));
} invoke;

#pragma pop_macro("RETURNS")

/**
 * @brief Yields the type that would result from invoking F with the given arguments.
 *
 * @tparam Fun A invocable: A function pointer or callable object, or a member pointer
 * @tparam Args The arguments to apply
 */
template <typename F, typename... Args>
using invoke_result_t = decltype(_traits::invoke(std::declval<F>(), std::declval<Args>()...));

/**
 * @brief Trait type to detect if the given object can be "invoked" using the given arguments.
 *
 * @tparam Fun A invocable: A function pointer or callable object, or a member pointer
 * @tparam Args The arguments to match against
 */
template <typename Fun, typename... Args>
#if defined(_MSC_VER) && _MSC_VER < 1910
using is_invocable = is_detected<invoke_result_t, Fun, Args...>;
#else
struct is_invocable : is_detected<invoke_result_t, Fun, Args...> {
};
#endif

/**
 * @brief Trait detects whether the given types are the same after the removal
 * of top-level CV-ref qualifiers
 */
template <typename T, typename U>
using is_alike = std::is_same<remove_cvref_t<T>, remove_cvref_t<U>>;

}  // namespace _traits

BSONCXX_INLINE_NAMESPACE_END

}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
