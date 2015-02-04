# Versions < 2.8.8 do not support Object Libraries
cmake_minimum_required(VERSION 3.1.0)

# Enforce the C++ standard
set (CXX_STANDARD_REQUIRED true)

# Include the required modules
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# Ensure we have Package Config
find_package(PkgConfig)

# Set PKG_CONFIG_PATH
if (DEFINED $ENV{PKG_CONFIG_PATH})
    set(PKG_CONFIG_PATH $ENV{PKG_CONFIG_PATH})
else()
    set(PKG_CONFIG_PATH "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
endif()

# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

option(BUILD_UNIT_TESTS "Build unit tests")

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default is Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -fPIC ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")

add_subdirectory(src)

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
