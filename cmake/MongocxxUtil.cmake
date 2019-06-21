# Define and link a form of the mongocxx library
#
# This function requires the following variables to be defined in its parent scope:
# - mongocxx_sources
# - libmongoc_include_directories
# - libmongoc_definitions
# - libmongoc_libraries
function(mongocxx_add_library TARGET OUTPUT_NAME LINK_TYPE)
    add_library(${TARGET} ${LINK_TYPE}
        ${mongocxx_sources}
    )

    set_target_properties (${TARGET} PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME}
        VERSION ${MONGOCXX_VERSION}
        DEFINE_SYMBOL MONGOCXX_EXPORTS
    )

    if(LINK_TYPE STREQUAL "SHARED")
        set_target_properties (${TARGET} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION ${MONGOCXX_VERSION_NO_EXTRA}
        )
    endif()

    if(LINK_TYPE STREQUAL "STATIC")
        target_compile_definitions(${TARGET} PUBLIC MONGOCXX_STATIC)
    endif()

    target_include_directories(${TARGET} PRIVATE ${libmongoc_include_directories})
    target_compile_definitions(${TARGET} PRIVATE ${libmongoc_definitions})
    target_link_libraries(${TARGET} PRIVATE ${libmongoc_libraries})

    generate_export_header(${TARGET}
        BASE_NAME MONGOCXX
        EXPORT_MACRO_NAME MONGOCXX_API
        NO_EXPORT_MACRO_NAME MONGOCXX_PRIVATE
        EXPORT_FILE_NAME config/export.hpp
        STATIC_DEFINE MONGOCXX_STATIC
    )
endfunction(mongocxx_add_library)

# Install a form of the mongocxx library with associated CMake config files
function(mongocxx_install TARGET)
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
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
      PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
    )

    write_basic_package_version_file(
      ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
      VERSION ${MONGOCXX_VERSION}
      COMPATIBILITY SameMajorVersion
    )

    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
    )
endfunction(mongocxx_install)
