# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=legacy)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

Please see our [wiki page](https://github.com/mongodb/mongo-cxx-driver/wiki/Download%20and%20Compile) for information about building, testing, and using the driver.

  Version [1.0.5](https://github.com/mongodb/mongo-cxx-driver/releases/tag/legacy-1.0.5) of the
  C++ legacy driver has been released. Please report any bugs or issues in the C++
  [JIRA project](http://jira.mongodb.org/browse/CXX).

Use the legacy branch if:

 - You are using MongoDB's C++ driver for the first time.
 - You had been using 26compat (or the driver inside of the server source) and want to benefit from incremental improvements while having the same overall API.

Use the 26compat branch only if:

 - You have existing code that used the driver from the 2.6 or earlier server source and want it to continue working without modification.

> **Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the driver from the server sources: this repository is the only approved source for C++ driver builds.

## Repository Overview

| Branch   | Stability   | Development       | Purpose                                                      |
| -------- | ------------| ----------------- | -----------------------------------------------------        |
| master   | Alpha       | In progress       | New C++11 driver                                             |
| legacy   | Stable      | Stable Evolution  | Primary stable C++ driver release                            |
| 26compat | Stable      | Maintenance Only  | Drop in replacement for users of existing 2.6 era C++ driver |

Please note that stable branches are only production quality at stable release tags. Other
commits or pre-release tags on a stable branch represent ongoing development work towards the
next stable release, and therefore may be unstable.

> **WARNING:** The master branch contains development work on a new driver with a completely
> new API and implementation. It is a work in progress and currently should not be used.

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
