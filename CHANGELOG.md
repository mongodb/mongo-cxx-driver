# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 3.10.0 [Unreleased]

### Added

- Forward headers providing non-defining declarations of bsoncxx and mongocxx class types.
  - Note: user-defined forward declarations of any library entity has not been, and is still not, supported.
    To obtain the declaration or definition of a library entity, always include the corresponding header.

### Removed

- Deprecated CMake package config files.
  - `find_package(libbsoncxx)` and `find_package(libmongocxx)` are no longer supported.
    Use `find_package(bsoncxx)` and `find_package(mongocxx)` instead.
  - Accordingly, `LIBBSONCXX_*` and `LIBMONGOCXX_*` CMake variables provided by the legacy CMake package config files are no longer supported. Use the `mongo::bsoncxx_*` and `mongo::mongocxx_*` CMake targets instead.
  - Note: manually setting compile definitions, include directories, and link libraries is unnecessary with target-based CMake. The former `LIBBSONCXX_*` and `LIBMONGOCXX_*` CMake variables are all superceeded by the `target_link_libraries()` CMake command, which automatically propagates the necessary compile definitions, include directories, and link libraries via target interface properties for `mongo::bsoncxx_*` and `mongo::bsoncxx_*`.

## 3.9.0

### Added

- The C++ driver container image is now available on [Docker hub](https://hub.docker.com/r/mongodb/mongo-cxx-driver).
- Document availability of on [vcpkg](https://vcpkg.io/) and [Conan](https://conan.io/center/recipes/mongo-cxx-driver).
- Add CMake option `MONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX` (default is `TRUE`
  for backwards-compatibility). If enabled, `CMAKE_INSTALL_PREFIX` defaults to the build directory.
- Add API to manage [Atlas Search Indexes](https://www.mongodb.com/docs/atlas/atlas-search/).
- Automatically download C driver dependency if not provided.
- Add VERSIONINFO resource to bsoncxx.dll and mongocxx.dll.

### Changed
- Do not build tests as part of `all` target. Configure with `BUILD_TESTING=ON` to build tests.
- Bump minimum required CMake version to 3.15 to support the FetchContent module and for consistency with the C Driver.
- Improve handling of downloaded (non-system) mnmlstc/core as the polyfill library.
  - Use `FetchContent` instead of `ExternalProject` to download and build the library.
  - Do not patch include directives in mnmlstc/core headers.
- Bump minimum C Driver version to [1.25.0](https://github.com/mongodb/mongo-c-driver/releases/tag/1.25.0).

### Fixed
- Explicitly document that throwing an exception from an APM callback is undefined behavior.
- Do not prematurely install mnmlstc/core headers during the CMake build step.
- Require a C Driver CMake package is found via `find_dependency()` for all installed CXX Driver package configurations.

### Removed
- Remove support for exported targets from the CMake project build tree.
- Drop support for the following operating systems:
  - macOS 10.14 and 10.15
  - Ubuntu 14.04
