+++
date = "2016-08-15T16:11:58+05:30"
title = "API and ABI versioning"
[menu.main]
  weight = 20
  parent="mongocxx3"
+++

For brevity, this page may describe properties and features as-if they only apply to the bsoncxx library.
Unless stated otherwise, one may assume the properties and features described are similarly applicable to the mongocxx library.

## API Versioning

See [API Versioning]({{< relref "policies/api-versioning" >}}).

## ABI Versioning

See [ABI Versioning]({{< relref "policies/abi-versioning" >}}).

## Header File Structure

The public header files of the bsoncxx library are organized by ABI namespace:

```
${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/
└── bsoncxx/
    ├── v_noabi/bsoncxx/
    │   └── ...
    ├── v1/bsoncxx/
    │   └── ...
    ├── v2/bsoncxx/
    │   └── ...
    └── vN/bsoncxx/
        └── ...
```

For forward compatibility, a public header file in an ABI namespace directory `vA` may include a header file in a _newer_ ABI namespace directory `vB`, where `A < B`.
Such forward compatibility include directives, when supported, will be explicitly documented.
For any given public API entity, we recommend including its corresponding header from the _latest_ ABI namespace directory whenever available.

**IMPORTANT:** For backward compatibility, the `bsoncxx/v_noabi/` ABI namespace directory is added to include paths such that `#include <bsoncxx/example.hpp>` is equivalent to `#include <bsoncxx/v_noabi/bsoncxx/example.hpp>`. Relying on this behavior is discouraged: we recommend explicitly including `#include <bsoncxx/v_noabi/bsoncxx/example.hpp>` instead.

**NOTE:** The stability of an _undocumented_ include directive is not supported.

Package files are installed to subdirectories in `${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/`.
Users are strongly encouraged to use these package files to obtain bsoncxx library header (as well for linking and the setting of compile flags).
Manual configuration of include paths to obtain bsoncxx header files is not recommended.
Package files are described further below.

In all cases, **CMake package config files are the recommended method for importing CXX Driver libraries.**

## Library Filenames

The filename and presence of symlinks depends on the platform and toolchain used to build the library, as well as the library type (shared vs. static).

### Shared Libraries

**IMPORTANT:** Shared libraries on Windows when configured with the MSVC toolchain on Windows are treated differently due to Windows-specific special considerations. See "Shared Libraries (MSVC Only)" below.

The "real name" of shared libraries use the following pattern:

```
lib<basename>.so.<api-version-number>
```

where:

* `<basename>` corresponds to the `BSONCXX_OUTPUT_BASENAME` CMake configuration variable (`MONGOCXX_OUTPUT_BASENAME` for the mongocxx library). By default, this is set to `bsoncxx` and `mongocxx`. The basename of a static library is suffixed with `-static`.
* `<api-version-number>` is the MAJOR, MINOR, and PATCH version for the given build configuration, corresponding to the `BUILD_VERSION` CMake configuration variable. (Note: explicitly setting this configuration variable should not be necessary in regular use of the CXX Driver.)
* API version number extensions are _not_ included in the real name (e.g. the `-alpha` in `1.2.3-alpha`).

Examples of expected library real names include:

```
libbsoncxx.so.1.0.0
libbsoncxx.so.2.3.4
```

CMake's `SOVERSION` target property and "namelink" behavior is used to also install symlinks to the library directory consistent with the Linux soname convention.
These symlinks include the ABI version number as part of the library soname.

For example, the following lib directory contains the expected library files for a bsoncxx library with API version `1.2.3` and ABI version `10`, where `a -> b` indicates `a` is a symlink to `b`:

```
${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/
├── libbsoncxx.so.1.2.3
├── libbsoncxx.so.10 -> libbsoncxx.so.1.2.3
├── libbsoncxx.so -> libbsoncxx.so.10
├── cmake/bsoncxx-1.2.3/
│   ├── bsoncxx-config.cmake
│   └── ...
└── pkgconfig/
    └── libbsoncxx.pc
```

The name of the CMake package for bsoncxx libraries is controlled by `BSONCXX_OUTPUT_BASENAME` (`MONGOCXX_OUTPUT_BASENAME` for mongocxx libraries).
They should be imported according to CMake's [Config Mode Search Procedure](https://cmake.org/cmake/help/latest/command/find_package.html#config-mode-search-procedure).
For example, assuming the CXX Driver was installed to a prefix `$INSTALLDIR`:

