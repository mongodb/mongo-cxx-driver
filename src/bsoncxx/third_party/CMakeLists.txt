
include(ExternalProject)

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)

    ExternalProject_Add(
        EP_mnmlstc_core
        GIT_REPOSITORY https://github.com/mnmlstc/core
        GIT_TAG v1.1.0
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        LOG_UPDATE ON
        UPDATE_COMMAND ""
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_TESTING=Off -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx/third_party/mnmlstc -DINCLUDE_INSTALL_DIR=.
    )

    ExternalProject_Add_Step(
        EP_mnmlstc_core
        fix-includes
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx/third_party/mnmlstc
        COMMAND find . -type f | xargs perl -pi -e "s|#include <core|#include <bsoncxx/third_party/mnmlstc/core|g"
        DEPENDEES install
    )

endif()

