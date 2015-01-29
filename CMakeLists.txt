# TODO: do we need anything out of cmake 3? should be more aggressive?

# Versions < 2.8.8 do not support Object Libraries
cmake_minimum_required(VERSION 2.8.8)

project(mongocxx)

include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)

find_package(PkgConfig)
pkg_check_modules(LIBBSON libbson-${LIBBSON_REQUIRED_ABI_VERSION})
pkg_check_modules(LIBMONGOC libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION})

configure_file (
    ${PROJECT_SOURCE_DIR}/src/driver/version.hpp.in
    ${PROJECT_BINARY_DIR}/src/driver/version.hpp
)

set(CPACK_RESOURCE_FILE_LICENSE
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
)

set(CPACK_PACKAGE_VERSION_MAJOR ${mongocxx_version_major})
set(CPACK_PACKAGE_VERSION_MINOR ${mongocxx_version_minor})

option(BUILD_UNIT_TESTS "Build unit tests")

link_directories(
    ${LIBMONGOC_LIBRARY_DIRS}
)

include_directories(
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(modernize
    clang-modernize
        -p ${CMAKE_BINARY_DIR}/compile_commands.json
        -include ${CMAKE_CURRENT_SOURCE_DIR}
        -exclude ${CMAKE_CURRENT_SOURCE_DIR}/test
        -format
    VERBATIM
)

add_custom_target(format
    perl -nle "/file/ or next; print ((split /: (.*)/)[1])" ${CMAKE_BINARY_DIR}/compile_commands.json |
    grep -v 'catch.hpp' |
    xargs clang-format -i
    VERBATIM
)

add_custom_target(docs
    doxygen
)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default is Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
#set(CMAKE_EXE_LINKER_FLAGS "-lprofiler ${CMAKE_EXE_LINKER_FLAGS}")
#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -ggdb3 -O2 -fsanitize=undefined,address ${CMAKE_CXX_FLAGS}")

if(BUILD_UNIT_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_UNIT_TESTS)

add_subdirectory(src)