```bash
# The CMake package may be imported via CMAKE_PREFIX_PATH (recommended)...
cmake -S example -B example-build -D CMAKE_PREFIX_PATH="$INSTALLDIR"

# Or via <PackageName>_ROOT (assuming BSONCXX_OUTPUT_NAME=bsoncxx)...
cmake -S example -B example-build -D bsoncxx_ROOT="$INSTALLDIR"

# Or via `PATHS` in the `find_package()` command... or etc.
# find_package(bsoncxx CONFIG REQUIRED PATHS "$INSTALLDIR")
```

**NOTE:** The C Driver is a required dependency of CXX Driver. The CMake package config files for the C Driver may also need to be imported using the same (or similar) approach to satisfy this dependency.

Although the CXX Driver also provides pkg-config files, users are strongly encouraged to use CMake package config files instead.

The name of the pkg-config file for bsoncxx libraries is also controlled by `BSONCXX_OUTPUT_NAME` and follow the pattern `lib<basename>.pc`.
Because the CXX Driver does not install pkg-config files to typical pkg-config system directories, the `PKG_CONFIG_PATH` environment variable may need to be used to augment pkg-config's search path, e.g.:

```bash
bsoncxx_cflags="$(PKG_CONFIG_PATH="$INSTALLDIR" pkg-config --cflags "libbsoncxx >= 1.2.3")"
bsoncxx_ldflags="$(PKG_CONFIG_PATH="$INSTALLDIR" pkg-config --libs "libbsoncxx >= 1.2.3")"

g++ $bsoncxx_cflags -c example-a.cpp
g++ $bsoncxx_cflags -c example-b.cpp
g++ $bsoncxx_ldflags -o example example-a.o example-b.o
```

### Shared Libraries (MSVC Only)

Since version 3.10.0, the shared libraries when built with the MSVC toolchain on Windows (even when the CMake generator is not Visual Studio) use a different naming scheme from other platforms.
To restore prior behavior, which is similar to other platforms, set `ENABLE_ABI_TAG_IN_LIBRARY_FILENAMES=OFF`.

The name of shared libraries (aka the "output name") use the following pattern (file extension excluded):

```
<basename>-v<abi-version-number>-<abi-tag>
```

where:

* `<basename>` corresponds to the `BSONCXX_OUTPUT_BASENAME` CMake configuration variable (`MONGOCXX_OUTPUT_BASENAME` for the mongocxx library). By default, this is set to `bsoncxx` and `mongocxx`. The basename of a static library is suffixed with `-static`.
* `<abi-version-number>` corresponds to the current ABI version number.
* `<abi-tag>` describes known properties that affect the binary compatibility of the shared library.

The `<abi-tag>` is a triplet of letters indicating:

* Build Type
* mongoc Link Type
* Polyfill Library

followed by a suffix describing the toolset and runtime library used to build the library.
The exact contents of the suffix depend on the build configuration.

Examples of expected library filenames (with a brief description of notable features) include:

* `bsoncxx-v_noabi-rhs-x64-v142-md.dll` (release build configuration)
* `bsoncxx-v_noabi-dhs-x64-v142-mdd.dll` (debug build configuration)
* `bsoncxx-v_noabi-rts-x64-v142-md.dll` (link with mongoc static libraries)
* `bsoncxx-v_noabi-rhi-x64-v142-md.dll` (bsoncxx as polyfill library)
* `bsoncxx-v_noabi-rhm-x64-v142-md.dll` (mnmlstc/core as polyfill library)
* `bsoncxx-v_noabi-rhb-x64-v142-md.dll` (Boost as polyfill library)
* `bsoncxx-v1-rhs-x64-v142-md.dll`      (ABI version number 1)
* `bsoncxx-v2-rhs-x64-v142-md.dll`      (ABI version number 2)

**NOTE:** This example also applies to the companion `.lib` file.

This naming scheme allows the bsoncxx library to be built and installed with different build configurations (e.g. Debug vs. Release) and different runtime library requirements (e.g. MultiThreadedDLL vs. MultiThreadedDebugDLL) in parallel and without conflict.
See references to `ENABLE_ABI_TAG_IN_LIBRARY_FILENAMES` and related code in the CMake configuration for more details.

