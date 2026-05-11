# AGENTS.md

This file provides guidance to AI coding agents working with code in this repository.

## Build System

This project uses CMake.

The MongoDB C Driver (which provides the required `bson` and `mongoc` libraries) is fetched automatically via `FetchContent` unless an existing installation prefix is found via `find_package()` (set `CMAKE_PREFIX_PATH` accordingly).

Use `build/` as the CMake binary directory unless otherwise specified by the user. When the user specifies a custom binary directory, always use that directory - do not fall back to `build/`.

The typical configure and build steps (for release and installation):

```bash
cmake -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_CXX_STANDARD=17 -B build
cmake --build build
```

The optional install step:

```bash
cmake --install build
```

> [!IMPORTANT]
> For multi-configuration generators (e.g. "Visual Studio *", "Ninja Multi-Config", etc.), use `--config <config>` during the build, install, and test steps instead of `CMAKE_BUILD_TYPE=<config>`.
> The `CMAKE_BUILD_TYPE` option will be ignored by the configuration step.
> Only use `CMAKE_BUILD_TYPE` with single-configuration generators (e.g. Makefile Generators, Ninja, etc.).

Key CMake configuration options (given `option=(default|alternatives...)`):

- `-G <generator-name>`: specify a build system generator.
- `-D CMAKE_PREFIX_PATH:PATH=<mongo-c-driver-prefix>`: specify installation prefixes to search with `find_*()` CMake commands (e.g. to use an existing MongoDB C Driver installation)
- `-D CMAKE_INSTALL_PREFIX:PATH=<install-prefix>`: install directory used by `install()`. Defaults to:
  - The `CMAKE_INSTALL_PREFIX` environment variable when set (with CMake 3.29 or newer).
  - `/usr/local` on UNIX platforms.
  - `C:/Program Files/${PROJECT_NAME}` on Windows.
  - Use `build/install/` as the custom install prefix when system modification is undesirable or disallowed by the user.
- `-D CMAKE_BUILD_TYPE:STRING=<config>`: specify the build type on single-configuration generators.
- `-D CMAKE_CXX_STANDARD:STRING=(11|<cxx-standard>)`: set the C++ standard ("17" or newer is recommended).
- `-D BUILD_SHARED_LIBS:BOOL=(ON|OFF)`: specify whether to build shared (`ON`) or static (`OFF`) libraries.
- `-D BUILD_SHARED_AND_STATIC_LIBS:BOOL=(OFF|ON)`: enable building *both* shared and static libraries (requires `BUILD_SHARED_LIBS=ON`).
- `-D ENABLE_TESTS:BOOL=(OFF|ON)`: required to enable and build test targets.
- `-D BUILD_TESTING:BOOL=(OFF|ON)`: include test targets in the "all" target (when `ENABLE_TESTS=ON`).

**Build performance:** Ninja parallelizes builds across all available cores by default; to cap the job count, set `CMAKE_BUILD_PARALLEL_LEVEL=<N>` in the environment before running `cmake --build`.

