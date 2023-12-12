#include <iostream>

#include <bsoncxx/stdx/optional.hpp>

[[noreturn]] void bsoncxx::v_noabi::stdx::detail::terminate_disengaged_optional(
    const char* msg) noexcept {
    std::cerr << msg << ": Invalid attempted use of disengaged optional<T>" << std::endl;
    std::terminate();
}

[[noreturn]] void bsoncxx::v_noabi::stdx::detail::throw_bad_optional() {
    throw bad_optional_access();
}