For example, the following install prefix directory contains the expected debug _and_ release library files for a bsoncxx library with API version `1.2.3` and ABI version `10` (the bin directory is included to account for `.dll` files):

```
${CMAKE_INSTALL_PREFIX}/
├── ${CMAKE_INSTALL_BINDIR}/
│   ├── bsoncxx-v10-dhs-x64-v142-mdd.dll
│   └── bsoncxx-v10-rhs-x64-v142-md.dll
└── ${CMAKE_INSTALL_LIBDIR}/
    ├── bsoncxx-v10-dhs-x64-v142-mdd.lib
    ├── bsoncxx-v10-rhs-x64-v142-md.lib
    ├── cmake/bsoncxx-1.2.3/
    │   ├── bsoncxx-config.cmake
    │   └── ...
    └── pkgconfig/
        ├── libbsoncxx-v10-dhs-x64-v142-mdd.pc
        └── libbsoncxx-v10-rhs-x64-v142-md.pc
```

The CMake package config files are the same as the regular (non-MSVC) shared library behavior described above.

**NOTE:** CMake automatically handles selection of libraries according to build type, but does not enforce build type consistency when only one build type is installed. Installing _both_ Debug and Release configurations on Windows is highly recommended for this reason. (This note only applies to the build type configuration parameter).

The name of the pkg-config file for CXX Driver libraries is the same as that for the regular (non-MSVC) shared library behavior described above.
However, when `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=ON`, the library output name is used as-if it were the basename for pkg-config files.
For example, to obtain flags for the shared library `bsoncxx-v_noabi-rhs-x64-v142-md.dll`, the pkg-config command may look as follows when `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=OFF` (the default):

```bash
pkg-config --cflags "libbsoncxx"
```

or as follows when `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=ON`:

```bash
pkg-config --cflags "libbsoncxx-v_noabi-rhs-x64-v142-md"
```

Setting `ENABLE_ABI_TAG_IN_PKGCONFIG_FILENAMES=ON` is recommended when parallel installation of CXX Driver libraries with different build configurations (e.g. Debug vs. Release) is expected.
However, using CMake package config files instead is most recommended.

### Static Libraries

Static libraries use a simple filename pattern:

```
lib<basename>-static.a
```

No API or ABI version information is included in the name of static library files.

Shared vs. static library selection for CMake package config files is handled via CMake targets.

Shared vs. static library selection for pkg-config files is handled by adding the `-static` suffix to the library basename, e.g. `lib<basename>-static.pc`.

For example, the following library directory contains the expected shared _and_ static library files for a bsoncxx library:

```
${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/
├── libbsoncxx.so.1.2.3
├── libbsoncxx.so.10 -> libbsoncxx.so.1.2.3
├── libbsoncxx.so -> libbsoncxx.so.10
├── libbsoncxx-static.a
├── cmake/bsoncxx-1.2.3/
│   ├── bsoncxx-config.cmake
│   └── ...
└── pkgconfig/
    ├── libbsoncxx.pc
    └── libbsoncxx-static.pc
```

### Static Libraries (MSVC Only)

The name of static libraries use the same pattern as regular (non-MSVC) static library behavior described above, but uses `static` as `<abi-version-number>` and the library output name as-if it were the basename.

For example, the following install prefix directory contains the expected shared _and_ static library files for a bsoncxx library (the bin directory is included to account for `.dll` files):

```
${CMAKE_INSTALL_PREFIX}/
├── ${CMAKE_INSTALL_BINDIR}/
│   └── bsoncxx-v10-rhs-x64-v142-md.dll
└── ${CMAKE_INSTALL_LIBDIR}/
    ├── bsoncxx-v10-rhs-x64-v142-md.lib
    ├── bsoncxx-static-dhs-x64-v142-mdd.lib
    ├── cmake/bsoncxx-1.2.3/
    │   ├── bsoncxx-config.cmake
    │   └── ...
    └── pkgconfig/
        ├── libbsoncxx-v10-rhs-x64-v142-md.pc
        └── libbsoncxx-static-rhs-x64-v142-md.pc
```
