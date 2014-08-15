# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=legacy)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

Please see our [wiki page](https://github.com/mongodb/mongo-cxx-driver/wiki/Download%20and%20Compile) for information about building, testing, and using the driver.

  Version [1.0.0-rc0](https://github.com/mongodb/mongo-cxx-driver/releases/tag/legacy-1.0.0-rc0)
  of the C++ legacy driver has been released. Help us test it out!
  Please report any bugs or issues in the C++
  [JIRA project](http://jira.mongodb.org/browse/CXX).

> **NOTE:** The legacy branch is currently in testing. If you are looking for a production-ready release of the C++ driver you should use the 26compat branch until our RC testing is complete.

Use the legacy branch if:

 - You want to help test the upcoming 1.0 release of the legacy C++ driver.
 - You are using MongoDB's C++ driver for the first time and do not mind the driver being actively worked on.
 - You had been using 26compat (or the driver inside of the server source) and want to benefit from incremental improvements while having the same overall API.

Switch to the 26compat branch if:

 - You need a production ready release of the C++ driver that is not under active development.
 - You have existing code that used the driver from the server source and want it to continue working without modification.

> **Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the driver from the server sources: this repository is the only approved source for driver builds.

## Repository Overview

| Branch   | Stability   | Development       | Purpose                                               |
| -------- | ------------| ----------------- | ----------------------------------------------------- |
| master   | Unstable    | Planning          | New C++ driver (work in progress)                     |
| legacy   | Pre-release | RC Testing        | Existing C++ driver with non-compatible improvements  |
| 26compat | Stable      | Maintenance Only  | Drop in replacement for users of existing C++ driver  |

> **WARNING:** The master branch will contain future development work on a new driver with a completely new API and implementation. It is a work in progress and currently should not be used.

## Components

  - `libmongoclient.[so|dylib|dll]` - The shared mongoclient library (but see notes)
  - `libmongoclient.a` - The static mongoclient library

## Building and Usage

 - [Download and Compile](https://github.com/mongodb/mongo-cxx-driver/wiki/Download%20and%20Compile)
 - [Tutorial](https://github.com/mongodb/mongo-cxx-driver/wiki/Tutorial)

## Bugs and Issues

  See http://jira.mongodb.org/browse/CXX

## Notes

  Use of the shared library is experimental on windows and is currently
  discouraged. This is primarily due to the complexity of ensuring a matching
  implementation of STL types between library and consumer code. This problem
  is unique to windows, as the consistent use of system libraries largely
  mitigates this danger.

## Documentation

  http://docs.mongodb.org/ecosystem/drivers/cpp/

## Mailing Lists and IRC

  http://dochub.mongodb.org/core/community

## License

  The source files in this repository are made available under the terms of the
  Apache License, version 2.0.
