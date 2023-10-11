# Copyright 2023 MongoDB Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(CMakePackageConfigHelpers)

# Generate and install modern CMake package config files.
if(1)
    write_basic_package_version_file(
        mongocxx-config-version.cmake
        VERSION ${MONGOCXX_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_file(
        mongocxx-config.cmake.in
        mongocxx-config.cmake
        @ONLY
    )

    install(TARGETS
        ${mongocxx_target_list}
        EXPORT mongocxx_targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/v_noabi
    )

    install(EXPORT mongocxx_targets
        NAMESPACE mongo::
        FILE mongocxx_targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mongocxx-${MONGOCXX_VERSION}
    )

    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/mongocxx-config-version.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/mongocxx-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mongocxx-${MONGOCXX_VERSION}
        COMPONENT Devel
    )
endif()

# Generate and install deprecated CMake package config files.
if(1)
    set(PACKAGE_INCLUDE_INSTALL_DIRS
        ${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/v_noabi
    )
    set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})

    function(mongocxx_install_deprecated_cmake NAME)
        set(PKG "lib${NAME}")

        configure_package_config_file(
            ${PKG}-config.cmake.in
            ${PKG}-config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
            PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
        )

        write_basic_package_version_file(
            ${PKG}-config-version.cmake
            VERSION ${MONGOCXX_VERSION}
            COMPATIBILITY SameMajorVersion
        )

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
        )
    endfunction(mongocxx_install_deprecated_cmake)

    if(MONGOCXX_BUILD_SHARED)
        mongocxx_install_deprecated_cmake(mongocxx)
    endif()

    if(MONGOCXX_BUILD_STATIC)
        mongocxx_install_deprecated_cmake(mongocxx-static)
    endif()
endif()

# Generate and install pkg-config package config files.
if(1)
    function(mongocxx_install_pkg_config NAME)
        set(PKG "lib${NAME}")

        configure_file(
            ${PKG}.pc.in
            ${PKG}.pc
            @ONLY
        )

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PKG}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
            COMPONENT dev
        )
    endfunction()

    if(MONGOCXX_BUILD_SHARED)
        mongocxx_install_pkg_config(mongocxx)
    endif()

    if(MONGOCXX_BUILD_STATIC)
        mongocxx_install_pkg_config(mongocxx-static)
    endif()
endif()

set_dist_list(src_mongocxx_cmake_DIST
    CMakeLists.txt
    libmongocxx-config.cmake.in
    libmongocxx-static-config.cmake.in
    libmongocxx-static.pc.in
    libmongocxx.pc.in
    mongocxx-config.cmake.in
)
