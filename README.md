# MongoDB Legacy C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.png?branch=legacy)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB Legacy C++ Driver!

This driver is an extraction of the "Server C++ Driver" from the primary
MongoDB sources. You should prefer using this to building the driver
from the server sources if at all possible. Please note that as of
MongoDB 2.6.0-rc1, it is no longer possible to build the driver from
the server sources: this repo is the only approved source for driver builds.

## Components

  libmongoclient.[so|dylib|dll] - The shared mongoclient library (but see notes)
  libmongoclient.a - The static mongoclient library

## Building

  See http://docs.mongodb.org/ecosystem/tutorial/download-and-compile-cpp-driver/

  The above instructions (currently) assume that you are building from the server
  repository, but the will work with this repository as described.

## Notes

  Use of the shared library is experimental on windows and is currently
  discouraged.  This is primarily due to the complexity of ensuring a matching
  implementation of STL types between library and consumer code.  This problem
  is unique to windows, as the consistent use of system libraries largely
  mitigates this danger.

## Documentation

  http://docs.mongodb.org/ecosystem/drivers/cpp/

## Mailing Lists and IRC

  http://dochub.mongodb.org/core/community

## Licencse

  The source files in this repository are made available under the terms of the
  Apache License, version 2.0.
