#pragma once

#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#if defined(__has_include)
#if __has_include(<version>)
#include <version>
#endif
#endif

#include "./operators.hpp"
#include "./type_traits.hpp"

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace detail {

/**
 * @brief Backport of std::pointer_traits with the addition of to_address() for pointers
 */
template <typename T>
struct pointer_traits : std::pointer_traits<T> {};

template <typename T>
struct pointer_traits<T*> : std::pointer_traits<T*> {
    using typename std::pointer_traits<T*>::pointer;
    using typename std::pointer_traits<T*>::element_type;

    static constexpr pointer to_address(pointer p) noexcept {
        return p;
    }
};

struct _to_address_fn {
    template <typename FancyPointer>
    static constexpr auto impl(FancyPointer fp, rank<3>)
        bsoncxx_returns(pointer_traits<FancyPointer>::to_address(fp));

    template <typename Iterator>
    static constexpr auto impl(Iterator iter, rank<2>) bsoncxx_returns(iter.operator->());

    template <typename T>
    static constexpr auto impl(T* p, rank<1>) bsoncxx_returns(p);

    template <typename Iterator>
    constexpr auto operator()(Iterator iter) const bsoncxx_returns((impl)(iter, rank<10>{}));
};

/**
 * @brief Convert an iterator or pointer-like type to a raw pointer.
 */
constexpr static _to_address_fn to_address;

/**
 * @brief The type obtained by to_address() for the given object, if valid
 */
template <typename T>
using to_address_t = decltype(to_address(std::declval<T&>()));

/**
 * @brief The result of applying unary operator* to the given object, if valid
 */
template <typename I>
using dereference_t = decltype(*std::declval<I>());

/**
 * @brief Detect a type that can be dereferenced (like a pointer) and the result
 * type is non-void
 */
template <typename I>
struct is_dereferencable : conjunction<is_detected<dereference_t, I>,
                                       // Clang supports dereferencing void*, and we can't detect
                                       // that easily. Refuse if I is (cv-)void*
                                       negation<std::is_void<remove_pointer_t<I>>>,
                                       negation<std::is_void<detected_t<dereference_t, I>>>> {};

/**
 * @brief Obtain the value type of the given iterator.
 *
 * This is only a very rough approximation for our use cases. A more thorough
 * C++20 impl requiers additional traits
 */
template <typename Iter>
using iter_value_t =
    requires_t<typename std::iterator_traits<Iter>::value_type, is_dereferencable<Iter>>;

/**
 * @brief Obtain the reference type of the given iterator (unless that type is `void`)
 */
template <typename Iter>
using iter_reference_t = requires_t<decltype(*std::declval<Iter&>()), is_dereferencable<Iter>>;

/**
 * @brief Obtain the difference type for the given iterator
 */
template <typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

template <typename Iter, typename = void>
struct is_weakly_incrementable : std::false_type {};

template <typename Iter>
struct is_weakly_incrementable<  //
    Iter,
    requires_t<void,
               std::is_object<Iter>,
               std::is_assignable<Iter&, Iter>,
               is_detected<iter_difference_t, Iter>,
               true_t<decltype(++std::declval<Iter&>()),  //
                      decltype(std::declval<Iter&>()++)>,
               std::is_same<decltype(++std::declval<Iter&>()), Iter&>>> : std::true_type {};

/**
 * @brief Detect a type that may be used as an iterator
 */
template <typename T>
struct is_iterator : conjunction<is_weakly_incrementable<T>,
                                 is_detected<iter_value_t, T>,
                                 is_detected<iter_reference_t, T>> {};

// We want contiguous_iterator_tag. We can't get the full functionality without
// stdlib support, but we can get reasonable approximation for our purposes
#if defined(__cpp_lib_ranges)
using std::contiguous_iterator_tag;
#else
struct contiguous_iterator_tag : std::random_access_iterator_tag {};
#endif

// Base case, use the iterator to get the actual traits from it.
template <typename I, typename = void>
struct ITER_TRAITS_impl {
    using type = I;
};

// If std::iterator_traits<I> is not "the default" (which contains no difference_type),
// then use std::iterator_traits.
template <typename I>
struct ITER_TRAITS_impl<I, void_t<typename std::iterator_traits<I>::difference_type>> {
    using type = std::iterator_traits<I>;
};

template <typename I>
using ITER_TRAITS = typename ITER_TRAITS_impl<I>::type;

// Get the iterator concept tag from the given iterator-like type
struct calc_iterator_concept {
    struct impl {
        template <typename I>
        static auto x(I*, rank<3>) bsoncxx_returns(contiguous_iterator_tag{});
        template <typename I>
        static auto x(I, rank<2>) bsoncxx_returns(typename ITER_TRAITS<I>::iterator_concept{});
        template <typename I>
        static auto x(I, rank<1>) bsoncxx_returns(typename ITER_TRAITS<I>::iterator_category{});
    };
    template <typename I>
    auto operator()(I) -> decltype(impl::x(I{}, rank<10>{}));
};

/**
 * @brief Obtain the iterator concept/category tag type, if present.
 *
 * Without C++20 stdlib support, does not detect contiguous_iterator_tag except
 * for on raw pointers.
 */
template <typename I>
using iterator_concept_t = decltype(calc_iterator_concept{}(I{}));

template <typename Iter, typename Tag>
struct is_iterator_kind
    : conjunction<is_iterator<Iter>, std::is_base_of<Tag, detected_t<iterator_concept_t, Iter>>> {};

template <typename I>
struct is_input_iterator : is_iterator_kind<I, std::input_iterator_tag> {};

template <typename I>
struct is_forwrd_iterator : is_iterator_kind<I, std::forward_iterator_tag> {};

template <typename I>
struct is_bidirectional_iterator : is_iterator_kind<I, std::bidirectional_iterator_tag> {};

template <typename I>
struct is_random_access_iterator : is_iterator_kind<I, std::random_access_iterator_tag> {};

template <typename I>
struct is_contiguous_iterator : is_iterator_kind<I, contiguous_iterator_tag> {};

/**
 * @brief Detect if the type `Sentinel` is a range sentinel for iterator `Iter`
 */
template <typename Sentinel, typename Iter>
struct is_sentinel_for : conjunction<is_equality_comparable<Sentinel, Iter>, is_iterator<Iter>> {};

template <typename Left, typename Right>
using difference_t = decltype(std::declval<Left>() - std::declval<Right>());

/**
 * @brief Detect if the type `Sentinel` is a sentinel for `Iter` and subtraction
 * is defined between them.
 */
template <typename Sentinel, typename Iter>
struct is_sized_sentinel_for
    : conjunction<
          is_sentinel_for<Sentinel, Iter>,
          std::is_convertible<detected_t<difference_t, Sentinel, Iter>, iter_difference_t<Iter>>,
          std::is_convertible<detected_t<difference_t, Iter, Sentinel>, iter_difference_t<Iter>>> {
};

}  // namespace detail
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
