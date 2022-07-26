+++
date = "2020-09-26T13:47:23-04:00"
title = "Windows"
[menu.main]
  identifier = "mongocxx3-installation-windows"
  parent = "mongocxx3-installation"
  weight = 31
+++

### Step 1: Install the latest version of the MongoDB C driver.

The mongocxx driver builds on top of the MongoDB C driver.

* For mongocxx-3.6.x, libmongoc 1.17.0 or later is required.
* For mongocxx-3.5.x, libmongoc 1.15.0 or later is required.
* For mongocxx-3.4.x, libmongoc 1.13.0 or later is required.
* For mongocxx-3.3.x, libmongoc 1.10.1 or later is required.
* For mongocxx-3.2.x, libmongoc 1.9.2 or later is required.
* For mongocxx-3.1.4+, libmongoc 1.7.0 or later is required.
* For mongocxx-3.1.[0-3], libmongoc 1.5.0 or later is required.
* For mongocxx-3.0.x, we recommend the last 1.4.x version of libmongoc

Unless you know that your package manager offers a high enough version, you
will need to download and build from the source code. Get a tarball from
the [C Driver releases](https://github.com/mongodb/mongo-c-driver/releases)
page.

Follow the instructions for building from a tarball at
[Installing libmongoc](http://mongoc.org/libmongoc/current/installing.html).

Industry best practices and some regulations require the use of TLS 1.1
or newer. The MongoDB C Driver supports TLS 1.1 on Linux if OpenSSL is
at least version 1.0.1. On macOS and Windows, the C Driver uses native
TLS implementations that support TLS 1.1.

If you are compiling on Windows, you should disable extra alignment with
`-DENABLE_EXTRA_ALIGNMENT=0` when configuring the C driver.

### Step 2: Choose a C++17 polyfill

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

   `std::experimental`
     Select with `-DBSONCXX_POLY_USE_STD_EXPERIMENTAL=1`. If your
     toolchain's standard library provides `optional` and
     `string_view` in the namespace `std::experimental`, you can use
     this option. Be aware that your standard library's
     `std::experimental` implementation may change over time,
     breaking binary compatibility in unexpected ways. Note that this
     polyfill is *not* recommended and is unsupported.

Most users should be fine sticking with the default. However, if you
have an existing application which makes heavy use of one of the
available libraries, you may prefer to build the mongocxx driver
against the same library.

**DO NOT** change your project's polyfill if you need to create a
stable binary interface.

### Step 3: Download the latest version of the mongocxx driver.

The most reliable starting point for building the mongocxx driver is the latest
release tarball.

The [mongocxx releases](https://github.com/mongodb/mongo-cxx-driver/releases)
page will have links to the release tarball for the version you wish you install.  For
example, to download version 3.6.7:

```sh
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.7/mongo-cxx-driver-r3.6.7.tar.gz
tar -xzf mongo-cxx-driver-r3.6.7.tar.gz
cd mongo-cxx-driver-r3.6.7/build
```

Make sure you change to the `build` directory of whatever source tree you
obtain.

### Step 4: Configure the driver

On Windows, the C++ driver is configured as follows (adjusting the path of the CMake executable as appropriate to your system):

```sh
'C:\Program Files (x86)\CMake\bin\cmake.exe' .. \
     -G "Visual Studio 14 2015 Win64"           \
    -DBOOST_ROOT=C:\local\boost_1_60_0          \
    -DCMAKE_PREFIX_PATH=C:\mongo-c-driver       \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

The example above assumes:

* Boost is found in `C:\local\boost_1_60_0`.
* `libmongoc` is found in `C:\mongo-c-driver`.
* `mongocxx` is to be installed into `C:\mongo-cxx-driver`.

For building with Visual Studio 2017 (without a C++17 polyfill), it is necessary to configure with
an additional option, `/Zc:__cplusplus` to opt into the correct definition of `__cplusplus`
([problem described here](https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/)):

```sh
'C:\Program Files (x86)\CMake\bin\cmake.exe' .. \
    -G "Visual Studio 15 2017 Win64"            \
    -DCMAKE_CXX_STANDARD=17                     \
    -DCMAKE_CXX_FLAGS="/Zc:__cplusplus"         \
    -DCMAKE_PREFIX_PATH=C:\mongo-c-driver       \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver  \
```

For details on how to install libmongoc for Windows, see the
[mongoc Windows installation instructions](http://mongoc.org/libmongoc/current/installing.html#building-windows).

#### Configuring with `mongocxx` 3.1.x or 3.0.x

Instead of the `-DCMAKE_PREFIX_PATH` option, users must specify the `libmongoc` installation
directory by using the `-DLIBMONGOC_DIR` and `-DLIBBSON_DIR` options:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DLIBMONGOC_DIR=C:\mongo-c-driver               \
    -DLIBBSON_DIR=C:\mongo-c-driver                 \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

### Step 5: Build and install the driver

Build and install the driver:

```sh
cmake --build .
cmake --build . --target install
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

