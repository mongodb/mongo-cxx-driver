# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=26compat)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

Please see our [wiki page](https://github.com/mongodb/mongo-cxx-driver/wiki/Download-and-Compile-the-Legacy-Driver) for information about building, testing, and using the driver.

  Version [legacy-0.0-26compat-2.6.12](https://github.com/mongodb/mongo-cxx-driver/tree/legacy-0.0-26compat-2.6.11) of the
  C++ legacy driver has been released. Please report any bugs or issues in the C++
  [JIRA project](http://jira.mongodb.org/browse/CXX).

Use this branch only if you have existing code that used the driver
from the server source and want it to continue working without
modification. Switch to the "legacy" branch if you had previosuly been
using 26compat (or the driver inside of the server source) and want to
benefit from incremental improvements while having the same overall
API. Switch to the "master" branch if you are using MongoDB's C++
driver for the first time.

> **Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the driver from the server sources: this repository is the only approved source for driver builds.

## Repository Overview

| Branch   | Stability   | Development       | Purpose                                                      |
| -------- | ------------| ----------------- | -----------------------------------------------------        |
| master   | Stable      | Stable Evolution  | Primary stable C++ driver release, requires C++11            |
| legacy   | Stable      | Maintenance Only  | Former stable C++ driver release                             |
| 26compat | Stable      | Maintenance Only  | Drop in replacement for users of existing 2.6 era C++ driver |

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

## Licencse

  The source files in this repository are made available under the terms of the
  Apache License, version 2.0.
