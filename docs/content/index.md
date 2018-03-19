+++
date = "2016-08-15T16:11:58+05:30"
title = "MongoDB C++ Driver Manual"
type = "index"
+++

# MongoDB C++ Driver

Welcome to the MongoDB C++ driver.  On this site, you'll find documentation
to help you get the most from MongoDB and C++.  If you're just starting
out, take a look at these pages first:

* [Installing mongocxx]({{< ref "mongocxx-v3/installation.md" >}})
* [Quick-start tutorial]({{< ref "mongocxx-v3/tutorial.md" >}})

## Driver status by family and version

Stability indicates whether this driver is recommended for production use.
Currently, no drivers guarantee API or ABI stability.

| Family/version       | Stability   | Development         | Purpose                                                      |
| -------------------- | ----------- | ------------------- | ------------------------------------------------------------ |
| (repo master branch) | Unstable    | Active development  | New feature development                                      |
| mongocxx 3.2.x       | Stable      | Bug fixes only      | Current stable C++ driver release, requires C++11            |
| mongocxx 3.1.x       | Stable      | Bug fixes only      | Older stable C++ driver release, requires C++11              |
| mongocxx 3.0.x       | Stable      | Critical fixes only | Older stable C++ driver release, requires C++11              |
| legacy   (all)       | EOL         | End of Life         | Legacy API stable C++ driver release                         |

## MongoDB compatibility

The following compatibility table specifies the driver version(s)
recommended for different versions of MongoDB.  The mongocxx series
is recommended for all new development.

| Family/version | MongoDB 3.0 | MongoDB 3.2 | MongoDB 3.4 | MongoDB 3.6 |
| -------------- | ----------- | ----------- | ----------- | ----------- |
| mongocxx 3.2.x | ✓           | ✓           | ✓           | Partial     |
| mongocxx 3.1.x | ✓           | ✓           | ✓           |             |
| mongocxx 3.0.x | ✓           | ✓           |             |             |
| legacy   1.1.x | ✓           | ✓           |             |             |
| legacy   1.0.x | ✓           |             |             |             |

## License

MongoDB C++ drivers are available under the terms of the Apache License, version 2.0.
