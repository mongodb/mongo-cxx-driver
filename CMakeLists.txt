project(MONGO_CXX_DRIVER LANGUAGES CXX)

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# Add in our modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

# CMake 3.1 doesn't know how to C++11 for clang.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_STANDARD EQUAL 11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif (CMAKE_CXX_STANDARD EQUAL 14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        error("Don't know how to do that standard for Clang")
    endif()
else()
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Include the required modules
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadverently pollute /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Ensure we have Package Config
find_package(PkgConfig)

# Set PKG_CONFIG_PATH
if(DEFINED $ENV{PKG_CONFIG_PATH})
    set(PKG_CONFIG_PATH $ENV{PKG_CONFIG_PATH})
else()
    set(PKG_CONFIG_PATH "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
endif()

# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default is Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(docs
    doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    VERBATIM
)

add_custom_target(modernize
    clang-modernize
        -p ${CMAKE_BINARY_DIR}/compile_commands.json
        -include ${CMAKE_CURRENT_SOURCE_DIR}
        -exclude ${CMAKE_CURRENT_SOURCE_DIR}/mongocxx/test ${CMAKE_CURRENT_SORUCE_DIR}/bsoncxx/test
        -format
    VERBATIM
)

add_custom_target(format
    perl -nle "/file/ or next; print ((split /: (.*)/)[1])" ${CMAKE_BINARY_DIR}/compile_commands.json |
    grep -v "catch.hpp" |
    grep -v "examples" |
    xargs clang-format -i
    VERBATIM
)

enable_testing()

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
