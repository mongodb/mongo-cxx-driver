add_custom_target(install_headers
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=dev -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
)

add_custom_target(install_libs
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=runtime -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
)

add_subdirectory(bsoncxx)
add_subdirectory(mongocxx)

add_custom_target(run-examples DEPENDS ${MONGOCXX_EXAMPLE_EXECUTABLES} ${BSONCXX_EXAMPLE_EXECUTABLES})

# Run all mongocxx examples on `make examples`.
foreach(EXAMPLE ${MONGOCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/mongocxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)

# Run all bsoncxx examples on `make examples`.
foreach(EXAMPLE ${BSONCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/bsoncxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)
