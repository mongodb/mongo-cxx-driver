+++
date = "2020-09-26T13:47:18-04:00"
title = "macOS"
[menu.main]
  identifier = "mongocxx3-installation-macos"
  parent = "mongocxx3-installation"
  weight = 32
+++

### Step 1: Choose a C++17 polyfill

The mongocxx driver uses C++17 features `std::optional` and `std::string_view`.
To use the C++17 standard library implementations for these features, set
the CMake configuration variable `CMAKE_CXX_STANDARD` to 17 or higher.
Otherwise, to compile the mongocxx driver for pre-C++17 configurations, a
polyfill library implementation must be selected from the following options
(note: "default" refers to **pre-C++17** configurations when no polyfill library
is explicitly selected):

* bsoncxx (*default only when `-DENABLE_BSONCXX_POLY_USE_IMPLS=ON`*)

  Select with `-DBSONCXX_POLY_USE_IMPLS=ON`. This option is most recommended, as
  it does not require additional external library dependencies. To enable
  selecting this option by default for pre-C++17 configurations when no other
  options are specified, set `ENABLE_BSONCXX_POLY_USE_IMPLS=ON` (this option
  will be set to ON by default in an upcoming major release).

* MNMLSTC/core (*default for non-Windows platforms*)

  **This option is deprecated and will be removed in an upcoming major release.**
  Select with `-DBSONCXX_POLY_USE_MNMLSTC=1`. **NOTE**: This option vendors a
  header-only installation of MNMLSTC/core into the bsoncxx library installation
  and will therefore download MLNMLSTC from GitHub during the configuration
  process. If you already have an available version of MNMLSTC on your system,
  you can avoid the download step by using `-DBSONCXX_POLY_USE_SYSTEM_MNMLSTC`.

* Boost (*default for Windows platforms*)

  **This option is deprecated and will be removed in an upcoming major release.**
  Select with `-DBSONCXX_POLY_USE_BOOST=1`. This is currently the only
  non-bsoncxx option if you are using a version of MSVC that does not support
  C++17.

* `std::experimental`

  **This option is deprecated and will be removed in an upcoming major release.**
  Select with `-DBSONCXX_POLY_USE_STD_EXPERIMENTAL=1`. If your toolchain's
  standard library provides `optional` and `string_view` in the namespace
  `std::experimental`, you can use this option. Be aware that your standard
  library's `std::experimental` implementation may change over time, breaking
  binary compatibility in unexpected ways. Note that this polyfill is *not*
  recommended and is unsupported.

Most users should use the default polyfill selection behavior. However, if you
have a preference for one of the external polyfill libraries (e.g. already a
dependency being used by your application), you may prefer to explicitly select
that external polyfill library rather than rely on default selection behavior.

**NOTE**: C++ standard conformance and supported behavior of polyfill features
may vary depending on the selected polyfill library. The purpose of these
polyfills is to support pre-C++17 configurations by providing stand-ins for
their C++17 equivalents. Therefore we recommend using the C++17 standard
library whenever possible by setting `-DCMAKE_CXX_STANDARD=17` or newer.

**WARNING**: the choice of polyfill library has a direct impact on the public
API and ABI for the mongocxx library. Changing the polyfill can lead to both
source-breaking changes (during compilation) and binary-breaking changes (during
linking or execution). To limit reliance on polyfill-specific configuration or
behavior, avoid using `stdx::string_view` and `stdx::optional<T>` with
non-mongocxx library interfaces.

### Step 2: Download the latest version of the mongocxx driver.

The most reliable starting point for building the mongocxx driver is the latest
release tarball.

The [mongocxx releases](https://github.com/mongodb/mongo-cxx-driver/releases)
page will have links to the release tarball for the version you wish you install.  For
example, to download version 3.9.0:

```sh
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.9.0/mongo-cxx-driver-r3.9.0.tar.gz
tar -xzf mongo-cxx-driver-r3.9.0.tar.gz
cd mongo-cxx-driver-r3.9.0/build
```

Make sure you change to the `build` directory of whatever source tree you
obtain.

### Step 3: Configure the driver

Without additional
configuration, `mongocxx` installs into its local build directory as a courtesy to those who build
from source. To configure `mongocxx` for installation into `/usr/local` as well, use the following
`cmake` command:

```
cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
```

These options can be freely mixed with a C++17 polyfill option. For instance, this is how a user
would run the command above with the Boost polyfill option:
```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBSONCXX_POLY_USE_BOOST=1                      \
    -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
```

### Step 4: Build and install the driver

Build and install the driver:

```sh
cmake --build .
sudo cmake --build . --target install
```

The driver can be uninstalled at a later time in one of two ways.  First,
the uninstall target can be called:

```sh
sudo cmake --build . --target uninstall
```

Second, the uninstall script can be called:

```sh
sudo <install-dir>/share/mongo-cxx-driver/uninstall.sh
```
