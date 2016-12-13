# MongoDB C++ Driver

## Driver status by family and version

Stability indicates whether this driver is recommended for production use.
Currently, no drivers guarantee API or ABI stability.

| Family/version | Stability   | Development         | Purpose                                                      |
| ---------------| ------------| ------------------- | ------------------------------------------------------------ |
| mongocxx 3.1.x | Unstable    | Active development  | Unstable C++ driver development, requires C++11              |
| mongocxx 3.0.x | Stable      | Bug fixes only      | Primary stable C++ driver release, requires C++11            |
| legacy   (all) | Stable      | Critical fixes only | Former stable C++ driver release                             |
| 26compat (all) | Deprecated  | Critical fixes only | Drop in replacement for users of existing 2.6 era C++ driver |

## Documentation versions available

| mongocxx                             | legacy
| -------------------------------------| --------------------------------
| [mongocxx-3.1.0](../mongocxx-3.1.0/) | [legacy-1.1.2](../legacy-1.1.2/)
| [mongocxx-3.0.3](../mongocxx-3.0.3/) | [legacy-1.1.1](../legacy-1.1.1/)
| [mongocxx-3.0.2](../mongocxx-3.0.2/) | [legacy-1.1.0](../legacy-1.1.0/)
| [mongocxx-3.0.1](../mongocxx-3.0.1/) | [legacy-1.0.7](../legacy-1.0.7/)
| [mongocxx-3.0.0](../mongocxx-3.0.0/) | [legacy-1.0.6](../legacy-1.0.6/)
|                                      | [legacy-1.0.5](../legacy-1.0.5/)
|                                      | [legacy-1.0.4](../legacy-1.0.4/)
|                                      | [legacy-1.0.3](../legacy-1.0.3/)
|                                      | [legacy-1.0.2](../legacy-1.0.2/)
|                                      | [legacy-1.0.1](../legacy-1.0.1/)
|                                      | [legacy-1.0.0](../legacy-1.0.0/)

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
