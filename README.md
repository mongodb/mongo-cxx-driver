# MongoDB C++ Driver [![Build Status](https://travis-ci.org/mongodb/mongo-cxx-driver.svg?branch=master)](https://travis-ci.org/mongodb/mongo-cxx-driver)
Welcome to the MongoDB C++ Driver!

> **WARNING:** This is not the branch you want, there is nothing here yet.

Switch to the legacy branch if:

 - You are using MongoDB's C++ driver for the first time and do not mind the driver being actively worked on.
 - You had been using 26compat (or the driver inside of the server source) and want to benefit from incremental improvements while having the same overall API.

Switch to the 26compat branch if:

 - You need a production ready release of the C++ driver that is not under active development.
 - You have existing code that used the driver from the server source and want it to continue working without modification.

> **Note:** As of MongoDB 2.6.0-rc1, it is no longer possible to build the driver from the server sources: this repository is the only approved source for driver builds.

This branch will contain future development work on a new driver with a completely new API and implementation. It is a work in progress and currently should not be used.

## Repository Overview

| Branch   | Stability | Development       | Purpose                                               |
| -------- | ----------| ----------------- | ----------------------------------------------------- |
| master   | Unstable  | Planning          | New C++ driver (work in progress)                     |
| legacy   | Unstable  | Active            | Existing C++ driver with non-compatible improvements  |
| 26compat | Stable    | Maintenance Only  | Drop in replacement for users of existing C++ driver  |

