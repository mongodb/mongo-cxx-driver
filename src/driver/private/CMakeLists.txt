file (GLOB mongocxx-driver-private_headers *.hpp)
file (GLOB mongocxx-driver-private_sources *.cpp)

if (BUILD_UNIT_TESTS)
add_library(mongocxx-driver-private-mocked OBJECT ${mongocxx-driver-private_sources})
set_target_properties(mongocxx-driver-private-mocked PROPERTIES
   COMPILE_FLAGS -DMONGOCXX_TESTING
)
endif (BUILD_UNIT_TESTS)

add_library(mongocxx-driver-private OBJECT ${mongocxx-driver-private_sources})
