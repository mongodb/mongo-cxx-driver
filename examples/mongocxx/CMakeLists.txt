include_directories(
    ${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}
    ${CMAKE_INSTALL_PREFIX}/${MONGOCXX_HEADER_INSTALL_DIR}
)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
)

set(MONGOCXX_EXAMPLES
    aggregate.cpp
    create.cpp
    document_validation.cpp
    index.cpp
    query.cpp
    remove.cpp
    update.cpp
    view_or_value_variant.cpp
)

foreach(EXAMPLE_SRC ${MONGOCXX_EXAMPLES})
    get_filename_component(EXAMPLE_TARGET ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SRC})
    add_dependencies(${EXAMPLE_TARGET} install_headers install_libs)
    target_link_libraries(${EXAMPLE_TARGET} mongocxx ${mongocxx_libs})
    set(MONGOCXX_EXAMPLE_EXECUTABLES ${MONGOCXX_EXAMPLE_EXECUTABLES} ${EXAMPLE_TARGET})
endforeach(EXAMPLE_SRC)

set(MONGOCXX_EXAMPLE_EXECUTABLES ${MONGOCXX_EXAMPLE_EXECUTABLES} PARENT_SCOPE)
