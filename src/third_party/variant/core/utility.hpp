#ifndef CORE_UTILITY_HPP
#define CORE_UTILITY_HPP

#include <functional>

#include <cstddef>

#include "type_traits.hpp"

namespace core {
inline namespace v1 {
namespace impl {

template <class T, T... I> struct integer_sequence {
    static_assert(
        ::std::is_integral<T>::value,
        "integer_sequence must use an integral type"
    );

    template <T N> using append = integer_sequence<T, I..., N>;
    static constexpr ::std::size_t size () noexcept { return sizeof...(I); }
    using next = append<size()>;
    using type = T;
};

template <class T, T Index, ::std::size_t N>
struct sequence_generator {
    static_assert(Index >= 0, "Index cannot be negative");
    using type = typename sequence_generator<T, Index - 1, N - 1>::type::next;
};

template <class T, T Index>
struct sequence_generator<T, Index, 0ul> { using type = integer_sequence<T>; };

template < ::std::size_t Index, class T, class U, class... Types>
struct typelist_index {
    using type = typename typelist_index<Index + 1, T, Types...>::type;
    static constexpr ::std::size_t value = Index;
};

template < ::std::size_t Index, class T, class... Types>
struct typelist_index<Index, T, T, Types...> {
    using type = typelist_index;
    static constexpr ::std::size_t value = Index;
};

} /* namespace impl */

template <class T>
constexpr T&& forward (remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template <class T>
constexpr T&& forward (remove_reference_t<T>&& t) noexcept {
    return static_cast<T&&>(t);
}

template <class T>
constexpr auto move (T&& t) noexcept -> decltype(
static_cast<remove_reference_t<T>&&>(t)
) { return static_cast<remove_reference_t<T>&&>(t); }


template <class T, T... I>
using integer_sequence = impl::integer_sequence<T, I...>;

template < ::std::size_t... I>
using index_sequence = integer_sequence< ::std::size_t, I...>;

template <class T, T N>
using make_integer_sequence = typename impl::sequence_generator<T, N, N>::type;

template < ::std::size_t N>
using make_index_sequence = make_integer_sequence< ::std::size_t, N>;

template <class T, class... Ts>
using typelist_index = ::std::integral_constant<
    ::std::size_t,
    impl::typelist_index<0ul, T, Ts...>::type::value
>;

/* N3761 (with some additions) */
template < ::std::size_t N, class T, class... Ts>
struct type_at { using type = typename type_at<N - 1, Ts...>::type; };

template <class T, class... Ts>
struct type_at<0ul, T, Ts...> { using type = T; };

template < ::std::size_t N, class... Ts>
using type_at_t = typename type_at<N, Ts...>::type;

template < ::std::size_t N, class T, class... Ts>
constexpr auto value_at (T&& value, Ts&&...) -> enable_if_t<
    N == 0 and N < (sizeof...(Ts) + 1),
    decltype(::core::forward<T>(value))
> { return ::core::forward<T>(value); }

template < ::std::size_t N, class T, class... Ts>
constexpr auto value_at (T&&, Ts&&... values) -> enable_if_t<
    N != 0 and N < (sizeof...(Ts) + 1),
    type_at_t<N, T, Ts...>
> { return value_at<N - 1, Ts...>(::core::forward<Ts>(values)...); }

template <class Callable>
struct scope_guard final {

    static_assert(
        ::std::is_nothrow_move_constructible<Callable>::value,
        "Given type must be nothrow move constructible"
    );

    explicit scope_guard (Callable callable) noexcept :
        callable { ::core::move(callable) },
        dismissed { false }
    { }

    scope_guard (scope_guard const&) = delete;
    scope_guard (scope_guard&&) = default;
    scope_guard () = delete;
    ~scope_guard () noexcept { if (not this->dismissed) { callable(); } }

    scope_guard& operator = (scope_guard const&) = delete;
    scope_guard& operator = (scope_guard&&) = default;

    void dismiss () noexcept { this->dismissed = true; }

   private:
    Callable callable;
    bool dismissed;
};

template <class Callable>
auto make_scope_guard(Callable&& callable) -> scope_guard<decay_t<Callable>> {
    return scope_guard<decay_t<Callable>> {
        ::core::forward<Callable>(callable)
    };
}

}} /* namespace core::v1 */

#endif /* CORE_UTILITY_HPP */
