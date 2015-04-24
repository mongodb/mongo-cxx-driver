# Try to find bsoncxx.

# TODO: currently this is just a shim around pkg-config. We will have to do 
# something for windows eventually.

# These vars will be defined:
# BSONCXX_FOUND - System has bsoncxx
# BSONCXX_VERSION - The version of bsoncxx we found
# BSONCXX_INCLUDE_DIRS - The bsoncxx include directories - pass this to include_directories
# BSONCXX_LIBRARY_DIRS - The bsoncxx library directories - pass this to link_directories
# BSONCXX_LIBRARIES - The libraries needed to use bsoncxx - pass this to target_link_libraries
# BSONCXX_DEFINITIONS - The compiler switches required for using bsoncxx

find_package(PkgConfig)
pkg_check_modules(BSONCXX QUIET libbsoncxx)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(BSONCXX
    REQUIRED_VARS
      BSONCXX_FOUND
      BSONCXX_INCLUDE_DIRS
      BSONCXX_LIBRARY_DIRS
      BSONCXX_LIBRARIES
    VERSION_VAR
      BSONCXX_VERSION
    FAIL_MESSAGE
      "bsoncxx is not installed on this system!"
)
