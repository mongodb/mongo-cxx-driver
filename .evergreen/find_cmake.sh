#!/bin/sh
set -o xtrace   # Write all commands first to stderr
set -o errexit  # Exit the script with error if any of the commands fail

OS=$(uname -s | tr '[:upper:]' '[:lower:]')

find_cmake ()
{
  if [ ! -z "$CMAKE" ]; then
    return 0
  elif [ -f "/Applications/cmake-3.2.2-Darwin-x86_64/CMake.app/Contents/bin/cmake" ]; then
    CMAKE="/Applications/cmake-3.2.2-Darwin-x86_64/CMake.app/Contents/bin/cmake"
  elif [ -f "/Applications/Cmake.app/Contents/bin/cmake" ]; then
    CMAKE="/Applications/Cmake.app/Contents/bin/cmake"
  elif [ -f "/opt/cmake/bin/cmake" ]; then
    CMAKE="/opt/cmake/bin/cmake"
  elif command -v cmake 2>/dev/null; then
     CMAKE=cmake
  elif uname -a | grep -iq 'x86_64 GNU/Linux'; then
     curl --retry 5 https://cmake.org/files/v3.11/cmake-3.11.0-Linux-x86_64.tar.gz -sS --max-time 120 --fail --output cmake.tar.gz
     mkdir cmake-3.11.0
     tar xzf cmake.tar.gz -C cmake-3.11.0 --strip-components=1
     CMAKE=$(pwd)/cmake-3.11.0/bin/cmake
  elif [ "$OS" == "cygwin"* ]; then
    CMAKE=/cygdrive/c/cmake/bin/cmake
  else
     # If cmake unavailable, create a BUILD ticket.
     echo "cmake not available"
     return 1
  fi
}

find_cmake