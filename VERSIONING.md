# C++ Driver Versioning

## API Versioning

* We use [semantic versioning](http://semver.org/).
* bsoncxx and mongocxx both define corresponding CMake variables for MAJOR, MINOR, and PATCH.

## ABI Versioning

* Both bsoncxx and mongocxx both have a single scalar ABI version.
* Only bump ABI version on **incompatible** ABI change (not for ABI additions).

## Parallel Header Installation

* For mongocxx, install all headers to `$PREFIX/mongocxx/v$MAJOR.$MINOR/`.
* For bsoncxx, install all headers to `$PREFIX/bsoncxx/v$MAJOR.$MINOR/`.
* We install a pkg-config file to shield consumers from this complexity.

## Sonames and symlinks

*Note that below examples are given for libmongocxx, but also apply to libbsoncxx*

*DSO = Dynamic Shared Object, to use Ulrich Drepper's terminology*

* Physical filename for a DSO is `libmongocxx.so.$MAJOR.$MINOR.$PATCH`

Note that the physical filename is disconnected from ABI version/soname. This looks a bit strange, but allows multiple versions of the library with the same ABI version to be installed on the same system.

* soname for a DSO is `libmongocxx.$ABI`

We provide a soname symlink that links to the physical DSO.
We also provide a dev symlink that links to the soname symlink of the highest ABI version of the library installed.

## Inline namespaces

* We provide inline namespace macros for both mongocxx and bsoncxx.
* This allows multiple, ABI incompatible versions of the library to be linked into the same application.
* The name of the namespace is `v$ABI`. We create them from ABI version to maintain forwards compatibibility.
