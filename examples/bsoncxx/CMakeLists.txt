include_directories(
    ${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}
)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
)

set(BSONCXX_EXAMPLES
    builder_core.cpp
    builder_basic.cpp
    builder_stream.cpp
    getting_values.cpp
    view_and_value.cpp
)

foreach(EXAMPLE_SRC ${BSONCXX_EXAMPLES})
    get_filename_component(EXAMPLE_TARGET ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SRC})
    add_dependencies(${EXAMPLE_TARGET} install_headers install_libs)
    target_link_libraries(${EXAMPLE_TARGET} bsoncxx ${bsoncxx_libs})
    set(BSONCXX_EXAMPLE_EXECUTABLES ${BSONCXX_EXAMPLE_EXECUTABLES} ${EXAMPLE_TARGET})
endforeach(EXAMPLE_SRC)

set(BSONCXX_EXAMPLE_EXECUTABLES ${BSONCXX_EXAMPLE_EXECUTABLES} PARENT_SCOPE)
