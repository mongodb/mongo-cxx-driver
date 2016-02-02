# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=master)](https://travis-ci.org/mongodb/mongo-cxx-driver)[![Windows Build Status](https://ci.appveyor.com/api/projects/status/w95xuowdjpr0img1/branch/master?svg=true)](https://ci.appveyor.com/project/markbenvenuto/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

This branch contains the new C++11 driver and is the recommended
branch for all users. Please see the README.md files in the "legacy"
and "26compat" branches for notes on when to use those release
streams.

#### Quickstart Guide

See the [wiki page](https://github.com/mongodb/mongo-cxx-driver/wiki/Quickstart-Guide-(New-Driver)).

#### Why a rewrite?
This driver contains a completely new API and implementation. A
complete rewrite allows us to take full advantage of the features
offered in C++11, and gives us the chance to re-architect the driver
based on past driver design decisions and their effectiveness. We aim
for this new driver to be more modern, more lightweight, and
friendlier to contributors than its predecessors.

## Repository Overview
| Branch   | Stability   | Development       | Purpose                                                      |
| -------- | ------------| ----------------- | ------------------------------------------------------------ |
| master   | Stable      | Stable Evolution  | New C++11 driver                                             |
| legacy   | Stable      | Stable Evolution  | Stable C++ driver release                                    |
| 26compat | Stable      | Maintenance Only  | Drop in replacement for users of existing 2.6 era C++ driver |

## Bugs and Issues

See our [JIRA project](http://jira.mongodb.org/browse/CXX).

## Mailing Lists and IRC

Outlined on the [MongoDB Community site](http://dochub.mongodb.org/core/community).

## License

The source files in this repository are made available under the terms of the Apache License, version 2.0.
