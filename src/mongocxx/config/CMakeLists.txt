configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/private/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/private/config.hpp
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.hpp
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}/mongocxx/config
    COMPONENT dev
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libmongocxx.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libmongocxx.pc
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libmongocxx.pc"
    DESTINATION "${PKG_CONFIG_PATH}"
    COMPONENT runtime
)
