include_directories(
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/include
)

add_executable(test_bson
    bson_builder.cpp
    bson_util_itoa.cpp
    bson_validate.cpp
    json.cpp
    new_tests.cpp
)

target_link_libraries(test_bson bsoncxx_static)
add_test(bson test_bson)
