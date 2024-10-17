# Copyright 2009-present MongoDB, Inc.
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

add_custom_target(examples)
add_custom_target(run-examples)

add_custom_target(run-api-examples)
add_custom_target(run-bsoncxx-examples)
add_custom_target(run-mongocxx-examples)

add_dependencies(run-examples
    run-api-examples
    run-bsoncxx-examples
    run-mongocxx-examples
)

if (TARGET bsoncxx_static)
    add_library(bsoncxx_target ALIAS bsoncxx_static)
else()
    add_library(bsoncxx_target ALIAS bsoncxx_shared)
endif()

if (TARGET mongocxx_static)
    add_library(mongocxx_target ALIAS mongocxx_static)
else()
    add_library(mongocxx_target ALIAS mongocxx_shared)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

function(add_examples_executable source)
    set(opt_args "")
    set(single_args "ADD_TO_RUN_EXAMPLES")
    set(multi_args "LIBRARIES")

    cmake_parse_arguments(PARSED "${opt_args}" "${single_args}" "${multi_args}" ${ARGN})

    if(NOT "${PARSED_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "unrecognized argument: ${PARSED_UNPARSED_ARGUMENTS}")
    endif()

    if ("${PARSED_ADD_TO_RUN_EXAMPLES}" STREQUAL "")
        set(PARSED_ADD_TO_RUN_EXAMPLES "ON") # Default to ON.
    endif ()

    # path/to/source.cpp -> source
    get_filename_component(name ${source} NAME_WE)

    # path/to/source.cpp -> path/to
    get_filename_component(subdir ${source} DIRECTORY)

    # path/to/source -> path-to-source
    string(REPLACE "/" "-" target_name "${subdir}/${name}")
    add_executable(${target_name} EXCLUDE_FROM_ALL ${source})
    target_link_libraries(${target_name} PRIVATE ${PARSED_LIBRARIES})

    # Permit `#include <examples/macros.hh>`.
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

    # Keep executables in separate directories.
    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME ${name}
        RUNTIME_OUTPUT_DIRECTORY ${subdir}
    )

    # Use `__vectorcall` by default with MSVC to catch missing `__cdecl`.
    target_compile_options(${target_name} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Gv>")

    # Keep build and run targets completely separate.
    add_dependencies(examples ${target_name})

    # Use full path to avoid implicit build dependency.
    if (isMultiConfig)
        add_custom_target(run-examples-${target_name} COMMAND "${subdir}/$<CONFIG>/${name}")
    else ()
        add_custom_target(run-examples-${target_name} COMMAND "${subdir}/${name}")
    endif ()

    set (type "")
    if (${source} MATCHES "^api/")
        set (type "api")
    elseif (${source} MATCHES "^bsoncxx/")
        set (type "bsoncxx")
    elseif (${source} MATCHES "^mongocxx/")
        set (type "mongocxx")
    else ()
        message (FATAL_ERROR "unexpected subdirectory ${subdir}")
    endif ()

    if (PARSED_ADD_TO_RUN_EXAMPLES)
        add_dependencies(run-${type}-examples run-examples-${target_name})
    endif ()
endfunction()

file(GLOB_RECURSE bsoncxx_examples_sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "bsoncxx/*.cpp"
)

file(GLOB_RECURSE mongocxx_examples_sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "mongocxx/*.cpp"
)

file(GLOB_RECURSE examples_headers
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "*.hh"
)


foreach(source ${bsoncxx_examples_sources})
    add_examples_executable(${source} LIBRARIES bsoncxx_target)
endforeach()

foreach(source ${mongocxx_examples_sources})
    set(extra_libs "")
    set(add_to_run_examples ON)

    if (${source} MATCHES "pool")
        list(APPEND extra_libs Threads::Threads)
    endif()

    # Don't run the CSFLE examples as part of the suite; needs mongocryptd running.
    if (${source} MATCHES "encryption")
        set(add_to_run_examples OFF)
    endif()

    add_examples_executable(${source}
        LIBRARIES mongocxx_target ${extra_libs}
        ADD_TO_RUN_EXAMPLES ${add_to_run_examples}
    )
endforeach()

file(GLOB_RECURSE api_examples_sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "api/*.cpp"
)
list(REMOVE_ITEM api_examples_sources "api/runner.cpp")

function (add_abi_examples)
    # Avoid MAX_PATH errors on Windows due to long target names by using a single `api-runner` .
    add_examples_executable(api/runner.cpp LIBRARIES mongocxx_target Threads::Threads)

    # Define a unique entry function name per component.
    foreach (source ${api_examples_sources})
        target_sources (api-runner PRIVATE ${source})

        # path/to/source.cpp -> source
        get_filename_component(name ${source} NAME_WE)

        # path/to/source.cpp -> path/to
        get_filename_component(subdir ${source} DIRECTORY)

        # path/to/source -> path_to_source
        string(REPLACE "/" "_" component_name "${subdir}/${name}")

        # Define a unique component name for each component to be registered with the runner.
        set_property (SOURCE ${source} APPEND PROPERTY COMPILE_DEFINITIONS "EXAMPLES_COMPONENT_NAME=${component_name}")
    endforeach ()

    # Silence warnings for common patterns in API examples.
    set_property (TARGET api-runner APPEND PROPERTY COMPILE_OPTIONS
        # Allow unused variables.
        $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wno-unused>
        $<$<CXX_COMPILER_ID:MSVC>:/wd4189>

        # Allow simpler switch statements.
        $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wno-switch>
    )
endfunction ()
add_abi_examples ()

set_local_dist (examples_DIST_local
   CMakeLists.txt
   ${api_examples_sources}
   ${bsoncxx_examples_sources}
   ${mongocxx_examples_sources}
   ${examples_headers}
   README.md
   add_subdirectory/.gitignore
   add_subdirectory/CMakeLists.txt
   add_subdirectory/hello_mongocxx.cpp
   api/runner.cpp
   projects/bsoncxx/cmake/shared/build.sh
   projects/bsoncxx/cmake/shared/build/.gitignore
   projects/bsoncxx/cmake/shared/CMakeLists.txt
   projects/bsoncxx/cmake/static/build.sh
   projects/bsoncxx/cmake/static/build/.gitignore
   projects/bsoncxx/cmake/static/CMakeLists.txt
   projects/bsoncxx/hello_bsoncxx.cpp
   projects/bsoncxx/pkg-config/shared/build.sh
   projects/bsoncxx/pkg-config/shared/build/.gitignore
   projects/bsoncxx/pkg-config/static/build.sh
   projects/bsoncxx/pkg-config/static/build/.gitignore
   projects/mongocxx/cmake/shared/build.sh
   projects/mongocxx/cmake/shared/build/.gitignore
   projects/mongocxx/cmake/shared/CMakeLists.txt
   projects/mongocxx/cmake/static/build.sh
   projects/mongocxx/cmake/static/build/.gitignore
   projects/mongocxx/cmake/static/CMakeLists.txt
   projects/mongocxx/hello_mongocxx.cpp
   projects/mongocxx/pkg-config/shared/build.sh
   projects/mongocxx/pkg-config/shared/build/.gitignore
   projects/mongocxx/pkg-config/static/build.sh
   projects/mongocxx/pkg-config/static/build/.gitignore
)

set (examples_DIST
   ${examples_DIST_local}
   PARENT_SCOPE
)
