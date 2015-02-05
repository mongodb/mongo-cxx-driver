configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
)

install (FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
    DESTINATION ${BSONCXX_INSTALL_DIR}/bson/config
)

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/private/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/private/config.hpp
)

