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

set(bsoncxx_sources_private
    bsoncxx/private/itoa.cpp
    bsoncxx/private/version.cpp
)

set(bsoncxx_sources_v_noabi
    bsoncxx/v_noabi/bsoncxx/array/element.cpp
    bsoncxx/v_noabi/bsoncxx/array/value.cpp
    bsoncxx/v_noabi/bsoncxx/array/view.cpp
    bsoncxx/v_noabi/bsoncxx/builder/core.cpp
    bsoncxx/v_noabi/bsoncxx/config/config.cpp
    bsoncxx/v_noabi/bsoncxx/config/export.cpp
    bsoncxx/v_noabi/bsoncxx/config/version.cpp
    bsoncxx/v_noabi/bsoncxx/decimal128.cpp
    bsoncxx/v_noabi/bsoncxx/document/element.cpp
    bsoncxx/v_noabi/bsoncxx/document/value.cpp
    bsoncxx/v_noabi/bsoncxx/document/view.cpp
    bsoncxx/v_noabi/bsoncxx/exception/error_code.cpp
    bsoncxx/v_noabi/bsoncxx/exception/exception.cpp
    bsoncxx/v_noabi/bsoncxx/json.cpp
    bsoncxx/v_noabi/bsoncxx/oid.cpp
    bsoncxx/v_noabi/bsoncxx/string/view_or_value.cpp
    bsoncxx/v_noabi/bsoncxx/types.cpp
    bsoncxx/v_noabi/bsoncxx/types/bson_value/value.cpp
    bsoncxx/v_noabi/bsoncxx/types/bson_value/view.cpp
    bsoncxx/v_noabi/bsoncxx/validate.cpp
    bsoncxx/v_noabi/bsoncxx/vector.cpp
)

set(bsoncxx_sources_v1
    bsoncxx/v1/config/config.cpp
    bsoncxx/v1/config/export.cpp
    bsoncxx/v1/config/version.cpp
    bsoncxx/v1/detail/postlude.cpp
    bsoncxx/v1/detail/prelude.cpp
)

list(APPEND bsoncxx_sources
    ${bsoncxx_sources_private}
    ${bsoncxx_sources_v_noabi}
    ${bsoncxx_sources_v1}
)
list(TRANSFORM bsoncxx_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
set(bsoncxx_sources "${bsoncxx_sources}" PARENT_SCOPE)

# Generate private headers.
configure_file(
    bsoncxx/private/config/config.hh.in
    bsoncxx/private/config/config.hh
)

# Generate and install public headers.
if(1)
    configure_file(
        bsoncxx/v1/config/config.hpp.in
        bsoncxx/v1/config/config.hpp
    )

    configure_file(
        bsoncxx/v1/config/version.hpp.in
        bsoncxx/v1/config/version.hpp
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bsoncxx/v1/config/config.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/bsoncxx/v1/config/version.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/v1/config
        COMPONENT dev
    )
endif()

set_dist_list(src_bsoncxx_lib_DIST
    CMakeLists.txt
    ${bsoncxx_sources_private}
    ${bsoncxx_sources_v_noabi}
    ${bsoncxx_sources_v1}
    bsoncxx/private/b64_ntop.hh
    bsoncxx/private/config/config.hh.in
    bsoncxx/private/convert.hh
    bsoncxx/private/export.hh
    bsoncxx/private/helpers.hh
    bsoncxx/private/itoa.hh
    bsoncxx/private/bson.hh
    bsoncxx/private/make_unique.hh
    bsoncxx/private/stack.hh
    bsoncxx/private/suppress_deprecation_warnings.hh
    bsoncxx/private/version.hh
    bsoncxx/v_noabi/bsoncxx/types/bson_value/value.hh
    bsoncxx/v1/config/config.hpp.in
    bsoncxx/v1/config/version.hpp.in
)
