+++
date = "2020-09-26T13:14:52-04:00"
title = "Installing the mongocxx driver"
[menu.main]
  identifier = "mongocxx3-installation"
  parent = "mongocxx3"
  weight = 8
+++

## Prerequisites

- Any standard Unix platform, or Windows 7 SP1+
- A compiler that supports C++11 (gcc, clang, or Visual Studio)
- [CMake](https://cmake.org) 3.2 or later
- [boost](https://www.boost.org) headers (optional)

If you encounter build failures or other problems with a platform configuration
that meets the above prerequisites, please file a bug report via
[JIRA](https://jira.mongodb.com/browse/CXX/).

## Installation

To configure and install the driver, follow the instructions for your platform:

* [Configuring and installing on Windows]({{< ref "/mongocxx-v3/installation/windows" >}})
* [Configuring and installing on macOS]({{< ref "/mongocxx-v3/installation/macos" >}})
* [Configuring and installing on Linux]({{< ref "/mongocxx-v3/installation/linux" >}})

## Advanced Options

* [Advanced Configuration and Installation Options]({{< ref "/mongocxx-v3/installation/advanced" >}})

## Package Managers

The Mongo C++ driver is available in the following package managers.
- [Vcpkg](https://vcpkg.io/) (search for mongo-cxx-driver)
- [Conan](https://conan.io/center/recipes/mongo-cxx-driver)

### Vcpkg Install Instructions

If you do not already have Vcpkg installed, install it with the following
command:
```
$ git clone https://github.com/Microsoft/vcpkg.git
$ cd vcpkg
$ ./bootstrap-vcpkg.sh
```

Install the driver.
```
$ ./vcpkg install mongo-cxx-driver
```

Optionally, to install with Visual Studio integration:
```
vcpkg integrate install
```

You can find the header files in:
```
vcpkg/installed/<CPU ARCHITECTURE>-<OPERATING SYSTEM>/include/
```

The library files are in:
```
vcpkg/installed/<CPU ARCHITECTURE>-<OPERATING SYSTEM>/lib/
```

### Conan Install Instructions

If you do not already have Conan installed, then install it and run the Conan
initalization command below:
```
$ pip install conan
$ conan profile detect --force
```

Add the following to your `conanfile.txt`
```
[requires]
mongo-cxx-driver/3.8.0
[generators]
CMakeDeps
CMakeToolchain
```

Install the driver via Conan, and build your project:
```
$ conan install conanfile.txt --output-folder=build --build=missing
$ cmake \
	-B build \
	-DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
	-DCMAKE_BUILD_TYPE=Release
$ cmake --build build
```

## Docker Image

You can find a pre-built docker image for the C++ driver in
[Docker Hub](https://hub.docker.com/r/mongodb/mongo-cxx-driver).
