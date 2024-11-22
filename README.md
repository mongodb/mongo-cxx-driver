# MongoDB C++ Driver 
[![codecov](https://codecov.io/gh/mongodb/mongo-cxx-driver/branch/master/graph/badge.svg)](https://codecov.io/gh/mongodb/mongo-cxx-driver)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://mongocxx.org/api/mongocxx-v3/)
[![Documentation](https://img.shields.io/badge/docs-mongocxx-green.svg)](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/mongodb/mongo-cxx-driver/blob/master/LICENSE)

Welcome to the MongoDB C++ Driver!

## Latest Stable Release

The `releases/stable` branch indicates the latest stable release.
This branch corresponds to the driver version which we currently recommend for use in production.
By default, new users should clone or download the driver version which is currently indicated by this branch.

```bash
# Use the latest stable release which is currently recommended for use.
git clone -b releases/stable https://github.com/mongodb/mongo-cxx-driver.git
```

> [!NOTE]
> This branch is regularly _force-pushed_ to refer to the latest stable release.

## Driver Development Status

> [!IMPORTANT]
> Relevant bug fixes will be backported from the current major version to the previous major version for a period of one year after the new major version is released.

| Version     | ABI Stability   | Development Stability       | Development Status |
| :---------: | :-------------: | :-------------------------: | :----------------: |
| master      | N/A             | _Do not use in production!_ | Active             |
| 4.0.0       | None            | Ready for Use               | Bug Fixes Only     |
| 3.11.0      | None            | Ready for Use               | Bug Fixes Only     |
| 3.10.2      | None            | Ready for Use               | Not Supported      |
| ...         | ...             | ...                         | ...                |
| 3.0.0       | None            | Ready for Use               | Not Supported      |

## MongoDB Server Compatibility

MongoDB server version compatibility per driver version is documented in the [MongoDB Manual](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/compatibility/).

## Bugs and Issues

See our [JIRA project](https://jira.mongodb.com/browse/CXX).

## Resources

* [MongoDB C++ Driver Installation](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/)
* [MongoDB C++ Driver Quickstart](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/tutorial/)
* [MongoDB CXX Driver Examples](https://github.com/mongodb/mongo-cxx-driver/tree/master/examples)
* [MongoDB C++ Driver Manual](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/)
* [MongoDB C++ Driver Documentation](https://www.mongodb.com/docs/drivers/cxx/)
* [MongoDB C++ Driver API Documentation](https://mongocxx.org/api/current/)
* [MongoDB C++ Driver Contribution guidelines](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/contributing/)
* [MongoDB Database Manual](https://www.mongodb.com/docs/manual/)
* [MongoDB Developer Center](https://www.mongodb.com/developer/languages/cpp/)
* [StackOverflow](https://stackoverflow.com/questions/tagged/mongodb%20c%2b%2b)

## License

The source files in this repository are made available under the terms of
the Apache License, version 2.0.