> [!NOTE]
> For local development and testing, use the Debug configure in the [Running Tests](#running-tests) section instead of the release configure above.

> [!NOTE]
> `.evergreen/scripts/compile.sh` is the authoritative reference for CI configure-build-install routines. Consult it for details on Ninja generator selection, ccache integration, sanitizer flags (`USE_SANITIZER_ASAN`, `USE_SANITIZER_UBSAN`), polyfill selection (`BSONCXX_POLYFILL`), and other platform-specific options.

## Running Tests

Tests use the Catch2 library and require C++14 or newer.

The typical configure and build steps (for testing and development):

```bash
cmake -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_STANDARD=17 -D ENABLE_TESTS=ON -D BUILD_TESTING=ON -B build
cmake --build build
```

> [!IMPORTANT]
> The "Debug" config type is recommended for local testing and development.
> If the project was already built with a different build type and the user has not requested a change, preserve the existing `CMAKE_BUILD_TYPE` rather than switching to `Debug`, and advise the user that switching to `Debug` is recommended for local testing and development.
> This configure step replaces the release/installation configure above — running both is unnecessary when developing.

Test executables `test_*` are generated under `build/src/bsoncxx/test` and `build/src/mongocxx/test`:

```bash
./build/src/bsoncxx/test/test_bson
./build/src/mongocxx/test/test_driver
./build/src/mongocxx/test/test_unified_format_specs
./build/src/mongocxx/test/test_* # other test executables
```

> [!IMPORTANT]
> For multi-configuration generators (e.g. Visual Studio, Ninja Multi-Config), executables appear under a `<config>/` subdirectory (e.g. `build/src/bsoncxx/test/Debug/test_bson`).

Run a test executable with `--help` to explore available options (e.g. listing test cases and tags, filtering by name/pattern/tags, etc.).

bsoncxx test cases do not require a live MongoDB server.
Most mongocxx test cases require a live MongoDB server. `test_instance` is an exception - it tests instance lifecycle via subprocess and does not connect to a server.

Test executables include:

- `test_bson` (bsoncxx): bsoncxx tests
- `test_driver` (mongocxx): mongocxx tests (includes legacy spec tests and prose tests)
- `test_unified_format_specs` (mongocxx): unified spec test runner
- `test_instance` (mongocxx): `mongocxx::instance` lifecycle tests (initialization, singleton behavior)
- `test_crud_specs` (mongocxx): CRUD spec test runner
- `test_gridfs_specs` (mongocxx): GridFS spec test runner
- `test_command_monitoring_specs` (mongocxx): command monitoring spec test runner
- `test_client_side_encryption_specs` (mongocxx): client-side encryption spec test runner
- `test_transactions_specs` (mongocxx): transactions spec test runner
- `test_retryable_reads_specs` (mongocxx): retryable reads spec test runner
- `test_read_write_concern_specs` (mongocxx): read/write concern spec test runner

Key environment variables controlling test behavior include:

- `*_TESTS_PATH`: path to `data/` subdirectory containing spec test files (e.g. `CRUD_LEGACY_TESTS_PATH`, `COMMAND_MONITORING_TESTS_PATH`).
  - Explore calls to `run_tests_in_suite()` in test code for details.
- `MONGOCXX_TEST_*`: additional test environment variables (TLS, AWS, Azure, GCP credentials, etc.).
  - Integration tests that connect to a live server default to `mongodb://localhost:27017` (hardcoded); no URI env var is required for a standard local setup.
  - `MONGODB_URI` is read by a small number of tests (e.g. search-index tests in `test_driver`) that skip themselves when it is unset.
  - Explore calls to `getenv_or_fail()` and `std::getenv()` in test code for the full list of variables.

## Architecture

The repository provides two libraries, each under `src/<library>/`:

### `bsoncxx`

A standalone BSON document library with no MongoDB dependency. It wraps the `bson` library.

### `mongocxx`

The MongoDB C++ Driver. It wraps the `mongoc` library and depends on the `bsoncxx` library.

### ABI Namespaces

Each library exposes headers under both `v_noabi/` (unstable ABI) and `v1/` (stable ABI) subdirectories. See `etc/coding_guidelines.md` for when to use each.

### C++ Standard

Minimum is C++11. Polyfills for `optional<T>` and `string_view` in `bsoncxx::v1::stdx` are required up to C++17.

Features in newer standards must be guarded by private macros (e.g. `BSONCXX_PRIVATE_INLINE_CXX17`).

## Before Committing

Always format before committing:

```bash
uv run --frozen etc/format.py           # C++ source files.
uv run --frozen etc/ruff-format-all.sh  # Python scripts.
uv run --frozen etc/shfmt-format-all.sh # Shell scripts.
```

`--frozen` ensures the pinned lockfile is used; if it fails with a lockfile error, update it with `uv sync`.

Also run linting when modifying C++ source files:

```bash
bash etc/run-clang-tidy.sh
```

## Code Style

See `etc/coding_guidelines.md` for detailed rules on:

- Directory structure and component design
- ABI versioning and namespace qualification
- Export macros and inline definitions
- Include ordering (IWYU)
- Exception specification (`noexcept` policy)
- Parameter passing and declaration order
