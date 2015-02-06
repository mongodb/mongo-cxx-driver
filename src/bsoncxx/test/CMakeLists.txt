include_directories(
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/include
    ${CMAKE_BINARY_DIR}/src
)

file(GLOB test_bson_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB test_bson_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(test_bson ${test_bson_sources})
target_link_libraries(test_bson bsoncxx)
add_test(bson test_bson)
