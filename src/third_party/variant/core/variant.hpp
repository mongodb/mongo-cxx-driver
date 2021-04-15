#ifndef CORE_VARIANT_HPP
#define CORE_VARIANT_HPP

#include "type_traits.hpp"
#include "functional.hpp"
#include "utility.hpp"

#include <stdexcept>
#include <typeinfo>
#include <limits>

#include <cstdint>

namespace core {
inline namespace v1 {
namespace impl {

template <class... Ts> union discriminate;
template <> union discriminate<> { };
template <class T, class... Ts>
union discriminate<T, Ts...> {
    T value;
    discriminate<Ts...> rest;
};

/* Used to provide lambda based pattern matching for the variant
 * Based off of Dave Abrahams C++11 'generic lambda' example.
 */
template <class... Lambdas> struct overload;
template <class Lambda> struct overload<Lambda> : Lambda {
    using call_type = Lambda;
    using call_type::operator ();
};
template <class Lambda, class... Lambdas>
struct overload<Lambda, Lambdas...> :
    private Lambda,
    private overload<Lambdas...>::call_type
{
    using base_type = typename overload<Lambdas...>::call_type;

    using lambda_type = Lambda;
    using call_type = overload;

    overload (Lambda&& lambda, Lambdas&&... lambdas) :
        lambda_type(::core::forward<Lambda>(lambda)),
        base_type(::core::forward<Lambdas>(lambdas)...)
    { }

    using lambda_type::operator ();
    using base_type::operator ();
};

template <class... Lambdas>
auto make_overload(Lambdas&&... lambdas) -> overload<Lambdas...> {
    return overload<Lambdas...> { ::core::forward<Lambdas>(lambdas)... };
}

template <class Visitor, class Type, class Data, class Result, class... Args>
auto visitor_gen () -> Result {
    return [](Visitor&& visitor, Data& data, Args&&... args) {
      return invoke(
          ::core::forward<Visitor>(visitor),
          reinterpret_cast<Type&>(data),
          ::core::forward<Args>(args)...
      );
    };
}

} /* namespace impl */


struct bad_variant_get final : ::std::logic_error {
    using ::std::logic_error::logic_error;
};

/* visitation semantics require that, given a callable type C, and variadic
 * arguments Args... that the return type of the visit will be SFINAE-ified
 * as common_type_t<invoke_of_t<C, Args>...> (this assumes a variadic
 * approach can be taken with common_type, which it cannot at this time. A
 * custom SFINAE-capable version has been written within the type traits
 * component.
 *
 * Obviously if a common type cannot be found, then the visitation function
 * cannot be generated.
 *
 * These same semantics are required for variant<Ts...>::match which simply
 * calls visit with a generate overload<Lambdas...> type.
 */
template <class... Ts>
class variant final {
    static_assert(
        sizeof...(Ts) < ::std::numeric_limits<uint8_t>::max(),
        "Cannot have more elements than variant can contain"
    );

    using tuple_type = ::std::tuple<Ts...>;
    using storage_type = aligned_storage_t<
        sizeof(impl::discriminate<Ts...>),
        ::std::alignment_of<impl::discriminate<Ts...>>::value
    >;

    template < ::std::size_t N>
    using element = typename ::std::tuple_element<N, tuple_type>::type;

    template < ::std::size_t N>
    using index = ::std::integral_constant< ::std::size_t, N>;

    struct copier final {
        using data_type = ::std::reference_wrapper<storage_type>;
        data_type data;

        template <class T>
        void operator ()(T const& value) const {
            new (::std::addressof(this->data.get())) T { value };
        }
    };

    struct mover final {
        using data_type = ::std::reference_wrapper<storage_type>;
        data_type data;

        template <class T>
        void operator () (T&& value) {
            new (::std::addressof(this->data.get())) decay_t<T> { ::std::move(value) };
        }
    };

    struct destroyer final {
        template <class T> void operator ()(T const& value) const { value.~T(); }
    };

    struct swapper final {
        using data_type = ::std::reference_wrapper<storage_type>;
        data_type data;
        template <class T>
        void operator ()(T&& value) noexcept(is_nothrow_swappable<T>::value) {
            using ::std::swap;
            swap(reinterpret_cast<decay_t<T>&>(this->data.get()), value);
        }
    };

    struct equality final {
        using data_type = ::std::reference_wrapper<storage_type const>;
        data_type data;

        template <class T>
        bool operator ()(T const& value) {
            return ::std::equal_to<T> { }(
                reinterpret_cast<T const&>(this->data.get()),
                value
            );
        }
    };

    struct less_than final {
        using data_type = ::std::reference_wrapper<storage_type const>;
        data_type data;

        template <class T>
        bool operator ()(T const& value) noexcept {
            return ::std::less<T> { }(
                reinterpret_cast<T const&>(this->data.get()),
                value
            );
        }
    };

    struct type_info final {
        template <class T>
        ::std::type_info const* operator ()(T&&) const noexcept {
            return ::std::addressof(typeid(decay_t<T>));
        }
    };

    template <
        ::std::size_t N,
        class=enable_if_t<N < sizeof...(Ts)>,
        class T
    > explicit variant (index<N>&&, ::std::false_type&&, T&& value) :
        variant {
            index<N + 1> { },
            ::std::is_constructible<type_at_t<N + 1, Ts...>, T> { },
            ::core::forward<T>(value)
        }
    { }

    template <
        ::std::size_t N,
        class=enable_if_t<N < sizeof...(Ts)>,
        class T
    > explicit variant (index<N>&&, ::std::true_type&&, T&& value) :
        data { }, tag { N }
    {
        new (::std::addressof(this->data)) type_at_t<N, Ts...> (
            ::core::forward<T>(value)
        );
    }

   public:

    template <
        class T,
        class=enable_if_t<not ::std::is_same<decay_t<T>, variant>::value>
    > variant (T&& value) :
        variant {
            index<0> { },
            ::std::is_constructible<type_at_t<0, Ts...>, T> { },
            ::core::forward<T>(value)
        }
    { }

    variant (variant const& that) :
        data { }, tag { that.tag }
    { that.visit(copier { ::std::ref(this->data) }); }

    variant (variant&& that) noexcept :
        data { }, tag { that.tag }
    { that.visit(mover { ::std::ref(this->data) }); }

    template <
        class=enable_if_t<
            ::std::is_default_constructible<type_at_t<0, Ts...>>::value
        >
    > variant () : variant { type_at_t<0, Ts...> { } } { }

    ~variant () { this->visit(destroyer { }); }

    template <
        class T,
        class=enable_if_t<not ::std::is_same<decay_t<T>, variant>::value>
    > variant& operator = (T&& value) {
        variant { ::core::forward<T>(value) }.swap(*this);
        return *this;
    }

    variant& operator = (variant const& that) {
        variant { that }.swap(*this);
        return *this;
    }

    variant& operator = (variant&& that) noexcept {
        this->visit(destroyer { });
        this->tag = that.tag;
        that.visit(mover { ::std::ref(this->data) });
        return *this;
    }

    /* Placing these inside of the variant results in no implicit conversions
     * occuring
     */
    bool operator == (variant const& that) const noexcept {
        if (this->tag != that.tag) { return false; }
        return that.visit(equality { ::std::cref(this->data) });
    }

    bool operator < (variant const& that) const noexcept {
        if (this->tag != that.tag) { return this->tag < that.tag; }
        return that.visit(less_than { ::std::cref(this->data) });
    }

    void swap (variant& that) noexcept(
    all_traits<is_nothrow_swappable<Ts>...>::value
    ) {
        if (this->which() == that.which()) {
            that.visit(swapper { ::std::ref(this->data) });
            return;
        }
        variant temp { ::core::move(*this) };
        *this = ::core::move(that);
        that = ::core::move(temp);
    }

    template <class Visitor, class... Args>
    auto visit (Visitor&& visitor, Args&&... args) -> common_type_t<
        invoke_of_t<Visitor, Ts, Args...>...
    > {
        using return_type = common_type_t<invoke_of_t<Visitor, Ts, Args...>...>;
        using function = return_type(*)(Visitor&&, storage_type&, Args&&...);
        constexpr ::std::size_t size = ::std::tuple_size<tuple_type>::value;

        static function const callers[size] {
            impl::visitor_gen<Visitor, Ts, storage_type, function, Args...>()...
        };

        return callers[this->tag](
            ::std::forward<Visitor>(visitor),
            this->data,
            ::std::forward<Args>(args)...
        );
    }

    template <class Visitor, class... Args>
    auto visit (Visitor&& visitor, Args&&... args) const -> common_type_t<
        invoke_of_t<Visitor, Ts, Args...>...
    > {
        using return_type = common_type_t<invoke_of_t<Visitor, Ts, Args...>...>;
        using function = return_type(*)(Visitor&&, storage_type const&, Args&&...);
        constexpr ::std::size_t size = ::std::tuple_size<tuple_type>::value;

        static function const callers[size] = {
            impl::visitor_gen<
                Visitor,
                Ts const,
                storage_type const,
                function,
                Args...
            >()...
        };

        return callers[this->tag](
            ::core::forward<Visitor>(visitor),
            this->data,
            ::core::forward<Args>(args)...
        );
    }

    template <class... Visitors>
    auto match (Visitors&&... visitors) -> decltype(
    this->visit(impl::make_overload(::core::forward<Visitors>(visitors)...))
    ) {
        return this->visit(
            impl::make_overload(::core::forward<Visitors>(visitors)...)
        );
    }

    template <class... Visitors>
    auto match (Visitors&&... visitors) const -> decltype(
    this->visit(impl::make_overload(::core::forward<Visitors>(visitors)...))
    ) {
        return this->visit(
            impl::make_overload(::core::forward<Visitors>(visitors)...)
        );
    }

    template < ::std::size_t N>
    auto get () const& noexcept(false) -> element<N> const& {
        if (this->tag != N) { throw bad_variant_get { "incorrect type" }; }
        return reinterpret_cast<element<N> const&>(this->data);
    }

    template < ::std::size_t N>
    auto get () && noexcept(false) -> element<N>&& {
        if (this->tag != N) { throw bad_variant_get { "incorrect type" }; }
        return ::std::move(reinterpret_cast<element<N>&>(this->data));
    }

    template < ::std::size_t N>
    auto get () & noexcept(false) -> element<N>& {
        if (this->tag != N) { throw bad_variant_get { "incorrect type" }; }
        return reinterpret_cast<element<N>&>(this->data);
    }

    ::std::type_info const& type () const noexcept {
        return *this->visit(type_info { });
    }

    ::std::uint32_t which () const noexcept { return this->tag; }
    bool empty () const noexcept { return false; }

   private:
    storage_type data;
    ::std::uint8_t tag;
};

template <class... Ts>
void swap (variant<Ts...>& lhs, variant<Ts...>& rhs) noexcept(
noexcept(lhs.swap(rhs))
) { lhs.swap(rhs); }

}} /* namespace core::v1 */

namespace std {

template <class... Ts>
struct hash<core::v1::variant<Ts...>> {
    using argument_type = core::v1::variant<Ts...>;
    using result_type = size_t;
    result_type operator () (argument_type const& value) const {
        return value.match(hash<Ts> { }...);
    };
};

template <size_t I, class... Ts>
auto get (core::v1::variant<Ts...> const& variant) noexcept(false) -> decltype(
variant.template get<I>()
) { return variant.template get<I>(); }

template <size_t I, class... Ts>
auto get (core::v1::variant<Ts...>&& variant) noexcept(false) -> decltype(
variant.template get<I>()
) { return variant.template get<I>(); }

template <size_t I, class... Ts>
auto get (core::v1::variant<Ts...>& variant) noexcept (false) -> decltype(
variant.template get<I>()
) { return variant.template get<I>(); }

} /* namespace std */

#endif /* CORE_VARIANT_HPP */
