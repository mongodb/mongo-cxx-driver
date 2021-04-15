#ifndef CORE_FUNCTIONAL_HPP
#define CORE_FUNCTIONAL_HPP

#include "type_traits.hpp"
#include "utility.hpp"
#include <functional>
#include <tuple>
#include <array>

namespace core {
inline namespace v1 {

template <class F> struct function_traits;

template <class R, class... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> { };

template <class C, class R>
struct function_traits<R(C::*)> : function_traits<R(C&)> { };

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(C&, Args...)> { };

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const volatile> :
    function_traits<R(C volatile const&, Args...)>
{ };

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) volatile> :
    function_traits<R(C volatile&, Args...)>
{ };

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> :
    function_traits<R(C const&, Args...)>
{ };

template <class R, class... Args>
struct function_traits<R(Args...)> {
    using return_type = R;

    using pointer = return_type(*)(Args...);
    static constexpr ::std::size_t arity = sizeof...(Args);

    template < ::std::size_t N>
    using argument = typename ::std::tuple_element<
        N,
        ::std::tuple<Args...>
    >::type;
};

template <class F> struct function_traits {
    using functor_type = function_traits<decltype(&decay_t<F>::operator())>;
    using return_type = typename functor_type::return_type;
    using pointer = typename functor_type::pointer;
    static constexpr ::std::size_t arity = functor_type::arity - 1;
    template < ::std::size_t N>
    using argument = typename functor_type::template argument<N>;
};

/* N3727 */
template <class Functor, class Object, class... Args>
auto invoke (Functor&& functor, Object&& object, Args&&... args) -> enable_if_t<
    invokable<Functor, Object, Args...>::value,
    decltype((object.*functor)(::core::forward<Args>(args)...))
> { return (object.*functor)(::core::forward<Args>(args)...); }

template <class Functor, class Object, class... Args>
auto invoke (Functor&& functor, Object&& object, Args&&... args) -> enable_if_t<
    invokable<Functor, Object, Args...>::value,
    decltype(
    ((*::core::forward<Object>(object)).*functor)(::core::forward<Args>(args)...)
    )
> {
    return (
        (*::core::forward<Object>(object)).*functor
    )(::core::forward<Args>(args)...);
}

template <class Functor, class Object>
auto invoke (Functor&& functor, Object&& object) -> enable_if_t<
    invokable<Functor, Object>::value,
    decltype(object.*functor)
> { return object.*functor; }

template <class Functor, class Object>
auto invoke (Functor&& functor, Object&& object) -> enable_if_t<
    invokable<Functor, Object>::value,
    decltype((*::core::forward<Object>(object)).*functor)
> { return (*::core::forward<Object>(object)).*functor; }

template <class Functor, class... Args>
constexpr auto invoke (Functor&& functor, Args&&... args) -> enable_if_t<
    invokable<Functor, Args...>::value,
    decltype(::core::forward<Functor>(functor)(::core::forward<Args>(args)...))
> { return ::core::forward<Functor>(functor)(::core::forward<Args>(args)...); }

namespace impl {

template <class Functor, class U, ::std::size_t... I>
auto unpack (
    Functor&& functor,
    U&& unpackable,
    index_sequence<I...>&&
) -> invoke_of_t<
    Functor,
    decltype(::std::get<I>(::std::forward<U>(unpackable)))...
> {
    return ::core::v1::invoke(::std::forward<Functor>(functor),
                              ::std::get<I>(::std::forward<U>(unpackable))...
    );
}

template <class U, ::std::size_t... I>
auto unpack (U&& unpackable, index_sequence<I...>&&) -> invoke_of_t<
    decltype(::std::get<I>(::std::forward<U>(unpackable)))...
> {
    return ::core::v1::invoke(::std::get<I>(::std::forward<U>(unpackable))...);
}

template <class Functor, class U, ::std::size_t... I>
auto runpack (
    Functor&& functor,
    U&& runpackable,
    index_sequence<I...>&&
) -> invoke_of_t<Functor, decltype(::std::forward<U>(runpackable).at(I))...> {
    return ::core::v1::invoke(
        ::std::forward<Functor>(functor),
        ::std::forward<U>(runpackable).at(I)...);
}

} /* namespace impl */

struct unpack_t final { };
constexpr unpack_t unpack { };

struct runpack_t final { };
constexpr runpack_t runpack { };

template <class Functor, class Unpackable>
auto invoke (unpack_t, Functor&& functor, Unpackable&& unpackable) ->
enable_if_t<
    is_unpackable<decay_t<Unpackable>>::value,
    decltype(
    impl::unpack(
        ::std::forward<Functor>(functor),
        ::std::forward<Unpackable>(unpackable),
        make_index_sequence<::std::tuple_size<decay_t<Unpackable>>::value> { }
    )
    )
> {
    return impl::unpack(
        ::std::forward<Functor>(functor),
        ::std::forward<Unpackable>(unpackable),
        make_index_sequence<::std::tuple_size<decay_t<Unpackable>>::value> { }
    );
}

template <class Unpackable>
auto invoke (unpack_t, Unpackable&& unpackable) ->
enable_if_t<
    is_unpackable<decay_t<Unpackable>>::value,
    decltype(
    impl::unpack(
        ::std::forward<Unpackable>(unpackable),
        make_index_sequence<::std::tuple_size<decay_t<Unpackable>>::value> { }
    )
    )
> {
    return impl::unpack(
        ::std::forward<Unpackable>(unpackable),
        make_index_sequence<::std::tuple_size<decay_t<Unpackable>>::value> { }
    );
}

template <class Functor, class Runpackable>
auto invoke (
    runpack_t,
    Functor&& functor,
    Runpackable&& unpackable
) -> enable_if_t<
    is_runpackable<decay_t<Runpackable>>::value,
    decltype(
    impl::runpack(
        ::std::forward<Functor>(functor),
        ::std::forward<Runpackable>(unpackable),
        make_index_sequence<function_traits<Functor>::arity> { }
    )
    )
> {
    return impl::runpack(
        ::std::forward<Functor>(functor),
        ::std::forward<Runpackable>(unpackable),
        make_index_sequence<function_traits<Functor>::arity> { }
    );
}

}} /* namespace core::v1 */

#endif /* CORE_FUNCTIONAL_HPP */
