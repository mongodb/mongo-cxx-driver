+++
date = "2024-02-26T00:00:00+00:00"
title = "API Versioning"
[menu.main]
  weight = 33
  parent="mongocxx3/policies"
+++

The CXX Driver uses Semantic Versioning (aka SemVer) for API versioning.

Per the [SemVer specification](http://semver.org/):

> Given a version number MAJOR.MINOR.PATCH, increment the:
> 1. MAJOR version when you make incompatible API changes
> 2. MINOR version when you add functionality in a backward compatible manner
> 3. PATCH version when you make backward compatible bug fixes

For purposes of API versioning, we distinguish between the _public API_ and the _private API_:

> For this system to work, you first need to declare a public API. This may consist of documentation or be enforced by the code itself. Regardless, it is important that this API be clear and precise. Once you identify your public API, you communicate changes to it with specific increments to your version number.

The API version number is updated according to SemVer and the definition of the public API below.

**IMPORTANT:** Only the stability of the public API is communicated by the API version number. Backward (in)compatible changes to the private API are not communicated by the API version number.

## Source Compatibility

A _public header file_ is installed to the include directory for any given CMake build configuration. Any other header file is a _private header file_.

A library's _root namespace_ is declared in the global namespace scope, e.g. `bsoncxx`.
All entities owned by the library are declared within the library's root namespace.
Preprocessor macros owned by the library are prefixed with the library name instead, e.g. `BSONCXX_EXAMPLE_MACRO`.

The _public API_ is the set of _documented_ entities that are declared or defined in a public header file within the library's root namespace, with the following exceptions:

* The entity is explicitly documented as private or not intended for public use.
* The entity is declared in a `detail` namespace.

Including these exceptions, all other entities are considered to be part of the _private API_.

If there is an entity that _should_ be documented, but is currently not, please submit a bug report.

**IMPORTANT:** The _documented_ behavior of a public entity is also considered part of the public API.
Behavior that is not documented is considered library-level undefined behavior and is not supported.
If there is behavior that _should_ be documented but is currently not, please submit a bug report.

**NOTE:** Some entities that are part of the public API may not be part of the stable ABI. Conversely, some symbols that are part of the stable ABI may not be part of the public API. See [ABI Versioning Policy]({{< relref "abi-versioning#binary-compatibility" >}}).

## Build System Compatibility

Although the public API depends on the configuration of the build system, the build system itself is **_NOT_** part of the public API.

Properties of the build system include:

* CMake configuration variables and corresponding behavior (with some exceptions: see below).
* The name, location, and contents of:
  * CMake source files (e.g. under `${PROJECT_SOURCE_DIR}`).
  * CMake binary files (e.g. under `${PROJECT_BINARY_DIR}`).
  * Installed library files (e.g. under `{CMAKE_INSTALL_LIBDIR}`).
  * Installed package files (e.g. under `{CMAKE_INSTALL_LIBDIR}/pkgconfig`).

However, build system configuration variables that directly impact the public API are considered part of the public API.

For example, C++17 polyfills declared in the `bsoncxx::stdx` namespace depend on configuration variables that determine the polyfill library used, e.g. `CMAKE_CXX_STANDARD`, `BSONCXX_POLY_USE_STD`, etc..
A build configuration using `CMAKE_CXX_STANDARD=17` produces a public API where `bsoncxx::stdx::optional<T> == std::optional<T>`.
Because changing the type of `bsoncxx::stdx::optional<T>` such that it is no longer equivalent to `std::optional<T>` is a breaking change to the public API, changing the polyfill library selection behavior of `CMAKE_CXX_STANDARD` is also a breaking change to the public API.
Therefore, `CMAKE_CXX_STANDARD` is considered part of the public API.

On the other hand, `BSONCXX_OUTPUT_NAME` controls the `<basename>` used to generate library and package filenames.
Changing the name of a library or package file does not directly impact the public API (even if existing projects or applications may fail to configure, build, or link due to unexpected filenames).
Therefore, `BSONCXX_OUTPUT_NAME` is _not_ considered part of the public API.

**NOTE:** We support the the stability of the public API _per build configuration_. We do not support compatibility of the public API _across_ different build configurations. For example, a public API produced using a build configuration with `CMAKE_BUILD_TYPE=Debug` is not expected to be compatible with a program compiled against a public API produced using a build configuration with `CMAKE_BUILD_TYPE=Release`. (This is particularly important when using multi-config generators such as Visual Studio!)

## Root Namespace Redeclarations

The API of CXX Driver libraries uses root namespace redeclarations of ABI namespace entities to facilitate incremental changes to the ABI: see [Root Namespace Redeclarations]({{< relref "abi-versioning#root-namespace-redeclarations" >}}).

Entities that are not part of the stable ABI or the public API may be declared in the root namespace without being a member of any particular ABI namespace (e.g. `bsoncxx::detail`).

## Deprecation and Removal

Before making a backward incompatible change, the existing entity or behavior will first be deprecated in a minor release.
The backward incompatible change will be finalized in a major release that changes or removes the deprecated entity or behavior.

When able, the backward incompatible entity or behavior will be provided alongside its deprecated counterpart to provide an opportunity for a clean transition in the minor release.
Users are strongly encouraged to apply the clean transition when such an opportunity is provided: please do not continue to use deprecated entities or behavior.
(Note: providing a clean transition path in a minor release may not always be possible.)

A deprecated entity or behavior will be documented using one or more of the following methods (not exhaustive):

* Use a `*_DEPRECATED` macro in the declaration of the entity.
* Include "deprecated" in the name of the entity.
* Explicitly document an entity or behavior as deprecated.
