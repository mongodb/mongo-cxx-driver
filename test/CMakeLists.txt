enable_testing()

set(CMAKE_CXX_FLAGS "-DMONGOCXX_TESTING ${CMAKE_CXX_FLAGS}")

include_directories(
    "include"
    ${LIBBSON_INCLUDE_DIRS}
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

file(GLOB_RECURSE test_driver_headers mongo/driver/*.hpp)
file(GLOB_RECURSE test_driver_sources mongo/driver/*.cpp)

link_directories(${LIBMONGOC_LIBRARY_DIRS} ${LIBBSON_LIBRARY_DIRS})

add_executable(test_driver 
    ${CMAKE_SOURCE_DIR}/test/main.cpp
    ${test_driver_sources}
)
target_link_libraries(test_driver mongocxx-mocked)
add_test(driver test_driver)

file(GLOB_RECURSE test_bson_headers mongo/bson/*.hpp)
file(GLOB_RECURSE test_bson_sources mongo/bson/*.cpp)

add_executable(test_bson ${test_bson_sources})
target_link_libraries(test_bson bsoncxx)
add_test(bson test_bson)
