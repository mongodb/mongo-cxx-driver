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

# Required by legacy CMake package config files.
set(BSONCXX_POLY_MNMLSTC_DEPRECATED_INCLUDE_DIRS "")

# Required by pkg-config package config files.
set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_INCLUDE_DIRS "")
set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_STATIC_INCLUDE_DIRS "")

if(BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    set(BSONCXX_POLY_MNMLSTC_DEPRECATED_INCLUDE_DIRS
        "${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc"
    )

    set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_INCLUDE_DIRS
        [[-I${includedir}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc ]]
    )
    set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_STATIC_INCLUDE_DIRS
        [[-I${includedir}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc ]]
    )
endif()

# Generate and install modern CMake package config files.
if(1)
    write_basic_package_version_file(
        bsoncxx-config-version.cmake
        VERSION ${BSONCXX_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_file(
        bsoncxx-config.cmake.in
        bsoncxx-config.cmake
        @ONLY
    )

    install(TARGETS
        ${bsoncxx_target_list}
        EXPORT bsoncxx_targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
        INCLUDES DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/v_noabi
        ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT bsoncxx_targets
        NAMESPACE mongo::
        FILE bsoncxx_targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bsoncxx-${BSONCXX_VERSION}
    )

    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bsoncxx-config-version.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bsoncxx-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bsoncxx-${BSONCXX_VERSION}
        COMPONENT Devel
    )
endif()

# Generate and install pkg-config package config files.
if(1)
    function(bsoncxx_install_pkg_config NAME)
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

    if(BSONCXX_BUILD_SHARED)
        bsoncxx_install_pkg_config(bsoncxx)
    endif()

    if(BSONCXX_BUILD_STATIC)
        bsoncxx_install_pkg_config(bsoncxx-static)
    endif()
endif()

set_dist_list(src_bsoncxx_cmake_DIST
    CMakeLists.txt
    bsoncxx-config.cmake.in
    libbsoncxx-static.pc.in
    libbsoncxx.pc.in
)
