# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=develop)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

This branch contains active development on a new driver written in C++11.

> **WARNING:** This branch is being actively developed. For a stable driver, switch to the ['legacy'](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) or ['26compat'](https://github.com/mongodb/mongo-cxx-driver/tree/26compat) branch.

#### Why a rewrite?
The new driver will contain a completely new API and implementation. A complete rewrite allows us to take full advantage of the features offered in C++11, and gives us the chance to re-architect the driver based on past driver design decisions and their effectiveness. We aim for this new driver to be more modern, more lightweight, and friendlier to contributors than its predecessors.

## Repository Overview
| Branch   | Stability   | Development       | Purpose                                                      |
| -------- | ------------| ----------------- | ------------------------------------------------------------ |
| master   | Unstable    | Planning          | New C++11 driver (work in progress)                          |
| legacy   | Stable      | Stable Evolution  | Primary stable C++ driver release                            |
| 26compat | Stable      | Maintenance Only  | Drop in replacement for users of existing 2.6 era C++ driver |

## Bugs and Issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## Mailing Lists and IRC

Outlined on the [MongoDB Community site](http://dochub.mongodb.org/core/community).

## License

The source files in this repository are made available under the terms of the Apache License, version 2.0.
