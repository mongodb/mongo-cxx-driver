add_custom_target(install_headers
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=dev -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
)

include_directories(
    ${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}
    ${CMAKE_INSTALL_PREFIX}/${MONGOCXX_HEADER_INSTALL_DIR}
)

link_directories(
    ${CMAKE_INSTALL_PERFIX}/lib
)

set(MONGOCXX_EXAMPLES
    aggregate.cpp
    create.cpp
    index.cpp
    query.cpp
    remove.cpp
    update.cpp
)

foreach(EXAMPLE_SRC ${MONGOCXX_EXAMPLES})
    get_filename_component(EXAMPLE_TARGET ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SRC})
    add_dependencies(${EXAMPLE_TARGET} install_headers)
    target_link_libraries(${EXAMPLE_TARGET} mongocxx ${mongocxx_libs})
endforeach(EXAMPLE_SRC)

add_custom_target(examples DEPENDS ${MONGOCXX_EXAMPLES})
