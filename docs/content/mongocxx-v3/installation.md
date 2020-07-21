+++
date = "2016-08-15T16:11:58+05:30"
title = "Installing the mongocxx driver"
[menu.main]
  parent = "mongocxx3"
  weight = 8
+++

## Prerequisites

- Any standard Unix platform, or Windows 7 SP1+
- A compiler that supports C++11 (gcc, clang, or Visual Studio)
- [CMake](https://cmake.org) 3.2 or later
- [boost](https://www.boost.org) headers (optional)

We currently test the driver on the following platforms:

- Linux with clang 3.8, GCC 5.4 and 7.5
- macOS with Apple clang 7.0 and 11.0 using Boost 1.70.0
- Windows with Visual Studio 2015 using Boost 1.60.0 and Visual Studio 2017

Versions older than the ones listed may not work and are not
supported; use them at your own risk.

Versions newer than the ones listed above should work; if you
have problems, please file a bug report via
[JIRA](https://jira.mongodb.org/browse/CXX/).

## Installation

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
example, to download version 3.5.1:

```sh
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.5.1/mongo-cxx-driver-r3.5.1.tar.gz
tar -xzf mongo-cxx-driver-r3.5.1.tar.gz
cd mongo-cxx-driver-r3.5.1/build
```

Make sure you change to the `build` directory of whatever source tree you
obtain.

### Step 4: Configure the driver

On Unix systems, `libmongoc` installs into `/usr/local` by default. Without additional
configuration, `mongocxx` installs into its local build directory as a courtesy to those who build
from source. To configure `mongocxx` for installation into `/usr/local` as well, use the following
`cmake` command:

```
cmake ..                                \
    -DCMAKE_BUILD_TYPE=Release          \
    -DCMAKE_INSTALL_PREFIX=/usr/local
```

In the Unix examples that follow,
`mongocxx` is customized in these ways:
* `libmongoc` is found in `/opt/mongo-c-driver`.
* `mongocxx` is to be installed into `/opt/mongo-cxx-driver`.

With those two distinct (arbitrary) install locations, a user would run this `cmake` command:
```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DCMAKE_PREFIX_PATH=/opt/mongo-c-driver         \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```

> *Note* If you need multiple paths in a CMake PATH variable, separate them with a semicolon like
> this:
> `-DCMAKE_PREFIX_PATH="/your/cdriver/prefix;/some/other/path"`

These options can be freely mixed with a C++17 polyfill option. For instance, this is how a user
would run the command above with the Boost polyfill option:
```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBSONCXX_POLY_USE_BOOST=1                      \
    -DCMAKE_PREFIX_PATH=/opt/mongo-c-driver         \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```
---

On Windows, this is the equivalent use of cmake:

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
    -DLIBMONGOC_DIR=/opt/mongo-c-driver             \
    -DLIBBSON_DIR=/opt/mongo-c-driver               \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```

#### Configuring with `mongocxx` 3.2.x or newer

Users have the option to build `mongocxx` as a static library. **This is not recommended for novice
users.** A user can enable this behavior with the `-DBUILD_SHARED_LIBS` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_LIBS=OFF                         \
    -DCMAKE_PREFIX_PATH=/opt/mongo-c-driver         \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```

#### Configuring with `mongocxx` 3.5.0 or newer

Users have the option to build `mongocxx` as both static and shared libraries. A user can enable
this behavior with the `-DBUILD_SHARED_AND_STATIC_LIBS` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_AND_STATIC_LIBS=ON               \
    -DCMAKE_PREFIX_PATH=/opt/mongo-c-driver         \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```

Users have the option to build `mongocxx` as a shared library that has statically linked
`libmongoc`. **This is not recommended for novice users.** A user can enable this behavior with the
`-DBUILD_SHARED_LIBS_WITH_STATIC_MONGOC` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_LIBS_WITH_STATIC_MONGOC=ON       \
    -DCMAKE_PREFIX_PATH=/opt/mongo-c-driver         \
    -DCMAKE_INSTALL_PREFIX=/opt/mongo-cxx-driver
```

### Step 5: Build and install the driver

If you are using the default MNMLSTC polyfill and are installing to a
directory requiring root permissions, you should install the polyfill with
`sudo` before building the rest of mongocxx so you don't have to run
the entire build with `sudo`:

```sh
# Only for MNMLSTC polyfill
sudo cmake --build . --target EP_mnmlstc_core
```

Once MNMLSTC is installed, or if you are using a different polyfill,
build and install the driver:

```sh
cmake --build .
sudo cmake --build . --target install
```

The driver can be uninstalled at a later time in one of two ways.  First,
the uninstall target can be called:

```sh
sudo cmake --build . --target uninstall
```

Second, the uninstall script can be called (on Linux or macOS):

```sh
sudo /opt/mongo-cxx-driver/share/mongo-cxx-driver/uninstall.sh
```

Or (on Windows):

```sh
C:\opt\mongo-cxx-driver\share\mongo-cxx-driver\uninstall.cmd
```

#### Additional Options for Integrators

In the event that you are building the BSON C++ library and/or the C++ driver to embed with other components and you wish to avoid the potential for collision with components installed from a standard build or from a distribution package manager, you can make use of the `BSONCXX_OUTPUT_BASENAME` and `MONGOCXX_OUTPUT_BASENAME` options to `cmake`.

```sh
cmake ..                                            \
    -DBSONCXX_OUTPUT_BASENAME=custom_bsoncxx        \
    -DMONGOCXX_OUTPUT_BASENAME=custom_mongocxx
```

The above command would produce libraries named `libcustom_bsoncxx.so` and `libcustom_mongocxx.so` (or with the extension appropriate for the build platform).  Those libraries could be placed in a standard system directory or in an alternate location and could be linked to by specifying something like `-lcustom_mongocxx -lcustom_bsoncxx` on the linker command line (possibly adjusting the specific flags to those required by your linker).

### Step 6: Test your installation

Save the following source file with the filename `test.cpp`
underneath any directory:

```c++
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    bsoncxx::builder::stream::document document{};

    auto collection = conn["testdb"]["testcollection"];
    document << "hello" << "world";

    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}
```

#### Compiling with the help of CMake

If you are using CMake for your project, you can use the `find_package()`
directive to import targets which can be used to link to your build
targets.  If you have installed `mongocxx` or `libmongoc` to a
 non-standard location on your system, you will need to set
`CMAKE_PREFIX_PATH` to the library installation prefix (specified at
build time with `CMAKE_INSTALL_PREFIX`) when running `cmake`.

In the `mongocxx` source repository (versions 3.2.x or newer only), see
the directory `examples/projects/mongocxx/cmake` for an example CMake
application which uses the shared library (the default option), and an
example CMake application which uses the static library (advanced users
only).

#### Compiling with the help of pkg-config

Compile the test program above with the following command:

```sh
c++ --std=c++11 test.cpp -o test $(pkg-config --cflags --libs libmongocxx)
```

Advanced users who are using the static library must replace the
`libmongocxx` argument to `pkg-config` above with `libmongocxx-static`
(this requires mongocxx 3.2.x or newer).

If you installed to somewhere not in your pkg-config search path, remember
to set the `PKG_CONFIG_PATH` environment variable first:

```
export PKG_CONFIG_PATH="$MY_INSTALL_PREFIX/lib/pkgconfig"
```

#### Compiling without pkg-config or CMake

If you aren't using CMake for your project and you don't have pkg-config
available, you will need to set include and library flags manually on the
command line or in your IDE.

Here's an example expansion of the compilation line above, on a system
where mongocxx and libmongoc are installed in `/usr/local`:

```sh
c++ --std=c++11 test.cpp -o test \
    -I/usr/local/include/mongocxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi \
    -L/usr/local/lib -lmongocxx -lbsoncxx
```

Advanced users only: here is an example expansion on the same system of
the compilation line above when static libraries are being used.  Note
that the preprocessor defines `MONGOCXX_STATIC` and `BSONCXX_STATIC` must
be defined in all source files that include mongocxx headers; failure to
do so will result in difficult-to-diagnose linker errors.

```sh
c++ --std=c++11 test.cpp -o test \
    -DMONGOCXX_STATIC -DBSONCXX_STATIC -DMONGOC_STATIC -DBSON_STATIC \
    -I/usr/local/include/libmongoc-1.0 \
    -I/usr/local/include/libbson-1.0 \
    -I/usr/local/include/mongocxx/v_noabi \
    -I/usr/local/include/bsoncxx/v_noabi \
    -L/usr/local/lib -lmongocxx-static -lbsoncxx-static
    -lmongoc-static-1.0 -lsasl2 -lssl -lcrypto -lbson-static-1.0 -lm -lpthread
```

#### Compiling with MSVC

To compile on MSVC, you will need to setup your project to include all the
necessary include paths, library paths, preprocessor defines, and link
libraries. To do this, you can set these values either by the UI or by
editing the XML `.vcxproj` file directly. To confirm you have everything
setup correctly, here are the `PropertyGroup` and `ItemDefinitionGroup`
settings for a Debug x64 build as an example:

```xml
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <LinkIncremental>true</LinkIncremental>
    <IncludePath>c:\local\boost_1_60_0\;C:\mongo-cxx-driver\include\mongocxx\v_noabi;C:\mongo-cxx-driver\include\bsoncxx\v_noabi;C:\mongo-c-driver\include\libmongoc-1.0;C:\mongo-c-driver\include\libbson-1.0;$(IncludePath)</IncludePath>
    <LibraryPath>c:\mongo-c-driver\lib\;c:\mongo-cxx-driver\lib\;$(LibraryPath)</LibraryPath>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <PrecompiledHeader>Use</PrecompiledHeader>
      <WarningLevel>Level3</WarningLevel>
      <Optimization>Disabled</Optimization>
      <PreprocessorDefinitions>MONGOCXX_STATIC;BSONCXX_STATIC;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <SDLCheck>true</SDLCheck>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>libmongocxx.lib;libbsoncxx.lib;mongoc-static-1.0.lib;bson-1.0.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
```
