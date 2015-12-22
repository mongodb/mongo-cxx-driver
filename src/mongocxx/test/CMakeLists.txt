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
    hint.cpp
    instance.cpp
    model/update_many.cpp
    options/aggregate.cpp
    options/create_collection.cpp
    options/find.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/index.cpp
    options/insert.cpp
    options/modify_collection.cpp
    options/update.cpp
    pool.cpp
    private/scoped_bson_t.cpp
    private/write_concern.cpp
    read_concern.cpp
    read_preference.cpp
    result/delete.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    validation_criteria.cpp
    write_concern.cpp
)

include_directories(
    ${THIRD_PARTY_SOURCE_DIR}/catch/include
)

link_directories(${LIBMONGOC_LIBRARY_DIRS} ${LIBBSON_LIBRARY_DIRS})

add_executable(test_driver
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    ${mongocxx_test_sources}
)

target_link_libraries(test_driver mongocxx_mocked)
add_test(driver test_driver)
