# MongoDB C++ Driver [![Evergreen Build Status](https://evergreen.mongodb.com/static/img/favicon.ico)](https://evergreen.mongodb.com/waterfall/cxx-driver)[![Travis Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=master)](https://travis-ci.org/mongodb/mongo-cxx-driver)

Welcome to the MongoDB C++ Driver!

## Branches - `releases/stable` versus `master`

The default checkout branch of this repository is `releases/stable`. 
This will always contain the latest stable release of the driver. The
 `master` branch is used for active development. `master` should 
**only** be used when making contributions back to the driver, as it 
is not stable for use in production.

See [Driver Status by family and version](#driver-status-by-family-and-version)
for more details about the various versions of the driver.

## Resources

* [MongoDB C++ Driver Quickstart](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/tutorial/)
* [MongoDB C++ Driver Manual](https://mongodb.github.io/mongo-cxx-driver/)
* [MongoDB C++ Driver API Documentation](https://mongodb.github.io/mongo-cxx-driver/api/mongocxx-v3)
* [MongoDB C++ Driver Contribution guidelines](https://mongodb.github.io/mongo-cxx-driver/contributing/)
* [MongoDB Database Manual](http://docs.mongodb.com/manual/)

## Driver status by family and version

Stability indicates whether this driver is recommended for production use.
Currently, no drivers guarantee API or ABI stability.

| Family/version       | Stability   | Development         | Purpose                             |
| -------------------- | ----------- | ------------------- | ----------------------------------- |
| (repo master branch) | Unstable    | Active development  | New feature development             |
| mongocxx 3.3.x       | Stable      | Bug fixes only      | Current stable C++ driver release   |
| mongocxx 3.2.x       | Stable      | None                | Previous stable C++ driver release  |
| mongocxx 3.1.x       | Stable      | None                | Previous stable C++ driver release  |
| mongocxx 3.0.x       | Stable      | None                | Previous stable C++ driver release  |

## MongoDB compatibility

The following compatibility table specifies the driver version(s)
recommended for different versions of MongoDB.  The mongocxx series
is recommended for all new development.

| Family/version | MongoDB 3.0 | MongoDB 3.2 | MongoDB 3.4 | MongoDB 3.6 |
| -------------- | ----------- | ----------- | ----------- | ----------- |
| mongocxx 3.3.x | ✓           | ✓           | ✓           | Complete    |
| mongocxx 3.2.x | ✓           | ✓           | ✓           | Partial     |
| mongocxx 3.1.x | ✓           | ✓           | ✓           |             |
| mongocxx 3.0.x | ✓           | ✓           |             |             |

## Bugs and issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## License

The source files in this repository are made available under the terms of
the Apache License, version 2.0.
