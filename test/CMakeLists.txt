enable_testing()

set(CMAKE_CXX_FLAGS "-DMONGOCXX_TESTING ${CMAKE_CXX_FLAGS}")

include_directories("include")

add_executable(test_driver
    main.cpp
    driver/base/bulk_write.cpp
    driver/base/client.cpp
    driver/base/collection.cpp
    driver/base/database.cpp
    driver/base/write_concern.cpp
    driver/base/private/write_concern.cpp
    driver/model/update_many.cpp
    driver/options/aggregate.cpp
    driver/result/insert_one.cpp
    driver/result/delete.cpp
    driver/result/replace_one.cpp
    driver/result/update.cpp
)
target_link_libraries(test_driver mongocxx-mocked)
add_test(driver test_driver)

add_executable(test_bson
    bson/new_tests.cpp
    bson/bson_builder.cpp
    bson/bson_util_itoa.cpp
    bson/bson_string_or_literal.cpp
    #bson/test_1.cpp
    #bson/test_3.cpp
    #bson/test_4.cpp
    #bson/test_5.cpp
)

target_link_libraries(test_bson mongocxx-mocked)
add_test(bson test_bson)
