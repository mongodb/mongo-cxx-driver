configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/private/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/private/config.hpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx/config
    COMPONENT dev
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libbsoncxx.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx.pc
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx.pc"
    DESTINATION "${PKG_CONFIG_PATH}"
    COMPONENT runtime
)
