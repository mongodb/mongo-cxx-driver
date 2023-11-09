#include "bsoncxx/stdx/iterator.hpp"

#include <deque>
#include <iterator>
#include <list>
#include <type_traits>

#include "bsoncxx/stdx/type_traits.hpp"
#include <third_party/catch/include/catch.hpp>

namespace {

struct fancy_ptr {
    std::string* realptr;

    constexpr std::string* operator->() const noexcept {
        return realptr;
    }
};

}  // namespace

namespace bsoncxx {
inline namespace v_noabi {
namespace detail {
template <>
struct pointer_traits<fancy_ptr> {
    constexpr static std::string* to_address(fancy_ptr p) {
        return p.realptr;
    }
};

}  // namespace detail
}  // namespace v_noabi
}  // namespace bsoncxx

TEST_CASE("To address") {
    int* p = nullptr;
    CHECK(bsoncxx::detail::to_address(p) == p);

    std::list<int> li = {1, 2, 3, 4};
    auto first = bsoncxx::detail::to_address(li.begin());
    CHECK(first == &li.front());

    std::string hello = "I am a string";
    fancy_ptr fp{&hello};
    CHECK(bsoncxx::detail::to_address(fp) == &hello);
}

static_assert(!bsoncxx::detail::is_dereferencable<int>{}, "fail");
static_assert(bsoncxx::detail::is_dereferencable<int*>{}, "fail");
static_assert(!bsoncxx::detail::is_dereferencable<void*>{}, "fail");

static_assert(bsoncxx::detail::is_weakly_incrementable<std::string::iterator>{}, "fail");
static_assert(!bsoncxx::detail::is_weakly_incrementable<std::string>{}, "fail");

static_assert(std::is_base_of<bsoncxx::detail::contiguous_iterator_tag,
                              bsoncxx::detail::iterator_concept_t<const char*>>{},
              "fail");
static_assert(std::is_base_of<std::random_access_iterator_tag,
                              bsoncxx::detail::iterator_concept_t<const char*>>{},
              "fail");
static_assert(std::is_base_of<std::bidirectional_iterator_tag,
                              bsoncxx::detail::iterator_concept_t<std::list<int>::iterator>>{},
              "fail");

static_assert(bsoncxx::detail::is_weakly_incrementable<const char*>{}, "fail");
static_assert(bsoncxx::detail::is_detected<bsoncxx::detail::iter_value_t, const char*>{}, "fail");
static_assert(std::is_same<bsoncxx::detail::iter_value_t<const char*>, char>::value, "fail");
static_assert(bsoncxx::detail::is_iterator<const char*>{}, "fail");
static_assert(bsoncxx::detail::is_contiguous_iterator<const char*>{}, "fail");
static_assert(!bsoncxx::detail::is_contiguous_iterator<std::deque<int>::iterator>{}, "fail");

struct deref_void {
    void operator*() const;
};
static_assert(bsoncxx::detail::is_detected<bsoncxx::detail::dereference_t, deref_void>{}, "fail");
static_assert(!bsoncxx::detail::is_dereferencable<deref_void>{}, "fail");
static_assert(!bsoncxx::detail::is_detected<bsoncxx::detail::iter_reference_t, deref_void>{},
              "fail");
