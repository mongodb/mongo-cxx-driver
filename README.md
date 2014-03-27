# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=26compat)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

Use this branch if:

 - You have existing code that used the driver from the server source
 and want it to continue working without modification.

Switch to the legacy branch if:

 - You are using MongoDB's C++ driver for the first time.
 - You had been using 26compat (or the driver inside of the server source) and
 want to benefit from incremental improvements but have the same overall API.

**Please Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the
driver from the server sources: this repository is the only approved source for
driver builds.

The master branch will contain future development work on a new driver with a
completely new API and implementation. It is a work in progress and currently
should not be used.

## Repository Overview

| Branch   | Development        | Purpose                                               |
| -------- | ------------------ | ----------------------------------------------------- |
| master   | Planning           | New C++ driver (work in progress)                     |
| legacy   | Active             | Existing C++ driver with non-compatible improvements  |
| 26compat | Maintenance Only   | Drop in replacement for users of existing C++ driver  |

## Components

  - `libmongoclient.[so|dylib|dll]` - The shared mongoclient library (but see notes)
  - `libmongoclient.a` - The static mongoclient library

## Building

  See http://docs.mongodb.org/ecosystem/tutorial/download-and-compile-cpp-driver/

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
