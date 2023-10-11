+++
date = "2020-09-26T13:47:02-04:00"
title = "Linux"
[menu.main]
  identifier = "mongocxx3-installation-linux"
  parent = "mongocxx3-installation"
  weight = 33
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

### Step 2: Download the latest version of the mongocxx driver.

The most reliable starting point for building the mongocxx driver is the latest
release tarball.

The [mongocxx releases](https://github.com/mongodb/mongo-cxx-driver/releases)
page will have links to the release tarball for the version you wish you install.  For
example, to download version 3.8.1:

```sh
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.8.1/mongo-cxx-driver-r3.8.1.tar.gz
tar -xzf mongo-cxx-driver-r3.8.1.tar.gz
cd mongo-cxx-driver-r3.8.1/build
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
