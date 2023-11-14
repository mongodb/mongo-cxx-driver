#include <cstddef>
#include <iostream>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/private/prelude.hh>

void bsoncxx::v_noabi::stdx::detail::string_view_oob_terminate(const char* msg,
                                                               std::size_t length,
                                                               std::ptrdiff_t position) noexcept {
    std::cerr << msg << ": Out-of-bounds access to string_view element at offset " << position
              << " (String has length " << length << ")" << std::endl;
    std::terminate();
}
