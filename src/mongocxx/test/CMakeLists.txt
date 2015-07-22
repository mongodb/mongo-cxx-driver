add_definitions(
    -DMONGOCXX_TESTING
    -Wno-deprecated-declarations
)

set(mongocxx_test_sources
    CMakeLists.txt
    bulk_write.cpp
    client.cpp
    collection.cpp
    collection_mocked.cpp
    database.cpp
    instance.cpp
    model/update_many.cpp
    options/aggregate.cpp
    pool.cpp
    private/write_concern.cpp
    read_preference.cpp
    result/delete.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    write_concern.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/include
)

link_directories(${LIBMONGOC_LIBRARY_DIRS} ${LIBBSON_LIBRARY_DIRS})

add_executable(test_driver
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/main.cpp
    ${mongocxx_test_sources}
)

target_link_libraries(test_driver mongocxx_mocked)
add_test(driver test_driver)
