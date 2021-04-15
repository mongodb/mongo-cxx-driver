#ifndef CORE_TYPE_TRAITS_HPP
#define CORE_TYPE_TRAITS_HPP

#include <type_traits>
#include <utility>

namespace core {
inline namespace v1 {

/* custom type traits */
/* tuple_size is used by unpack, so we expect it to be available.
 * We also expect ::std::get<N> to be available for the give type T
 */
template <class T>
class is_unpackable {
    template <class U> using tuple_size_t = typename ::std::tuple_size<U>::type;
    template <class U> static void check (tuple_size_t<U>*) noexcept;
    template <class> static void check (...) noexcept(false);
   public:
    static constexpr bool value = noexcept(check<T>(nullptr));
};

/* Used for types that have a .at(size_type) member function */
template <class T>
class is_runpackable {
    template <class U>
    static auto check (U* u) noexcept -> decltype(u->at(0ul), void());
    template <class> static void check (...) noexcept(false);
   public:
    static constexpr bool value = noexcept(check<T>(nullptr));
};

/* extracts the class of a member function ponter */
template <class T> struct class_of { using type = T; };
template <class Signature, class Type>
struct class_of<Signature Type::*> { using type = Type; };

/* forward declaration */
template <class... Args> struct invokable;
template <class... Args> struct invoke_of;
template <class T> struct result_of; /* SFINAE result_of */

/* C++14 style aliases for standard traits */
template <class T>
using remove_volatile_t = typename ::std::remove_volatile<T>::type;

template <class T>
using remove_const_t = typename ::std::remove_const<T>::type;
template <class T> using remove_cv_t = typename ::std::remove_cv<T>::type;

template <class T>
using add_volatile_t = typename ::std::add_volatile<T>::type;
template <class T> using add_const_t = typename ::std::add_const<T>::type;
template <class T> using add_cv_t = typename ::std::add_cv<T>::type;

template <class T>
using add_lvalue_reference_t = typename ::std::add_lvalue_reference<T>::type;

template <class T>
using add_rvalue_reference_t = typename ::std::add_rvalue_reference<T>::type;

template <class T>
using remove_reference_t = typename ::std::remove_reference<T>::type;

template <class T>
using remove_pointer_t = typename ::std::remove_pointer<T>::type;

template <class T> using add_pointer_t = typename ::std::add_pointer<T>::type;

template <class T>
using make_unsigned_t = typename ::std::make_unsigned<T>::type;
template <class T> using make_signed_t = typename ::std::make_signed<T>::type;

template <class T>
using remove_extent_t = typename ::std::remove_extent<T>::type;

template <class T>
using remove_all_extents_t = typename ::std::remove_all_extents<T>::type;

template < ::std::size_t Len, ::std::size_t Align>
using aligned_storage_t = typename ::std::aligned_storage<Len, Align>::type;

template <class T> using decay_t = typename ::std::decay<T>::type;

template <bool B, class T = void>
using enable_if_t = typename ::std::enable_if<B, T>::type;

template <bool B, class T, class F>
using conditional_t = typename ::std::conditional<B, T, F>::type;

template <class T>
using underlying_type_t = typename ::std::underlying_type<T>::type;

/* custom type trait specializations */
template <class... Args> using invoke_of_t = typename invoke_of<Args...>::type;
template <class T> using class_of_t = typename class_of<T>::type;

namespace impl {

struct undefined { undefined (...); };

/* Get the result of an attempt at the INVOKE expression */
/* fallback */
template <class... Args> auto invoke_expr (undefined, Args&&...) -> undefined;

template <class Functor, class Object, class... Args>
auto invoke_expr (Functor&& fun, Object&& obj, Args&&... args) -> enable_if_t<
    ::std::is_member_function_pointer<remove_reference_t<Functor>>::value and
    ::std::is_base_of<
        class_of_t<remove_reference_t<Functor>>,
        remove_reference_t<Object>
    >::value,
    decltype((::std::forward<Object>(obj).*fun)(::std::forward<Args>(args)...))
>;

template <class Functor, class Object, class... Args>
auto invoke_expr (Functor&& fun, Object&& obj, Args&&... args) -> enable_if_t<
    ::std::is_member_function_pointer<remove_reference_t<Functor>>::value and
    not ::std::is_base_of<
        class_of_t<remove_reference_t<Functor>>,
        remove_reference_t<Object>
    >::value,
    decltype(
    ((*::std::forward<Object>(obj)).*fun)(::std::forward<Args>(args)...)
    )
>;

template <class Functor, class Object>
auto invoke_expr (Functor&& functor, Object&& object) -> enable_if_t<
    ::std::is_member_object_pointer<remove_reference_t<Functor>>::value and
    ::std::is_base_of<
        class_of_t<remove_reference_t<Functor>>,
        remove_reference_t<Object>
    >::value,
    decltype(::std::forward<Object>(object).*functor)
>;

template <class Functor, class Object>
auto invoke_expr (Functor&& functor, Object&& object) -> enable_if_t<
    ::std::is_member_object_pointer<remove_reference_t<Functor>>::value and
    not ::std::is_base_of<
        class_of_t<remove_reference_t<Functor>>,
        remove_reference_t<Object>
    >::value,
    decltype((*::std::forward<Object>(object)).*functor)
>;

template <class Functor, class... Args>
auto invoke_expr (Functor&& functor, Args&&... args) -> decltype(
::std::forward<Functor>(functor)(::std::forward<Args>(args)...)
);

template <bool, class... Args> struct invoke_of { };
template <class... Args>
struct invoke_of<true, Args...> {
    using type = decltype(invoke_expr(::std::declval<Args>()...));
};

/* swappable implementation details */
using ::std::declval;
using ::std::swap;

template <class T, class U>
class is_swappable {
    template <class X, class Y>
    static auto check (int) noexcept -> decltype(
    swap(declval<X&>(), declval<Y&>()),
        void()
    );
    template <class X, class Y> static void check (...) noexcept(false);
   public:
    static constexpr bool value =
        noexcept(check<T, U>(0)) and noexcept(check<U, T>(0));
};

template <class T, class U>
struct is_nothrow_swappable : ::std::integral_constant<
    bool,
    is_swappable<T, U>::value and noexcept(swap(declval<T&>(), declval<U&>()))
> { };

} /* namespace impl */

template <class... Args> struct invokable : ::std::integral_constant<
    bool,
    not ::std::is_same<
        decltype(impl::invoke_expr(::std::declval<Args>()...)),
        impl::undefined
    >::value
> { };

template <class... Args> struct invoke_of :
    impl::invoke_of<invokable<Args...>::value, Args...>
{ };

template <class F, class... Args>
struct result_of<F(Args...)> : invoke_of<F, Args...> { };

template <class T> using result_of_t = typename result_of<T>::type;

template <class... Ts> struct common_type;

template <class T> struct common_type<T> { using type = decay_t<T>; };
template <class T, class U>
struct common_type<T, U> {
    using type = decay_t<
        decltype(true ? ::std::declval<T>() : ::std::declval<U>())
    >;
};

template <class T, class U, class... Ts>
struct common_type<T, U, Ts...> {
    using type = typename common_type<
        typename common_type<T, U>::type,
        Ts...
    >::type;
};

template <class... T> using common_type_t = typename common_type<T...>::type;

/* is_null_pointer */
template <class T> struct is_null_pointer : ::std::false_type { };

template <>
struct is_null_pointer<add_cv_t< ::std::nullptr_t>> : ::std::true_type { };
template <>
struct is_null_pointer< ::std::nullptr_t volatile> : ::std::true_type { };
template <>
struct is_null_pointer< ::std::nullptr_t const> : ::std::true_type { };
template <>
struct is_null_pointer< ::std::nullptr_t> : ::std::true_type { };

/* is_swappable */
template <class T, class U=T>
using is_swappable = ::std::integral_constant<
    bool,
    impl::is_swappable<T, U>::value
>;

/* is_nothrow_swappable */
template <class T, class U=T>
using is_nothrow_swappable = impl::is_nothrow_swappable<T, U>;

/* all-traits */
template <class...> struct all_traits;
template <class T, class... Args>
struct all_traits<T, Args...> : ::std::integral_constant<bool,
    T::value and all_traits<Args...>::value
> { };
template <> struct all_traits<> : ::std::true_type { };

/* any-traits */
template <class...> struct any_traits;
template <class T, class... Args>
struct any_traits<T, Args...> : ::std::integral_constant<bool,
    T::value or any_traits<Args...>::value
> { };
template <> struct any_traits<> : ::std::false_type { };

/* no-traits */
template <class... Args> struct no_traits : ::std::integral_constant<bool,
    not all_traits<Args...>::value
> { };

}} /* namespace core::v1 */

#endif /* CORE_TYPE_TRAITS_HPP */
