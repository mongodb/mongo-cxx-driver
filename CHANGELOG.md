# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Changes prior to 3.9.0 are documented as [release notes on GitHub](https://github.com/mongodb/mongo-cxx-driver/releases).

## 3.12.0 [Unreleased]

<!-- Will contain entries for the next minor release. -->

## 3.11.0

### Added

- Support for MongoDB Server version 8.0.
- Stable support for In-Use Encryption Range Indexes.
- Documentation of the [API and ABI versioning and compatibility policy](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/).
- API documentation pages for directories, files, namespaces, and root namespace redeclarations.
- `empty()` member function for `mongocxx::v_noabi::bulk_write`.

### Fixed

- GCC 4.8.5 (RHEL 7) compatibility issues.
  - Redeclaration error due to `bsoncxx::v_noabi::stdx::basic_string_view<...>::npos`.
  - User-defined literal syntax error due to `bsoncxx::v_noabi::operator"" _bson(const char*, size_t)`.
- MSVC 17.11 compatibility issue due to missing `<string>` include directives.
- `bsoncxx::v_noabi::to_json` error handling given invalid BSON documents.
- Client pool error handling on wait queue timeout per `waitQueueTimeoutMS`.

### Changed

- Bump the minimum required C Driver version to [1.28.0](https://github.com/mongodb/mongo-c-driver/releases/tag/1.28.0).
- Declare all exported function symbols with `__cdecl` when compiled with MSVC.
  - This does not affect users who compile with MSVC's default calling convention.
  - This is an ABI breaking change for users who use an alternative default calling convention when building their projects (e.g. with `/Gz`, `/Gv`, etc.). See [Argument Passing and Naming Conventions](https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions) for more information.
- `FetchContent_MakeAvailable()` is used to populate dependencies instead of `FetchContent_Populate()` for the C Driver (when not provided by `CMAKE_PREFIX_PATH`) and mnmlstc/core (when automatically selected or when `BSONCXX_POLY_USE_MNMLSTC=ON`).
  - Note: `FetchContent_Populate()` is still used for mnmlstc/core for CMake versions prior to 3.18 to avoid `add_subdirectory()` behavior.
- Test suite now uses Catch2 v3 via FetchContent instead of the bundled Catch2 v2 standalone header.
  - C++14 or newer is required to build tests when enabled with `ENABLE_TESTS=ON`.
  - Set `ENABLE_TESTS=OFF` to avoid the C++14 requirement when building C++ Driver libraries.

### Deprecated

- The `bsoncxx/util/functor.hpp` header.
- The `bsoncxx::util` namespace.

### Removed

- Support for MongoDB Server 3.6. See [MongoDB Software Lifecycle Schedules](https://www.mongodb.com/legal/support-policy/lifecycles).
- Export of private member functions in the bsoncxx ABI:
  - `bsoncxx::v_noabi::types::bson_value::value::value(const uint8_t*, uint32_t, uint32_t, uint32_t)`
  - `bsoncxx::v_noabi::types::bson_value::view::_init(void*)`
  - `bsoncxx::v_noabi::types::bson_value::view::view(const uint8_t*, uint32_t, uint32_t, uint32_t)`
  - `bsoncxx::v_noabi::types::bson_value::view::view(void*)`
- Export of private member functions in the mongocxx ABI:
  - `mongocxx::v_noabi::options::change_stream::as_bson()`
  - `mongocxx::v_noabi::options::aggregate::append(bsoncxx::v_noabi::builder::basic::document&)`
  - `mongocxx::v_noabi::options::index::storage_options()`

## 3.10.2

### Added

- SSDLC Compliance Report and related release artifacts.

### Fixed

- Undefined behavior when moving a `mongocxx::v_noabi::events::topology_description::server_descriptions` object due to uninitialized data member access.

## 3.10.1

### Fixed

- (MSVC only) The name of the libbsoncxx package in the "Requires" field of the libmongocxx pkg-config file incorrectly used the library output name instead of the pkg-config package name when `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=OFF`.
- (MSVC only) The translation of the `MSVC_RUNTIME_LIBRARY` target property into an ABI tag parameter in library and package filenames did not account for generator expressions.

## 3.10.0

### Added

- Forward headers providing non-defining declarations of bsoncxx and mongocxx class types.
  - Note: user-defined forward declarations of any library entity has not been, and is still not, supported.
    To obtain the declaration or definition of a library entity, always include the corresponding header.
- The CMake option `ENABLE_BSONCXX_USE_POLY_IMPLS` (OFF by default) allows selecting bsoncxx implementations of C++17 polyfills by default when no polyfill library is requested.
- The CMake option `BSONCXX_POLY_USE_IMPLS` (OFF by default) allows selecting bsoncxx implementations of C++17 polyfills instead of external libraries or the C++ standard library.

### Changed

- The `bsoncxx::v_noabi` and `mongocxx::v_noabi` namespaces are no longer declared `inline`.
  - This change is *not* expected to break source or binary compatibility, but is nevertheless documented here due to its significance. If this change does inadvertently break source or binary compatibility, please submit a bug report.
  - Root namespace declarations are still supported and expected to remain equivalent to their prior definitions (e.g. `bsoncxx::document` is still equivalent to `bsoncxx::v_noabi::document`, `mongocxx::client` is still equivalent to `mongocxx::v_noabi::client`, etc.). Argument-dependent lookup and template instantiations are expected to remain equivalent to their prior behavior.
  - Note: user-defined forward declarations of any library entity has not been, and is still not, supported.
    To obtain the declaration or definition of a library entity, always include the corresponding header.
- Library filenames, when compiled with MSVC (as detected by [CMake's MSVC variable](https://cmake.org/cmake/help/v3.15/variable/MSVC.html)), are now embedded with an ABI tag string, e.g. `bsoncxx-v_noabi-rhs-x64-v142-md.lib`.
  - This new behavior is enabled by default; disable by setting `ENABLE_ABI_TAG_IN_LIBRARY_FILENAMES=OFF` when configuring the CXX Driver.
  - The ABI tag string can also be embedded in pkg-config metadata filenames, e.g. `libbsoncxx-v_noabi-rhs-x64-v142-md.pc`. This is disabled by default; enable by setting `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=ON` (requires `ENABLE_ABI_TAG_IN_LIBRARY_FILENAMES=ON`).

### Removed

- Deprecated CMake package config files.
  - `find_package(libbsoncxx)` and `find_package(libmongocxx)` are no longer supported.
    Use `find_package(bsoncxx)` and `find_package(mongocxx)` instead.
  - Accordingly, `LIBBSONCXX_*` and `LIBMONGOCXX_*` CMake variables provided by the legacy CMake package config files are no longer supported. Use the `mongo::bsoncxx_*` and `mongo::mongocxx_*` CMake targets instead.
  - Note: manually setting compile definitions, include directories, and link libraries is unnecessary with target-based CMake. The former `LIBBSONCXX_*` and `LIBMONGOCXX_*` CMake variables are superseded by the `target_link_libraries()` CMake command, which automatically propagates the necessary compile definitions, include directories, and link libraries to dependent targets via `mongo::bsoncxx_*` and `mongo::mongocxx_*` interface properties.
- Experimental C++ standard library as a polyfill option via `BSONCXX_POLY_USE_STD_EXPERIMENTAL`.

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
