+++
date = "2016-08-15T16:11:58+05:30"
title = "Installing the mongocxx driver"
[menu.main]
  parent = "mongocxx3"
  weight = 8
+++

## Installing the mongocxx driver

### Prerequisites

- Any standard Unix platform, or Windows 7 SP1+
- GCC 4.8.2+, Clang 3.5+, Apple Clang 5.1+, or Visual Studio C++ 2015
  Update 1 or later
- CMake 3.2 or later
- On Windows, boost 1.56 or later

### Installation

#### Step 1: Install the latest version of the MongoDB C driver.

The mongocxx driver builds on top of the MongoDB C driver.

Follow the instructions at
[Installing libmongoc](http://mongoc.org/libmongoc/current/installing.html).

#### Step 2: Choose a C++17 polyfill

The mongocxx driver uses the experimental C++17 features
`std::optional` and `std::string_view`. To compile
the mongocxx driver, you must choose one of the following
implementations for these features:

   MNMLSTC/core (*default for non-Windows platforms*)
     Select with `-DBSONCXX_POLY_USE_MNMLSTC=1`. This option vendors a
     header-only installation of MNMLSTC/core into the bsoncxx library
     installation.

   Boost (*default for Windows platforms*)
     Select with `-DBSONCXX_POLY_USE_BOOST=1`. This is currently the
     only option if you are using MSVC.

   `std::experimental`
     Select with `-DBSONCXX_POLY_USE_STD_EXPERIMENTAL=1`. If your
     toolchain's standard library provides `optional` and
     `string_view` in the namespace `std::experimental`, you can use
     this option.

Most users should be fine sticking with the default. However, if you
have an existing application which makes heavy use of one of the
available libraries, you may prefer to build the mongocxx driver
against the same library.

**DO NOT** change your project's polyfill if you need to create a
stable binary interface.

Be aware that your standard library's `std::experimental` implementation
may change over time, breaking binary compatibility in unexpected ways.

#### Step 3: Download and install the latest version of the mongocxx driver.

```sh
curl -OL https://github.com/mongodb/mongo-cxx-driver/archive/r3.0.1.tar.gz
tar -xzf r3.0.1.tar.gz
cd mongo-cxx-driver-r3.0.1/build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make && sudo make install
```

If you did not install the MongoDB C driver into the default path,
substitute the `cmake` line above with the following:

```sh
PKG_CONFIG_PATH=CDRIVER_INSTALL_PATH/lib/pkgconfig \
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=CXXDRIVER-INSTALL-PATH ..
```

To select a polyfill, pass the option to `cmake`. For example,
to select the Boost polyfill, substitute the `cmake` line with
the following:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_BOOST=1 ..
```

#### Step 4: Test your installation

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

Compile the test program with the following command:

```sh
c++ --std=c++11 test.cpp -o test $(pkg-config --cflags --libs libmongocxx)
```
