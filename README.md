# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=3.1-dev)](https://travis-ci.org/mongodb/mongo-cxx-driver)[![Windows Build Status](https://ci.appveyor.com/api/projects/status/w95xuowdjpr0img1/branch/3.1-dev?svg=true)](https://ci.appveyor.com/project/markbenvenuto/mongo-cxx-driver)

Welcome to the MongoDB C++ Driver!

## Resources

* [MongoDB C++ Driver Quickstart](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/tutorial/)
* [MongoDB C++ Driver Manual](https://mongodb.github.io/mongo-cxx-driver/)
* [MongoDB C++ Driver API Documentation](https://mongodb.github.io/mongo-cxx-driver/api/mongocxx-v3)
* [MongoDB C++ Driver Contribution guidelines](https://mongodb.github.io/mongo-cxx-driver/contributing/)
* [MongoDB Database Manual](http://docs.mongodb.com/manual/)

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

| Family/version | MongoDB 2.4 | MongoDB 2.6 | MongoDB 3.0 | MongoDB 3.2 | MongoDB 3.4 |
| ---------------| ------------| ------------| ------------| ------------|-------------|
| mongocxx 3.1.x | ✓           | ✓           | ✓           | ✓           | ✓           |
| mongocxx 3.0.x | ✓           | ✓           | ✓           | ✓           |             |
| legacy   1.1.x | ✓           | ✓           | ✓           | ✓           |             |
| legacy   1.0.x | ✓           | ✓           | ✓           |             |             |
| 26compat (all) |             | ✓           |             |             |             |

## Bugs and issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## License

The source files in this repository are made available under the terms of
the Apache License, version 2.0.
