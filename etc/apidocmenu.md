# MongoDB C++ Driver

## Driver status by family and version

Stability indicates whether this driver is recommended for production use.
Currently, no drivers guarantee API or ABI stability.

| Family/version       | Stability   | Development         | Purpose                                                      |
| -------------------- | ----------- | ------------------- | ------------------------------------------------------------ |
| (repo master branch) | Unstable    | Active development  | New feature development                                      |
| mongocxx 3.1.x       | Stable      | Bug fixes only      | Current stable C++ driver release, requires C++11            |
| mongocxx 3.0.x       | Stable      | Critical fixes only | Previous stable C++ driver release, requires C++11           |
| legacy   (all)       | Stable      | Critical fixes only | Legacy API stable C++ driver release, no longer recommended  |

## Documentation versions available

| mongocxx                             | legacy                           |
| ------------------------------------ | -------------------------------- |
| [mongocxx-3.2.0](../mongocxx-3.2.0)  | [legacy-1.1.2](../legacy-1.1.2/) |
| [mongocxx-3.1.4](../mongocxx-3.1.4/) | [legacy-1.1.1](../legacy-1.1.1/) |
| [mongocxx-3.1.3](../mongocxx-3.1.3/) | [legacy-1.1.0](../legacy-1.1.0/) |
| [mongocxx-3.1.2](../mongocxx-3.1.2/) | [legacy-1.0.7](../legacy-1.0.7/) |
| [mongocxx-3.1.1](../mongocxx-3.1.1/) | [legacy-1.0.6](../legacy-1.0.6/) |
| [mongocxx-3.1.0](../mongocxx-3.1.0/) | [legacy-1.0.5](../legacy-1.0.5/) |
| [mongocxx-3.0.3](../mongocxx-3.0.3/) | [legacy-1.0.4](../legacy-1.0.4/) |
| [mongocxx-3.0.2](../mongocxx-3.0.2/) | [legacy-1.0.3](../legacy-1.0.3/) |
| [mongocxx-3.0.1](../mongocxx-3.0.1/) | [legacy-1.0.2](../legacy-1.0.2/) |
| [mongocxx-3.0.0](../mongocxx-3.0.0/) | [legacy-1.0.1](../legacy-1.0.1/) |
|                                      | [legacy-1.0.0](../legacy-1.0.0/) |


## MongoDB compatibility

The following compatibility table specifies the driver version(s)
recommended for different versions of MongoDB.  The mongocxx series
is recommended for all new development.

| Family/version | MongoDB 2.4 | MongoDB 2.6 | MongoDB 3.0 | MongoDB 3.2 | MongoDB 3.4 |
| -------------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| mongocxx 3.2.x | ✓           | ✓           | ✓           | ✓           | ✓           |
| mongocxx 3.1.x | ✓           | ✓           | ✓           | ✓           | ✓           |
| mongocxx 3.0.x | ✓           | ✓           | ✓           | ✓           |             |
| legacy   1.1.x | ✓           | ✓           | ✓           | ✓           |             |
| legacy   1.0.x | ✓           | ✓           | ✓           |             |             |

## Resources

* [MongoDB C++ Driver Quickstart](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/tutorial/)
* [MongoDB C++ Driver Manual](https://mongodb.github.io/mongo-cxx-driver/)
* [MongoDB C++ Driver Source Code on GitHub](https://github.com/mongodb/mongo-cxx-driver)
* [MongoDB Database Manual](http://docs.mongodb.com/manual/)

## Bugs and issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## License

The source files in this repository are made available under the terms of
the Apache License, version 2.0.
