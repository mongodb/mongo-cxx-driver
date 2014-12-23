file(WRITE ${CMAKE_BINARY_DIR}/mongocxx.cpp "")

set(MONGO_CXX ${CMAKE_BINARY_DIR}/mongocxx.cpp)

add_library(mongocxx-static STATIC ${MONGO_CXX})
add_library(mongocxx-shared SHARED ${MONGO_CXX})

add_compiler_export_flags(LIBMONGOCXX_EXPORT_FLAGS)

set_target_properties(mongocxx-shared PROPERTIES
    DEFINE_SYMBOL LIBMONGOCXX_EXPORTS
    OUTPUT_NAME mongocxx
    COMPILE_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
    LINK_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
)

set_target_properties(mongocxx-static PROPERTIES
    DEFINE_SYMBOL LIBMONGOCXX_EXPORTS
    OUTPUT_NAME mongocxx
    COMPILE_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
    LINK_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
)

# TODO: LIBMONGOCXX_EXPORT -> LIBMONGOCXX_API
# TODO: MONGOCXX_SHARED_NO_EXPORT -> LIBMONGOCXX_PRIVATE
# TODO: LIBMONGOCXX_EXPORTS -> LIBMONGOCXX_BUILDING
# TODO: Need deprecated defines? or do the same?
generate_export_header(mongocxx-shared
    BASE_NAME LIBMONGOCXX
    EXPORT_MACRO_NAME LIBMONGOCXX_API
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE LIBMONGOCXX_STATIC
)

target_link_libraries(mongocxx-shared
    ${LIBMONGOC_LIBRARIES}
    mongocxx-driver
    mongocxx-bson
    mongocxx-stdx
)

target_link_libraries(mongocxx-static
    ${LIBMONGOC_LIBRARIES}
    mongocxx-driver
    mongocxx-bson
    mongocxx-stdx
)

if (BUILD_UNIT_TESTS)
    add_library(mongocxx-mocked SHARED ${MONGO_CXX})
    set_target_properties(mongocxx-mocked PROPERTIES
        DEFINE_SYMBOL LIBMONGOCXX_EXPORTS
        OUTPUT_NAME mongocxx-mocked
        COMPILE_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
        LINK_FLAGS ${LIBMONGOCXX_EXPORT_FLAGS}
    )
    target_link_libraries(mongocxx-mocked
        ${LIBMONGOC_LIBRARIES}
        mongocxx-driver-mocked
        mongocxx-bson
        mongocxx-stdx
    )
endif(BUILD_UNIT_TESTS)

add_subdirectory(driver)
add_subdirectory(bson)
add_subdirectory(stdx)
