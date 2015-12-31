project(MONGO_CXX_DRIVER LANGUAGES CXX)

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set (CMAKE_SKIP_BUILD_RPATH false)
set (CMAKE_BUILD_WITH_INSTALL_RPATH false)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

# Add in our modules, we write FindX modules for libbson and libmongoc
# since they don't currently install them.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

# Include the required modules
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadverently pollute /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

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
        -exclude ${CMAKE_CURRENT_SOURCE_DIR}/mongocxx/test ${CMAKE_CURRENT_SOURCE_DIR}/bsoncxx/test
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

set(THIRD_PARTY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party)

enable_testing()

add_subdirectory(src)

add_subdirectory(examples)
