# MongoDB Legacy C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.png?branch=legacy)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB Legacy C++ Driver!

This driver is an extraction of the "Server C++ Driver" from the primary
MongoDB sources. You should prefer using this to building the driver
from the server sources if at all possible. Please note that as of
MongoDB 2.6.0-rc1, it is no longer possible to build the driver from
the server sources: this repo is the only approved source for driver builds.

## Components

  libmongoclient.[so|dylib|dll] - The shared mongoclient library
  libmongoclient.a - The static mongoclient library (but see notes)

## Building

  See http://docs.mongodb.org/ecosystem/tutorial/download-and-compile-cpp-driver/

  The above instructions (currently) assume that you are building from the server
  repository, but the will work with this repository as described.

## Notes

  Use of the static library is discouraged, and support may be removed soon. To properly
  link against the static library on ELF platforms, you must link it with the
  --whole-archive flag to ld. The static library is not useable on Windows since
  there is no analogue to --whole-archive. The static library may work on OS X, but
  we still strongly recommend using the dynamic library.

## Documentation

  http://docs.mongodb.org/ecosystem/drivers/cpp/

## Mailing Lists and IRC

  http://dochub.mongodb.org/core/community

## Licencse

  The source files in this repository are made available under the terms of the
  Apache License, version 2.0.
