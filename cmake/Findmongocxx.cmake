# Try to find mongocxx.

# TODO: currently this is just a shim around pkg-config. We will have to do 
# something for windows eventually.

# These vars will be defined:
# MONGOCXX_FOUND - System has mongocxx
# MONGOCXX_VERSION - The version of mongocxx we found
# MONGOCXX_INCLUDE_DIRS - The mongocxx include directories - pass this to include_directories
# MONGOCXX_LIBRARY_DIRS - The mongocxx library directories - pass this to link_directories
# MONGOCXX_LIBRARIES - The libraries needed to use mongocxx - pass this to target_link_libraries
# MONGOCXX_DEFINITIONS - The compiler switches required for using mongocxx

find_package(PkgConfig)
pkg_check_modules(MONGOCXX QUIET libmongocxx)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MONGOCXX
    REQUIRED_VARS
      MONGOCXX_FOUND
      MONGOCXX_INCLUDE_DIRS
      MONGOCXX_LIBRARY_DIRS
      MONGOCXX_LIBRARIES
    VERSION_VAR
      MONGOCXX_VERSION
    FAIL_MESSAGE
      "mongocxx is not installed on this system!"
)
