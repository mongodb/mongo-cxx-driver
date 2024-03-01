+++
date = "2024-02-26T00:00:00+00:00"
title = "ABI Versioning"
[menu.main]
  weight = 33
  parent="mongocxx3/policies"
+++

The CXX Driver uses Ulrich Drepper's guide to "Defining Stability", the Linux shared library "soname" convention, and the CMake `SOVERSION` target property for ABI versioning.

Per [Ulrich Drepper](https://www.cs.dartmouth.edu/~sergey/cs258/ABI/UlrichDrepper-How-To-Write-Shared-Libraries.pdf):

> The definition of stability should therefore use the _documented_ interface as the basis. Legitimate uses of interfaces should not be affected by changes in the implementation; using interfaces in an undefined way void the warranty. The same is true for using completely undocumented, internal symbols. Those must not be used at all.

Per [The Linux Documentation Project](https://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html):

> Every shared library has a special name called the \`\`soname''. The soname has the prefix \`\`lib'', the name of the library, the phrase \`\`.so'', followed by a period and a version number that is incremented whenever the interface changes (as a special exception, the lowest-level C libraries don't start with \`\`lib''). A fully-qualified soname includes as a prefix the directory it's in; on a working system a fully-qualified soname is simply a symbolic link to the shared library's \`\`real name''.

Per [CMake documentation](https://cmake.org/cmake/help/latest/prop_tgt/SOVERSION.html):

> For shared libraries `VERSION` and `SOVERSION` can be used to specify the build version and ABI version respectively. When building or installing appropriate symlinks are created if the platform supports symlinks and the linker supports so-names. If only one of both is specified the missing is assumed to have the same version number.

For purposes of ABI versioning, we distinguish between the _stable ABI_ and the _unstable ABI_.

The ABI version number is bumped whenever there is a backward incompatible change to the stable ABI as defined below.

**IMPORTANT:** Only the stability of the stable ABI is communicated by the ABI version number. Backward (in)compatible changes to the unstable ABI are not communicated by the ABI version number.

**NOTE:** The ABI versioning policy is only applicable to _shared libraries_. It does not apply to static libraries.

## Binary Compatibility

An _ABI namespace_ is declared within the library's root namespace (see [Source Compatibility]({{< relref "api-versioning#source-compatibility" >}})), prefixed with the letter `v` and followed by either an integer or `_noabi`.

Examples of ABI namespaces include (relative to the global namespace scope):

* `bsoncxx::v_noabi`
* `bsoncxx::v1`
* `bsoncxx::v2`
* `bsoncxx::v99`

An _unstable ABI namespace_ is a namespace with the name `v_noabi`. Any other ABI namespace is a _stable ABI namespace_. The root namespace is not an ABI namespace.

The _stable ABI_ is the set of exported symbols that are used by the public API and declared in a stable ABI namespace, with the following exceptions:

* The symbol or corresponding public API entities are explicitly documented as experimental or not yet prepared for ABI stability.
* The entity is not declared within an ABI namespace.

Including these exceptions, all other exported symbols are considered to be part of the _unstable ABI_.

If there is a symbol that _should_ be exported or part of the stable ABI, but is currently not, please submit a bug report.

**IMPORTANT:** Direct use of exported symbols that bypasses the public API is not supported. All exported symbols (stable or unstable) must be used via the public API. If there is a symbol that _should_ be exported or part of the stable ABI, but is currently not, please submit a bug report.

**IMPORTANT:** A symbol only needs to be _used by_ the public API (even indirectly) to be considered part of the stable ABI. An exported symbol that has _never_ been used by any public API entity in _any_ prior release is not considered part of the stable ABI (see note above).

**NOTE:** The _behavior_ of a stable ABI symbol is not considered part of the stable ABI. The stability of documented behavior is communicated by the API version number, not the ABI version number.

**NOTE:** Some entities that are part of the public API may not be part of the stable ABI (e.g. inline functions, inline variables, and template instantiations of functions and variables). Conversely, some entities that are part of the stable ABI may not be part of the public API (e.g. an exported private member function). See [API Versioning]({{< relref "api-versioning" >}}).

## Build System Compatibility

The stable ABI policy concerning properties of the build system is mostly the same as that for the public API (see [Build System Compatibility]({{< relref "/api-versioning#build-system-compatibility" >}})) with the following differences:

* Instead of properties that directly impact the public API, properties that directly impact the stable ABI are considered part of the stable ABI.
* **The soname is considered part of the stable ABI**. This means, in contrast to the public API, `BSONCXX_OUTPUT_NAME` is considered part of the stable ABI, as it directly impacts the soname of the resulting bsoncxx shared library.
* The "soname" is not applicable on Windows. See [Shared Libraries (MSVC Only)]({{< relref "../api-abi-versioning#shared-libraries-msvc-only" >}}).

**NOTE:** We support the the stability of the public API _per build configuration_. We do not support compatibility of the public API _across_ different build configurations. For example, a shared library generated with `BSONCXX_OUTPUT_NAME=bsoncxx` is not expected to be compatible with a program compiled against a stable ABI produced using a build configuration with `BSONCXX_OUTPUT_NAME=bsoncxx-custom-basename`. (This is particularly important when using multi-config generators such as Visual Studio!)

## Root Namespace Redeclarations

The root namespace provides "redeclarations" of entities declared in an ABI namespace. The ABI namespace may differ across entities being redeclared.

For example, all of the following redeclarations may be present simultaneously:

* `bsoncxx::example::foo` -> `bsoncxx::v_noabi::example::foo`
* `bsoncxx::example::bar` -> `bsoncxx::v1::example::bar`
* `bsoncxx::example::baz` -> `bsoncxx::v2::example::baz`

Root namespace redeclarations are designed to allow for the addition of binary incompatible symbols in `vB` without breaking binary compatibility of `vA` symbols.
They facilitate the deprecation of stable ABI symbols while providing the opportunity for a clean transition from `vA` to `vB` without breaking binary compatibility.
They allow user code to opt into "redeclaration upgrades" that reduces required changes to source code when transitioning from `vA` to `vB`.

These redeclaration upgrades are intended to support a clean transition away from deprecated, to-be-removed ABI symbols across releases.
Accordingly, users are encouraged to use the root namespace declarations by default to opt into these upgrades.
However, users may need to use explicit ABI namespace qualifiers when referencing CXX Driver entities in their own stable ABIs depending on their own stability policy.

The following compatibility table describes when the API major version number or ABI version number must be bumped due to an incompatible change:

| Change Type | Source Compatible | Binary Compatible |
| - | - | - |
| Add a new `vB` symbol | Yes | Yes |
| Upgrade a redeclaration from `vA` to `vB` | Maybe (1) | Yes |
| Remove a `vA` symbol (from the public API) | No | Maybe (2) |
| Remove a `vA` symbol (from the stable ABI) | Maybe (2) | No |

* (1): A `vB` entity could be 100% source compatible with the `vA` API despite requiring use of a new, incompatible stable ABI symbol.
* (2): A stable ABI symbol could be removed from the public API (e.g. via documentation or removal from public headers) while still providing an exported symbol definition for backward compatibility. This is probably unlikely to happen, but if it does, it would be a rare case where the source and binary incompatible changes can be split into separate releases.

**IMPORTANT:** The integer used by an ABI namespace does _NOT_ directly correspond to an ABI version number. The ABI version number is bumped whenever _any_ binary incompatible change occurs, even when it is the removal of a _single_ symbol from the stable ABI. An ABI version number bump from `A` to `B` does _NOT_ imply the deprecation or removal of symbols declared in the ABI namespace `vA` (if one exists).

## Deprecation and Removal

The policy for deprecation and removal of symbols in the stable ABI is the same as that for the public API: see [Deprecation and Removal]({{< relref "api-versioning#deprecation-and-removal" >}}).

A release containing binary incompatible changes will bump the ABI version number rather than the API major version number.
However, it is likely that when the ABI version number is bumped, the API major version number will also be bumped, as a binary incompatible change is likely to also be a source incompatible change.
