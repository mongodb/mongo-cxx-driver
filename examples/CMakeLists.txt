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
    set(single_args "")
    set(multi_args "LIBRARIES")

    cmake_parse_arguments(PARSED "${opt_args}" "${single_args}" "${multi_args}" ${ARGN})

    if(NOT "${PARSED_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "unrecognized argument: ${PARSED_UNPARSED_ARGUMENTS}")
    endif()

    # path/to/source.cpp -> source
    get_filename_component(name ${source} NAME_WE)

    # path/to/source.cpp -> path/to
    get_filename_component(subdir ${source} DIRECTORY)

    add_executable(${name} EXCLUDE_FROM_ALL ${source})
    target_link_libraries(${name} PRIVATE ${PARSED_LIBRARIES})

    # Keep executables in separate directories.
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${subdir})

    # Deliberately use `__vectorcall` by default with MSVC to catch missing `__cdecl`.
    target_compile_options(${name} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Gv>")

    # Keep build and run targets completely separate.
    add_dependencies(examples ${name})
    add_custom_target(run-examples-${name} COMMAND ${name})
    add_dependencies(run-examples run-examples-${name})
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

foreach(source ${bsoncxx_examples_sources})
    add_examples_executable(${source} LIBRARIES bsoncxx_target)
endforeach()

foreach(source ${mongocxx_examples_sources})
    set(extra_libs "")

    # Don't run the CSFLE examples as part of the suite; needs mongocryptd running.
    if (${source} MATCHES "encryption")
        continue()
    endif()

    if (${source} MATCHES "pool")
        list(APPEND extra_libs Threads::Threads)
    endif()

    add_examples_executable(${source} LIBRARIES mongocxx_target ${extra_libs})
endforeach()

set_local_dist (examples_DIST_local
   CMakeLists.txt
   ${bsoncxx_examples_sources}
   ${mongocxx_examples_sources}
   README.md
   add_subdirectory/.gitignore
   add_subdirectory/CMakeLists.txt
   add_subdirectory/hello_mongocxx.cpp
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
