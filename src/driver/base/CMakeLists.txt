file (GLOB mongocxx-driver-base_headers *.hpp)
file (GLOB mongocxx-driver-base_sources *.cpp)

add_library(mongocxx-driver-base OBJECT ${mongocxx-driver-base_sources})

if (BUILD_UNIT_TESTS)
add_library(mongocxx-driver-base-mocked OBJECT ${mongocxx-driver-base_sources})
set_target_properties(mongocxx-driver-base-mocked PROPERTIES
   COMPILE_FLAGS -DMONGOCXX_TESTING
)
endif (BUILD_UNIT_TESTS)
