# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=master)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

**This is not the branch you want.**

Switch to the legacy branch if:

 - You are using MongoDB's C++ driver for the first time.
 - You had been using 26compat (or the driver inside of the server source) and
 want to benefit from incremental improvements but have the same overall API.

Switch to the 26compat branch if:

 - You have existing code that used the driver from the server source
 and want it to continue working without modification.

**Please Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the
driver from the server sources: this repository is the only approved source for
driver builds.

This branch will contain future development work on a new driver with a
completely new API and implementation. It is a work in progress and currently
should not be used.

## Repository Overview

| Branch   | Development        | Purpose                                               |
| -------- | ------------------ | ----------------------------------------------------- |
| master   | Planning           | New C++ driver (work in progress)                     |
| legacy   | Active             | Existing C++ driver with non-compatible improvements  |
| 26compat | Maintenance Only   | Drop in replacement for users of existing C++ driver  |

