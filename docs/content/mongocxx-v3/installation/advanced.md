+++
date = "2020-09-26T13:14:52-04:00"
title = "Advanced Configuration and Installation Options"
[menu.main]
  identifier = "mongocxx3-installation-advanced"
  parent = "mongocxx3-installation"
  weight = 34
+++

## Additional Options for Integrators

In the event that you are building the BSON C++ library and/or the C++ driver to embed with other components and you wish to avoid the potential for collision with components installed from a standard build or from a distribution package manager, you can make use of the `BSONCXX_OUTPUT_BASENAME` and `MONGOCXX_OUTPUT_BASENAME` options to `cmake`.

```sh
cmake ..                                            \
    -DBSONCXX_OUTPUT_BASENAME=custom_bsoncxx        \
    -DMONGOCXX_OUTPUT_BASENAME=custom_mongocxx
```

The above command would produce libraries named `libcustom_bsoncxx.so` and `libcustom_mongocxx.so` (or with the extension appropriate for the build platform).  Those libraries could be placed in a standard system directory or in an alternate location and could be linked to by specifying something like `-lcustom_mongocxx -lcustom_bsoncxx` on the linker command line (possibly adjusting the specific flags to those required by your linker).

## Advanced configuration (static configurations)

The following sub-sections detail advanced options for configuring the C++ driver and/or its
dependencies as static libraries rather than the typical shared libraries.  These options will
produce library artifacts that will behave differently.  Ensure you have a complete understanding
of the implications of the various linking approaches before utilizing these options.

### For Linux and macOS

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

### For Windows

#### Configuring with `mongocxx` 3.2.x or newer

Users have the option to build `mongocxx` as a static library. **This is not recommended for novice
users.** A user can enable this behavior with the `-DBUILD_SHARED_LIBS` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_LIBS=OFF                         \
    -DCMAKE_PREFIX_PATH=C:\mongo-c-driver           \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

#### Configuring with `mongocxx` 3.5.0 or newer

Users have the option to build `mongocxx` as both static and shared libraries. A user can enable
this behavior with the `-DBUILD_SHARED_AND_STATIC_LIBS` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_AND_STATIC_LIBS=ON               \
    -DCMAKE_PREFIX_PATH=C:\mongo-c-driver           \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

Users have the option to build `mongocxx` as a shared library that has statically linked
`libmongoc`. **This is not recommended for novice users.** A user can enable this behavior with the
`-DBUILD_SHARED_LIBS_WITH_STATIC_MONGOC` option:

```sh
cmake ..                                            \
    -DCMAKE_BUILD_TYPE=Release                      \
    -DBUILD_SHARED_LIBS_WITH_STATIC_MONGOC=ON       \
    -DCMAKE_PREFIX_PATH=C:\mongo-c-driver           \
    -DCMAKE_INSTALL_PREFIX=C:\mongo-cxx-driver
```

## Disabling tests

Pass `-DENABLE_TESTS=OFF` as a cmake option to disable configuration of test targets.

```sh
cmake .. -DENABLE_TESTS=OFF
cmake --build .. --target help
# No test targets are configured.
```