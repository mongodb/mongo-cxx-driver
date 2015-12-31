# Find libmongo-c, either via pkg-config, find-package in config mode,
# or other less admirable jiggery-pokery

SET(LIBMONGOC_DIR "" CACHE STRING "Manual search path for libmongoc")

include(FindPackageHandleStandardArgs)

# Load up PkgConfig if we have it
find_package(PkgConfig QUIET)

if (PKG_CONFIG_FOUND)
  pkg_check_modules(LIBMONGOC REQUIRED libmongoc-1.0>=${LibMongoC_FIND_VERSION} )
  # We don't reiterate the version information here because we assume that
  # pkg_check_modules has honored our request.
  find_package_handle_standard_args(LIBMONGOC DEFAULT_MSG LIBMONGOC_FOUND)
elseif(LIBMONGOC_DIR)
  # The best we can do until libMONGOC starts installing a libmongoc-config.cmake file
  set(LIBMONGOC_LIBRARIES mongoc-1.0 CACHE INTERNAL "")
  set(LIBMONGOC_LIBRARY_DIRS ${LIBMONGOC_DIR}/lib CACHE INTERNAL "")
  set(LIBMONGOC_INCLUDE_DIRS ${LIBMONGOC_DIR}/include/libMONGOC-1.0 CACHE INTERNAL "")
  find_package_handle_standard_args(LIBMONGOC DEFAULT_MSG LIBMONGOC_LIBRARIES LIBMONGOC_LIBRARY_DIRS LIBMONGOC_INCLUDE_DIRS)
else()
    message(FATAL_ERROR "Don't know how to find libmongoc; please set LIBMONGOC_DIR to the prefix directory with which libbson was configured.")
endif()
