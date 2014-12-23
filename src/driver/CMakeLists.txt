add_library(mongocxx-driver
    $<TARGET_OBJECTS:mongocxx-driver-base>
    $<TARGET_OBJECTS:mongocxx-driver-model>
    $<TARGET_OBJECTS:mongocxx-driver-options>
    $<TARGET_OBJECTS:mongocxx-driver-private>
    $<TARGET_OBJECTS:mongocxx-driver-result>
)

if (BUILD_UNIT_TESTS)
    add_library(mongocxx-driver-mocked
        $<TARGET_OBJECTS:mongocxx-driver-base-mocked>
        $<TARGET_OBJECTS:mongocxx-driver-model>
        $<TARGET_OBJECTS:mongocxx-driver-options>
        $<TARGET_OBJECTS:mongocxx-driver-private-mocked>
        $<TARGET_OBJECTS:mongocxx-driver-result>
    )
    set_target_properties(mongocxx-driver-mocked PROPERTIES
       COMPILE_FLAGS -DMONGOCXX_TESTING
    )
endif(BUILD_UNIT_TESTS)


add_subdirectory(base)
add_subdirectory(model)
add_subdirectory(options)
add_subdirectory(private)
add_subdirectory(result)
