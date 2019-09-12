# Define and link a form of the bsoncxx library
#
# This function requires the following variables to be defined in its parent scope:
# - bsoncxx_sources
# - libbson_include_directories
# - libbson_definitions
# - libbson_libraries
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
            SOVERSION ${BSONCXX_ABI_VERSION}
        )
    endif()

    if(LINK_TYPE STREQUAL "STATIC")
        target_compile_definitions(bsoncxx_static PUBLIC BSONCXX_STATIC)
    endif()

    if(BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
        add_dependencies(${TARGET} EP_mnmlstc_core)
        ExternalProject_Get_Property(EP_mnmlstc_core source_dir)
        target_include_directories(${TARGET} PUBLIC ${source_dir}/include)
    elseif(BSONCXX_POLY_USE_BOOST)
        target_link_libraries(${TARGET} PUBLIC Boost::boost)
    endif()

    target_include_directories(${TARGET} PRIVATE ${libbson_include_directories})
    target_compile_definitions(${TARGET} PRIVATE ${libbson_definitions})
    target_link_libraries(${TARGET} PRIVATE ${libbson_libraries})

    generate_export_header(${TARGET}
        BASE_NAME BSONCXX
        EXPORT_MACRO_NAME BSONCXX_API
        NO_EXPORT_MACRO_NAME BSONCXX_PRIVATE
        EXPORT_FILE_NAME config/export.hpp
        STATIC_DEFINE BSONCXX_STATIC
    )
endfunction(bsoncxx_add_library)

# Install a form of the bsoncxx library with associated CMake config files
function(bsoncxx_install TARGET)
    install(TARGETS
        ${TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
    )
    get_target_property(PKG ${TARGET} OUTPUT_NAME)
    set(PKG "lib${PKG}")

    configure_package_config_file(
      cmake/${PKG}-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${BSONCXX_VERSION}
      PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
    )

    write_basic_package_version_file(
      ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
      VERSION ${BSONCXX_VERSION}
      COMPATIBILITY SameMajorVersion
    )

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${BSONCXX_VERSION}
    )
endfunction(bsoncxx_install)
