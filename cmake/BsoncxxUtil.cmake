# Define and link a form of the bsoncxx library
#
# This function requires the following variables to be defined in its parent scope:
# - bsoncxx_sources
# - libbson_target
# - libbson_definitions
# - libbson_include_directories
function(bsoncxx_add_library TARGET OUTPUT_NAME LINK_TYPE)
    add_library(${TARGET} ${LINK_TYPE}
        ${bsoncxx_sources}
    )

    set_target_properties(${TARGET} PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME}
        VERSION ${BSONCXX_VERSION}
        DEFINE_SYMBOL BSONCXX_EXPORT
    )

    if(LINK_TYPE STREQUAL "SHARED")
        set_target_properties(${TARGET} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION _noabi
        )
    endif()

    if(LINK_TYPE STREQUAL "STATIC")
        target_compile_definitions(${TARGET} PUBLIC BSONCXX_STATIC)
    endif()

    if(BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
        target_include_directories(
            ${TARGET}
            PUBLIC
            $<BUILD_INTERFACE:${CORE_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc>
        )
    elseif(BSONCXX_POLY_USE_BOOST)
        find_package(Boost 1.56.0 REQUIRED)
        target_link_libraries(${TARGET} PUBLIC Boost::boost)
    endif()

    target_link_libraries(${TARGET} PRIVATE ${libbson_target})
    target_include_directories(${TARGET} PRIVATE ${libbson_include_directories})
    target_include_directories(
        ${TARGET}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/bsoncxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/bsoncxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib/bsoncxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
    )
    target_compile_definitions(${TARGET} PRIVATE ${libbson_definitions})
endfunction(bsoncxx_add_library)
