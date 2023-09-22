# Define and link a form of the mongocxx library
#
# This function requires the following variables to be defined in its parent scope:
# - mongocxx_sources
# - libmongoc_target
# - libmongoc_definitions
# - libmongoc_definitions
function(mongocxx_add_library TARGET OUTPUT_NAME LINK_TYPE)
    add_library(${TARGET} ${LINK_TYPE}
        ${mongocxx_sources}
    )

    set_target_properties(${TARGET} PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME}
        VERSION ${MONGOCXX_VERSION}
        DEFINE_SYMBOL MONGOCXX_EXPORTS
    )

    if(LINK_TYPE STREQUAL "SHARED")
        set_target_properties(${TARGET} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION _noabi
        )
    endif()

    if(LINK_TYPE STREQUAL "STATIC")
        target_compile_definitions(${TARGET} PUBLIC MONGOCXX_STATIC)
    endif()

    target_link_libraries(${TARGET} PRIVATE ${libmongoc_target})
    target_include_directories(${TARGET} PRIVATE ${libmongoc_include_directories})
    target_include_directories(
        ${TARGET}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/mongocxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/mongocxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib/mongocxx/v_noabi>
    )
    target_compile_definitions(${TARGET} PRIVATE ${libmongoc_definitions})
endfunction(mongocxx_add_library)
