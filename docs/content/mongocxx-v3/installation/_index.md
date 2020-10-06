+++
date = "2020-09-26T13:14:52-04:00"
title = "Installing the mongocxx driver"
[menu.main]
  identifier = "mongocxx3-installation"
  parent = "mongocxx3"
  weight = 8
+++

## Prerequisites

- Any standard Unix platform, or Windows 7 SP1+
- A compiler that supports C++11 (gcc, clang, or Visual Studio)
- [CMake](https://cmake.org) 3.2 or later
- [boost](https://www.boost.org) headers (optional)

We currently test the driver on the following platforms:

- Linux with clang 3.8 and 6.0, GCC 5.4 and 7.5
- macOS with Apple clang 11.0 using Boost 1.70.0
- Windows with Visual Studio 2015 using Boost 1.60.0 and Visual Studio 2017

Versions older than the ones listed may not work and are not
supported; use them at your own risk.

Versions newer than the ones listed above should work; if you
have problems, please file a bug report via
[JIRA](https://jira.mongodb.org/browse/CXX/).

## Installation

To configure and install the driver, follow the instructions for your platform:

* [Configuring and installing on Windows]({{< ref "/mongocxx-v3/installation/windows" >}})
* [Configuring and installing on macOS]({{< ref "/mongocxx-v3/installation/macos" >}})
* [Configuring and installing on Linux]({{< ref "/mongocxx-v3/installation/linux" >}})

## Advanced Options

* [Advanced Configuration and Installation Options]({{< ref "/mongocxx-v3/installation/advanced" >}})

