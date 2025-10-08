# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Changes prior to 3.9.0 are documented as [release notes on GitHub](https://github.com/mongodb/mongo-cxx-driver/releases).

## 4.2.0 [Unreleased]

### Added

* To support incremental migration, the following entities are defined as equivalent to their renamed counterparts.
  - `bsoncxx::types::id`: equivalent to `bsoncxx::type`.
  - `bsoncxx::types::binary_subtype`: equivalent to `bsoncxx::binary_sub_type`.
  - `bsoncxx::types::view`: equivalent to `bsoncxx::types::bson_value::view`.
  - `bsoncxx::types::value`: equivalent to `bsoncxx::types::bson_value::value`.
  - `type_view()` in `bsoncxx::document::element` and `bsoncxx::array::element`: equivalent to `get_value()`.
  - `type_value()` in `bsoncxx::document::element` and `bsoncxx::array::element`: equivalent to `get_owning_value()`.

### Fixed

- CMake option `ENABLE_TESTS` (`OFF` by default) is no longer overwritten by the auto-downloaded C Driver (`ON` by default) during CMake configuration.
- `storage_engine() const` in `mongocxx::v_noabi::options::index` is correctly exported using mongocxx export macros instead of bsoncxx export macros.

### Changed

- Bump the minimum required C Driver version to [2.1.0](https://github.com/mongodb/mongo-c-driver/releases/tag/2.1.0).
- CMake 3.16.0 or newer is required when `ENABLE_TESTS=ON` for compatibility with the updated Catch2 library version (3.7.0 -> 3.8.1).
- Minimum supported compiler versions to build from source are updated to the following:
  - GCC 8.1 (from GCC 4.8.2).
    - Users on RHEL 7 may consult Red Hat's ["Hello World - installing GCC on RHEL 7"](https://developers.redhat.com/HW/gcc-RHEL-7) or ["How to install GCC 8 and Clang/LLVM 6 on Red Hat Enterprise Linux 7"](https://developers.redhat.com/blog/2019/03/05/yum-install-gcc-8-clang-6) for instructions on how to obtain GCC 8 or newer.
  - Clang 3.8 (from Clang 3.5).
  - Apple Clang 13.1 with Xcode 13.4.1 (from Apple Clang 5.1 with Xcode 5.1).
  - MSVC 19.0.24210 with Visual Studio 2015 Update 3 (from MSVC 19.0.23506 with Visual Studio 2015 Update 1).
- `mongocxx::v_noabi::instance::~instance()` no longer skips calling `mongoc_cleanup()` when compiled with ASAN enabled.
  - See https://github.com/google/sanitizers/issues/89 for context.
- Bump the minimum required C Driver version to [2.1.2](https://github.com/mongodb/mongo-c-driver/releases/tag/2.1.2).

### Deprecated

- `mongocxx::v_noabi::instance::current()` is "for internal use only". The `instance` constructor(s) should be used instead.
  - Creating the `instance` object in the scope of `main()`, or in an appropriate (non-global) scope such that its (non-static) lifetime is valid for the duration of all other mongocxx library operations, is recommended over the following workarounds.
  - If there is only _one_ call to `current()` present within an application, it may be replaced with a static local variable:
    ```cpp
    // Before:
    mongocxx::instance::current();

    // After:
    static mongocxx::instance instance; // Only ONE instance object!
    ```
  - If there are _multiple_ calls to `current()` present within an application, they may be replaced with a call to a user-defined function containing the static local variable:
    ```cpp
    mongocxx::instance& mongocxx_instance() {
      static mongocxx::instance instance; // Only ONE instance object!
      return instance;
    }
    ```
- These following entities will be renamed (and removed) in an upcoming major release. To support incremental migration, both old and new names are still provided.
  - `bsoncxx::type` -> `bsoncxx::types::id`.
  - `bsoncxx::binary_sub_type` -> `bsoncxx::types::binary_subtype`.
  - `bsoncxx::types::bson_value::view` -> `bsoncxx::types::view`
  - `bsoncxx::types::bson_value::value` -> `bsoncxx::types::value`
  - `get_value()` -> `type_view()` in `bsoncxx::document::element` and `bsoncxx::array::element`.
  - `get_owning_value()` -> `type_value()` in `bsoncxx::document::element` and `bsoncxx::array::element`.

### Removed

- Support for MongoDB Server 4.2.
  - See: [MongoDB Software Lifecycle Schedules](https://www.mongodb.com/legal/support-policy/lifecycles).
  - See: [MongoDB C Driver 2.1.0 Release Notes](https://github.com/mongodb/mongo-c-driver/releases/tag/2.1.0).
- Support for macOS 11 and macOS 12 (deprecated in 4.1.0).

## 3.11.1

### Fixed

- The API version of auto-downloaded C Driver libraries no longer incorrectly inherits the C++ Driver's `BUILD_VERSION` value.

### Changed

- Bump the minimum required C Driver version to [1.30.6](https://github.com/mongodb/mongo-c-driver/releases/tag/1.30.6).

## 4.1.3

### Changed

- Bump the auto-downloaded C Driver version to [2.0.2](https://github.com/mongodb/mongo-c-driver/releases/tag/2.0.2).

## 4.1.2

### Fixed

- `storage_engine() const` in `mongocxx::v_noabi::options::index` is correctly exported using mongocxx export macros instead of bsoncxx export macros.
- Fix `-Wdeprecated-literal-operator` warning.

## 4.1.1

### Fixed

- CMake option `ENABLE_TESTS` (`OFF` by default) is no longer overwritten by the auto-downloaded C Driver (`ON` by default) during CMake configuration.
- Static pkg-config files are updated to depend on the static (not shared) libbson / libmongoc packages.
- Fix build if macros `GCC`/`GNU`/`Clang`/`MSVC` are already defined.

## 4.1.0

### Fixed

- The API version of auto-downloaded C Driver libraries no longer incorrectly inherits the C++ Driver's `BUILD_VERSION` value.
- Potentially unchecked narrowing conversion in `bsoncxx::v_noabi::builder::core::append()` now throws an exception with error code `k_cannot_append_string`.

### Added

- `storage_engine()` in `mongocxx::v_noabi::options::index`.
- Support for sort option with replaceOne and updateOne operations.
- Support for BSON Binary Vector.
- Support $lookup in CSFLE and QE.

### Deprecated

- Support for MacOS 11 (EOL since Nov 2020) and MacOS 12 (EOL since Oct 2021).
- `storage_options()` in `mongocxx::v_noabi::options::index`: use `storage_engine()` instead.
  - `base_storage_options` and `wiredtiger_storage_options` in `mongocxx::v_noabi::options::index` are also deprecated.
- `hedge()` in `mongocxx::v_noabi::read_preference`: hedged reads will no longer be supported by MongoDB.

### Changed

- Bump the minimum required C Driver version to [2.0.0](https://github.com/mongodb/mongo-c-driver/releases/tag/2.0.0).
- Set CMake maximum policy version to 4.0.
  - The CMake minimum required version is still 3.15.
- C++17 polyfill implementations for `optional<T>` and `string_view` are now declared in the `bsoncxx::v1::stdx` namespace.
  - The `bsoncxx::v_noabi::stdx` namespace now contains type aliases to their `v1` equivalents.
  - This is an ABI breaking change for all symbols which reference `bsoncxx::v_noabi::stdx` in their declaration (including mongocxx library symbols).
    - This does not affect users who compile with C++17 or newer and have not set `BSONCXX_POLY_USE_IMPLS=ON`.

## 4.0.0

### Added

- Getter for the `start_at_operation_time` option in `mongocxx::v_noabi::options::change_stream`.

### Changed

- Bump the minimum required C Driver version to [1.29.0](https://github.com/mongodb/mongo-c-driver/releases/tag/1.29.0).
- CMake option `ENABLE_TESTS` is now `OFF` by default.
  - Set `ENABLE_TESTS=ON` to re-enable building test targets.
  - Set `BUILD_TESTING=ON` to include test targets in the "all" target when `ENABLE_TESTS=ON` (since 3.9.0, `OFF` by default).
- Layout of `mongocxx::v_noabi::options::change_stream` to support the new optional `start_at_operation_time` accessor.

### Deprecated

- Support for MongoDB Server 4.0.
  - See: [MongoDB Software Lifecycle Schedules](https://www.mongodb.com/legal/support-policy/lifecycles).

### Removed

- Support for external polyfill libraries.
  - `ENABLE_BSONCXX_POLY_USE_IMPLS=ON` is now implicit behavior.
  - The following CMake options are no longer supported:
    - `ENABLE_BSONCXX_POLY_USE_IMPLS`
    - `BSONCXX_POLY_USE_MNMLSTC`
    - `BSONCXX_POLY_USE_MNMLSTC_SYSTEM`
    - `BSONCXX_POLY_USE_BOOST`
- Support for CMake option `MONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX`.
  - `MONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF` is now implicit behavior.
- Redeclarations of `bsoncxx::stdx` interfaces in the `mongocxx::stdx` namespace.
  - Use `bsoncxx::stdx::optional<T>` instead of `mongocxx::stdx::optional<T>`.
  - Use `bsoncxx::stdx::string_view` instead of `mongocxx::stdx::string_view`.
- Inline namespace macros for bsoncxx and mongocxx namespace: `*_INLINE_NAMESPACE_*`.
- The `<bsoncxx/stdx/make_unique.hpp>` header.
- The `<bsoncxx/types/value.hpp>` header.
- The `<bsoncxx/util/functor.hpp>` header.
- The `<mongocxx/options/create_collection.hpp>` header.
- References to deprecated `utf8` which have equivalent `string` alternatives.
  - `k_utf8` in `bsoncxx::v_noabi::type`. Use `k_string` instead.
  - `b_utf8` in `bsoncxx::v_noabi::types`. Use `b_string` instead.
  - `get_utf8` in `bsoncxx::v_noabi::document::element`, `bsoncxx::v_noabi::array::element`, and `bsoncxx::v_noabi::types::bson_value::view`. Use `get_string` instead.
  - `k_cannot_append_utf8` and `k_need_element_type_k_utf8` in `bsoncxx::v_noabi::exception::error_code`. Use `k_cannot_append_string` and `k_need_element_type_k_string` instead.
- Undocumented using-directives and using-declarations.
  - `bsoncxx::builder::types` in `<bsoncxx/builder/list.hpp>`. Use `bsoncxx::types` in `<bsoncxx/types.hpp>` instead.
  - `bsoncxx::builder::stream::concatenate` in `<bsoncxx/builder/stream/helpers.hpp>`. Use `bsoncxx::builder::concatenate` in `<bsoncxx/builder/concatenate.hpp>` instead.
  - `mongocxx::events::read_preference` in `<mongocxx/events/topology_description.hpp>`. Use `mongocxx::read_preference` in `<mongocxx/read_preference.hpp>` instead.

## 3.11.0

> [!IMPORTANT]
> This is the final v3 minor release. Patch releases containing backports for relevant bug fixes will be supported for up to one year after the first v4 release. New features will not be backported.

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

- Support for MongoDB Server 3.6 (due to the minimum required C Driver version).
  - See: [MongoDB Software Lifecycle Schedules](https://www.mongodb.com/legal/support-policy/lifecycles).
  - See: [MongoDB C Driver 1.28.0 Release Notes](https://github.com/mongodb/mongo-c-driver/releases/tag/1.28.0).
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
