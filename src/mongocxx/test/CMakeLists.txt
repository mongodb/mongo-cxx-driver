set(CMAKE_CXX_FLAGS "-DMONGOCXX_TESTING ${CMAKE_CXX_FLAGS}")

file(GLOB_RECURSE test_driver_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE test_driver_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/include
    ${LIBBSON_INCLUDE_DIRS}
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

link_directories(${LIBMONGOC_LIBRARY_DIRS} ${LIBBSON_LIBRARY_DIRS})

add_executable(test_driver
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/main.cpp
    ${test_driver_sources}
)
target_link_libraries(test_driver mongocxx-mocked)
add_test(driver test_driver)
