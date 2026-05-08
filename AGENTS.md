# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

This project uses CMake. The C driver (`mongoc`) is fetched automatically via `FetchContent` unless already installed. A typical build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DBUILD_TESTING=ON
cmake --build build
```

Key CMake options:
- `ENABLE_TESTS=ON` — required to build test targets
- `BUILD_TESTING=ON` — includes tests in the `all` target (otherwise they're excluded from all)
- `CMAKE_BUILD_TYPE` — defaults to `Release` if unset; use `Debug` when developing
- `BUILD_SHARED_LIBS=OFF` — build static libraries instead of shared
- `BUILD_SHARED_AND_STATIC_LIBS=ON` — build both (only valid with `BUILD_SHARED_LIBS=ON`)

## Running Tests

Tests use Catch2 with `ctest`:

```bash
cd build
ctest --output-on-failure             # all tests
ctest -R bson --output-on-failure     # bsoncxx tests only (test_bson)
ctest -R driver --output-on-failure   # mongocxx tests only (test_driver)
```

To run a specific Catch2 test case directly:

```bash
build/src/bsoncxx/test/test_bson "[tag or test name]" --reporter compact
build/src/mongocxx/test/test_driver "[tag or test name]" --reporter compact
```

The `mongocxx` integration tests require a running MongoDB instance. The `bsoncxx` tests do not.

## Formatting

```bash
uv run --frozen etc/format.py
```

This runs ClangFormat. Always format before committing.

## Commit Messages

Prefix PR titles with the JIRA ticket number: `CXX-XXXX Short description`.

## Architecture

The repository provides two libraries, each under `src/<library>/`:

### `bsoncxx`
A standalone BSON document library with no MongoDB dependency. It wraps the C driver's `libbson`.

### `mongocxx`
The MongoDB C++ driver. It wraps `libmongoc` (the C driver) and depends on `bsoncxx`.

### Directory Layout

Each library follows this structure:

```
src/<library>/
├── include/<library>/      # Public headers (installed as-is)
│   ├── v_noabi/<library>/  # Unstable ABI declarations
│   └── v1/                 # Stable ABI declarations
├── lib/<library>/          # Implementation files (not installed)
│   ├── private/            # Internal utilities shared across ABI versions
│   ├── v_noabi/            # Unstable ABI implementations
│   └── v1/                 # Stable ABI implementations
└── test/                   # Test files (mirrors lib/ layout)
    ├── v_noabi/
    └── v1/
```

### ABI Versioning

The driver has two ABI namespaces:
- `v_noabi` — unstable ABI, subject to change; most existing code lives here
- `v1` — stable ABI, newer additions

Each entity in an ABI namespace (e.g., `mongocxx::v_noabi::client`) is also redeclared in the root namespace (`mongocxx::client`) to let users pick up the latest supported ABI automatically. User code should use root namespace names; internal library code within an ABI namespace must use fully qualified ABI names and never reference root namespace redeclarations (which would break when an entity migrates between ABI versions).

### Component File Conventions

A component `foo` in `v<abi>/foo/` has up to six files:
- `include/.../v<abi>/foo-fwd.hpp` — forward declarations only (class and enum declarations)
- `include/.../v<abi>/foo.hpp` — full declarations (included by users)
- `lib/.../v<abi>/foo.hh` — internal/private header (`.hh` extension marks non-public)
- `lib/.../v<abi>/foo.cpp` — implementation definitions
- `test/.../v<abi>/foo.hh` — test header (Catch2 `StringMaker` specializations)
- `test/.../v<abi>/foo.cpp` — test cases

**Include What You Use** order is enforced: forward header → normal header → internal header → implementation file; each file includes its own header first.

### Export Macros

- `BSONCXX_ABI_EXPORT_CDECL(ReturnType)` — export a free function or operator overload
- `BSONCXX_ABI_EXPORT_CDECL()` — export a constructor or destructor
- `BSONCXX_ABI_EXPORT` — export a class (for polymorphic types only) or variable
- Prefix `MONGOCXX_` variants apply to the mongocxx library

Exported functions must be defined out-of-line in `.cpp` files. Templates must not be exported.

### Namespace Rules

Inside an ABI namespace, always qualify cross-component references with the ABI namespace (e.g., `v_noabi::bar::type`), not the root namespace (`mongocxx::bar::type`), so the reference is not affected by future root namespace redeclarations when `bar::type` migrates to a newer ABI.

### C++ Standard

Minimum is C++11. C++17 features require guards (see `BSONCXX_PRIVATE_INLINE_CXX17`).

### `noexcept` Policy

Follow "Policy Statement E — Minimal `noexcept`": only mark constructors or UDCFs `noexcept` when their nothrow guarantee is queried for algorithm selection (e.g., `std::is_nothrow_move_constructible`).
