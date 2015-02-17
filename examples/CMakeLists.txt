add_custom_target(install_headers
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=dev -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
)

add_subdirectory(bsoncxx)
add_subdirectory(mongocxx)

add_custom_target(examples DEPENDS ${MONGOCXX_EXAMPLES} ${BSONCXX_EXAMPLES})
