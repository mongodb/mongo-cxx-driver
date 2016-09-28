# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=26compat)](https://travis-ci.org/mongodb/mongo-cxx-driver)

Welcome to the MongoDB C++ Driver!

This branch contains the `26compat` C++ driver.  Use this branch only if
you have existing code that used the 26compat driver and want it to
continue working without modification.

**NOTE: Continued use of 26compat driver is discouraged.**

Switch to the "legacy" branch if you had previously been using 26compat (or
the driver inside of the server source) and want to benefit from
incremental improvements while having the same overall API.

Switch to the "master" branch if you are using MongoDB's C++ driver for the
first time.

## Driver status by family and version

Stability indicates whether this driver is recommended for production use.
Currently, no drivers guarantee API or ABI stability.

| Family/version | Stability   | Development         | Purpose                                                      |
| ---------------| ------------| ------------------- | ------------------------------------------------------------ |
| mongocxx 3.1.x | Unstable    | Active development  | Unstable C++ driver development, requires C++11              |
| mongocxx 3.0.x | Stable      | Bug fixes only      | Primary stable C++ driver release, requires C++11            |
| legacy   (all) | Stable      | Critical fixes only | Former stable C++ driver release                             |
| 26compat (all) | Deprecated  | Critical fixes only | Drop in replacement for users of existing 2.6 era C++ driver |

## MongoDB compatibility

The following compatibility table specifies the driver version(s)
recommended for different versions of MongoDB.  The 3.0.x series
is recommended for all new development.

| Family/version | MongoDB 2.4 | MongoDB 2.6 | MongoDB 3.0 | MongoDB 3.2 |
| ---------------| ------------| ------------| ------------| ------------|
| mongocxx 3.0.x | ✓           | ✓           | ✓           | ✓           |
| legacy   1.1.x | ✓           | ✓           | ✓           | ✓           |
| legacy   1.0.x | ✓           | ✓           | ✓           |             |
| 26compat (all) |             | ✓           |             |             |

## Components

  - `libmongoclient.[so|dylib|dll]` - The shared mongoclient library (but see notes)
  - `libmongoclient.a` - The static mongoclient library

## Notes

  Use of the shared library is experimental on windows and is currently
  discouraged. This is primarily due to the complexity of ensuring a matching
  implementation of STL types between library and consumer code. This problem
  is unique to windows, as the consistent use of system libraries largely
  mitigates this danger.

## Bugs and issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## License

The source files in this repository are made available under the terms of
the Apache License, version 2.0.
