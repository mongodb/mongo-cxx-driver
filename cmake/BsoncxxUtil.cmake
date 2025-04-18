# Define and link a form of the bsoncxx library
#
# This function requires the following variables to be defined in its parent scope:
# - bsoncxx_sources
# - bson_target
function(bsoncxx_add_library TARGET OUTPUT_NAME LINK_TYPE)
    if(NOT DEFINED bsoncxx_sources)
        message(FATAL_ERROR "expected bsoncxx_sources to be defined")
    endif()
    if(NOT DEFINED bson_target)
        message(FATAL_ERROR "expected bson_target to be defined")
    endif()

    add_library(${TARGET} ${LINK_TYPE}
        ${bsoncxx_sources}
    )

    # Full ABI tag string to append to library output name.
    # The value is determined at generator-time when using a multi-config generator.
    # Otherwise, the value is determined at configure-time.
    set(abi_tag "")

    # ABI tag and properties.
    if(1)
        # Enforce ABI compatibility in dependent targets.
        set_property(TARGET ${TARGET} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING
            BSONCXX_ABI_TAG_MONGOC_LINK_TYPE
            BSONCXX_ABI_TAG_POLYFILL_LIBRARY
        )

        # ABI version number. Only necessary for shared library targets.
        if(LINK_TYPE STREQUAL "SHARED")
            set(soversion _noabi)
            set_target_properties(${TARGET} PROPERTIES SOVERSION ${soversion})
            string(APPEND abi_tag "-v${soversion}")
        endif()

        # Build type:
        # - 'd' for debug.
        # - 'r' for release (including RelWithDebInfo and MinSizeRel).
        # - 'u' for unknown (e.g. to allow user-defined configurations).
        # Compatibility is handled via CMake's IMPORTED_CONFIGURATIONS rather than interface properties.
        string(APPEND abi_tag "-$<IF:$<CONFIG:Debug>,d,$<IF:$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>,r,u>>")

        # Link type with bson and mongoc libraries.
        # - 'h' for shared.
        # - 't' for static.
        if(1)
            if(BSONCXX_LINK_WITH_STATIC_MONGOC)
                set(mongoc_link_type "t")
            else()
                set(mongoc_link_type "h")
            endif()

            set_target_properties(${TARGET} PROPERTIES INTERFACE_BSONCXX_ABI_TAG_MONGOC_LINK_TYPE ${mongoc_link_type})

            string(APPEND abi_tag "${mongoc_link_type}")
        endif()

        # Library used for C++17 polyfills:
        # - 'i' for bsoncxx implementations.
        # - 's' for standard library (no polyfill).
        if(1)
            if(BSONCXX_POLY_USE_IMPLS)
                set(polyfill "i")
            elseif(BSONCXX_POLY_USE_STD)
                set(polyfill "s")
            else()
                message(FATAL_ERROR "could not determine polyfill library: must be one of [mbis]")
            endif()

            set_target_properties(${TARGET} PROPERTIES INTERFACE_BSONCXX_ABI_TAG_POLYFILL_LIBRARY ${polyfill})

            string(APPEND abi_tag "${polyfill}")
        endif()

        # MSVC-specific ABI tag suffixes.
        if(MSVC)
            set(vs_suffix "")

            # Include the target architecture if applicable (Win32, x64, etc.).
            if(CMAKE_VS_PLATFORM_NAME)
                string(APPEND vs_suffix "-$<LOWER_CASE:${CMAKE_VS_PLATFORM_NAME}>")
            endif()

            # Include the platform toolset version if applicable (v140, v142, etc.).
            if(CMAKE_VS_PLATFORM_TOOLSET)
                string(APPEND vs_suffix "-$<LOWER_CASE:${CMAKE_VS_PLATFORM_TOOLSET}>")
            endif()

            # Include the C runtime if applicable.
            if(1)
                get_target_property(runtime ${TARGET} MSVC_RUNTIME_LIBRARY)

                set(runtime_str "")

                if(runtime)
                    # MSVC_RUNTIME_LIBRARY may contain generator expressions.
                    # Therefore the comparison must be evaluated during the build generation step.
                    string(APPEND runtime_str "$<$<STREQUAL:${runtime},MultiThreaded>:mt>")
                    string(APPEND runtime_str "$<$<STREQUAL:${runtime},MultiThreadedDebug>:mtd>")
                    string(APPEND runtime_str "$<$<STREQUAL:${runtime},MultiThreadedDLL>:md>")
                    string(APPEND runtime_str "$<$<STREQUAL:${runtime},MultiThreadedDebugDLL>:mdd>")
                else()
                    # Per CMake documentation: if MSVC_RUNTIME_LIBRARY is not set, then
                    # CMake uses the default value MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
                    # to select a MSVC runtime library.
                    set(runtime_str "m$<$<CONFIG:Debug>:d>d")
                endif()

                string(APPEND vs_suffix "-${runtime_str}")
            endif()

            set_target_properties(${TARGET} PROPERTIES BSONCXX_ABI_TAG_VS_SUFFIX ${vs_suffix})

            string(APPEND abi_tag "${vs_suffix}")
        endif()
    endif()

    set_target_properties(${TARGET} PROPERTIES
        VERSION $CACHE{BSONCXX_VERSION}
        DEFINE_SYMBOL BSONCXX_EXPORT
    )

    if(BSONCXX_API_OVERRIDE_DEFAULT_ABI)
        target_compile_definitions(${TARGET} PUBLIC BSONCXX_API_OVERRIDE_DEFAULT_ABI)
    endif()

    if(ENABLE_ABI_TAG_IN_LIBRARY_FILENAMES)
        set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME}${abi_tag})
    else()
        set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
    endif()

    if(LINK_TYPE STREQUAL "SHARED")
        set_target_properties(${TARGET} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
        )
    endif()

    if(LINK_TYPE STREQUAL "STATIC")
        target_compile_definitions(${TARGET} PUBLIC BSONCXX_STATIC)
    endif()

    target_link_libraries(${TARGET} PRIVATE ${bson_target})
    target_include_directories(
        ${TARGET}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/bsoncxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/bsoncxx/v_noabi>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
    )
endfunction(bsoncxx_add_library)
