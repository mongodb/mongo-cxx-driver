+++
date = "2020-09-26T13:47:23-04:00"
title = "Windows"
[menu.main]
  identifier = "mongocxx3-installation-windows"
  parent = "mongocxx3-installation"
  weight = 31
+++

### Step 1: Choose a C++17 polyfill

The mongocxx driver uses the C++17 features `std::optional` and
`std::string_view`. To compile the mongocxx driver for pre-C++17, you
must choose one of the following implementations for these features:

   MNMLSTC/core (*default for non-Windows platforms*)
     Select with `-DBSONCXX_POLY_USE_MNMLSTC=1`.  **NOTE**: This option
     vendors a header-only installation of MNMLSTC/core into the bsoncxx
     library installation and will therefore download MLNMLSTC from GitHub
     during the build process. If you already have an available version of
     MNMLSTC on your system, you can avoid the download step by using
     `-DBSONCXX_POLY_USE_SYSTEM_MNMLSTC`.

   Boost (*default for Windows platforms*)
     Select with `-DBSONCXX_POLY_USE_BOOST=1`. This is currently the
     only option if you are using a version of MSVC that does not support
     C++17.

Most users should be fine sticking with the default. However, if you
have an existing application which makes heavy use of one of the
available libraries, you may prefer to build the mongocxx driver
against the same library.

**DO NOT** change your project's polyfill if you need to create a
stable binary interface.

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

On Windows, the C++ driver is configured as follows (adjusting the path of the CMake executable as appropriate to your system):

```sh
'C:\Program Files (x86)\CMake\bin\cmake.exe' .. \
     -G "Visual Studio 14 2015" -A "x64"        \
    -DBOOST_ROOT=C:\local\boost_1_60_0          \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

The example above assumes:

* Boost is found in `C:\local\boost_1_60_0`.
* `mongocxx` is to be installed into `C:\mongo-cxx-driver`.

To build with Visual Studio 2017 without a C++17 polyfill, configure as follows:

```sh
'C:\Program Files (x86)\CMake\bin\cmake.exe' .. \
    -G "Visual Studio 15 2017" -A "x64"         \
    -DCMAKE_CXX_STANDARD=17                     \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver  \
```

#### Configuring with `mongocxx` 3.7.0 and older

To build versions 3.7.0 and older without a C++17 polyfill, it is necessary to configure with additional options:
- `/Zc:__cplusplus` to opt into the correct definition of `__cplusplus` ([problem described here](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/))
- `/EHsc` to enable recommended [exception handling behavior](https://learn.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-170).

```sh
'C:\Program Files (x86)\CMake\bin\cmake.exe' .. \
    -G "Visual Studio 15 2017" -A "x64"         \
    -DCMAKE_CXX_STANDARD=17                     \
    -DCMAKE_CXX_FLAGS="/Zc:__cplusplus /EHsc"   \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver  \
```

#### Configuring with `mongocxx` 3.1.x or 3.0.x

```sh
cmake ..                                            \
    -DLIBMONGOC_DIR=C:\mongo-c-driver               \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

### Step 4: Build and install the driver

Build and install the driver. Use `--config` to select a build configuration (e.g. `Debug`, `RelWithDebInfo`, `Release`):

```sh
cmake --build . --config RelWithDebInfo
cmake --build . --target install --config RelWithDebInfo
```

The driver can be uninstalled at a later time in one of two ways.  First,
the uninstall target can be called:

```sh
cmake --build . --target uninstall
```

Second, the uninstall script can be called:

```sh
C:\opt\mongo-cxx-driver\share\mongo-cxx-driver\uninstall.cmd
```
