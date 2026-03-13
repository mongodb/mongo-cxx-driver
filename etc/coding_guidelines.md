# MongoDB C++ Driver Coding Guidelines

The bsoncxx and mongocxx libraries largely follow the same structure and patterns. For brevity, descriptions pertaining to bsoncxx also apply to mongocxx unless otherwise noted.

These guidelines are not exhaustive. Within reason, prioritize consistency with existing code, and consistency with newer code over older code.

See also:

- [API and ABI Versioning](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/)
- [MongoDB Server C++ Style Guide](https://github.com/mongodb/mongo/blob/master/docs/cpp_style.md)
- [CppCoreGuidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Google Style Guide](https://google.github.io/styleguide/cppguide.html)

## Project Directory Structure

The bsoncxx and mongocxx libraries are each organized under a `src/<library>` directory.

See [ABI Versioning](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/abi-versioning/) regarding how ABI directories and namespaces are intended to be used.

The basic directory structure for `<library>` is:

```
<library>/
├── include/<library>/
│   ├── docs/
│   ├── v_noabi/<library>/
│   ├── v1/
│   └── ...
├── lib/<library>/
│   ├── private/
│   ├── v_noabi/<library>/
│   ├── v1/
│   └── ...
├── test/
│   ├── private/
│   ├── v_noabi/
│   ├── v1/
│   └── ...
└── ...
```

- `<library>/include/` contains "public headers" which are installed as-is into the install prefix.
    - A header under `v<N>` must never include a header under `v_noabi` (stable vs unstable ABI).
    - A header under `v<N>` must never include a header under `v<less-than-N>` (ABI backward compatibility).
- `<library>/lib/` contains internal headers and implementation files.
    - Files under `private/` provide internal interfaces which are reusable across ABI components (do not affect ABI).
    - The layout of `lib/v<abi>/...` mirrors the layout of `include/v<abi>/...`.
    - Components under `v<N>` must never use components under `v_noabi` (stable vs unstable ABI).
    - Components `v<N>` must never use components under `v<less-than-N>` (ABI backward compatibility).
- `<library>/test/` contain test files.
    - Files under `test/...` mirrors the layout of `lib/...`.
    - Test files can use any ABI component as needed.

The `<library>/lib/` directory also contains "input" files (e.g. `foo.ext.in`) used to produce "generated files" (e.g. `foo.ext`) during CMake configuration and build. A generated file (e.g. `config.hpp`) is produced using an input file (e.g. `config.hpp.in`) within the "same" directory ("source" vs. "binary" CMake directories).

### Component Design

A component `foo` may have up to six associated source files:

```
src/<library>/
├── include/bsoncxx/v<abi>/
│   ├── foo-fwd.hpp (Forward Header, optional)
│   └── foo.hpp     (Normal Header)
├── lib/bsoncxx/v<abi>/
│   ├── foo.hh  (Internal / Private Header, optional)
│   └── foo.cpp (Implementation / Source File)
├── test/v<abi>/
│   ├── foo.hh  (Test Header, optional)
│   └── foo.cpp (Test Implementation / Source File)
└── ...
```

- A component in directory `v<abi>/foo/bar/` must declare interfaces in namespace `v<abi>::foo::bar`.
    - This does not apply to detail, internal, private, and test interfaces.
- Include What You Use priority:
    - The forward header must be the first include directive in the normal header.
    - The normal header must be the first include directive in the internal header.
    - The internal (or normal) header must be the first include directive in the implementation file.
    - The internal (or normal) header must be the first include directive in the test header.
    - The test (or internal, or normal) header must be the first include directive in the test implementation file.
- The forward header declares (but does not define) class types.
    - Root namespace redeclarations for class types belong here.
- The normal header declares functions, declares variables, defines class types, and defines type aliases.
    - Root namespace redeclarations for functions and variables belong here.
- The internal header declares and defines internal interfaces.
    - `BSONCXX_ABI_EXPORT_CDECL_TESTING` may be required to use internal interfaces in test components.
- The implementation file defines all declared functions and declared variables.
- The test header defines Catch2 `StringMaker<T>` specializations.
    - Tests MUST include the test header (containing `StringMaker<T>` specializations) when one exists instead of the declaration header.
- The test implementation file defines test cases for all related component interfaces.
    - Test implementation files MAY include internal headers for components being tested.
    - Internal interfaces required by test files MUST be exported using `BSONCXX_ABI_EXPORT_CDECL_TESTING`.

### Namespaces

The library root namespace declares ABI namespaces (e.g. `mongocxx::v_noabi`, `mongocxx::v1`, etc.), within which symbols are declared according to their compatibility with an ABI version.

The library root namespace also redeclares ABI-specific entities without an ABI namespace qualifier (e.g. `mongocxx::v_noabi::document::view` as `mongocxx::document::view`) to allow users to automatically opt-into the latest supported ABI version of a given entity without requiring changes to source code. The root namespace redeclarations are intended to be the default method for using library entities. A user should only include the ABI namespace in a qualifier if they require compatibility with that specific ABI version.

References to ABI-specific entities in ABI namespaces MUST always be (un)qualified such that it is not affected by changes to root namespace redeclarations:

```cpp
namespace mongocxx {
namespace v_noabi {
namespace example {

struct type {}; // The type intended to be referenced below.

// OK: always resolves to `mongocxx::v_noabi::example::type`.
void fn(type param);

// Also OK: unambiguously refers to the ABI-specific type.
void fn(v_noabi::example::type param);

}  // namespace example
}  // namespace v_noabi
}  // namespace mongocxx
```

To avoid being affected by changes to root namespace redeclarations, interfaces declared within an ABI namespace MUST NOT be written in terms of a root namespace redeclaration:

```cpp
namespace mongocxx {
namespace v_noabi {
namespace example {

struct type {}; // The type intended to be referenced below.

// Problem: when `mongocxx::example::type` is changed from `v_noabi` to `v1`,
// this parameter type will also (incorrectly) change from `v_noabi` to `v1`.
void fn(mongocxx::example::type param);

}  // namespace example
}  // namespace v_noabi
}  // namespace mongocxx
```

### Public Headers

Public headers are organized under `src/<library>/include/`. With the exception of generated headers, all header files under this directory are installed as-is to the install prefix with their existing structure. This includes headers within `detail` subdirectories, which are headers reserved for internal use only and are not a part of the public API.

```
include/
├── v_noabi/bsoncxx/
│   ├── fwd.hpp
│   ├── detail/
│   │   └── ...
│   ├── document/
│   │   ├── value-fwd.hpp
│   │   ├── value.hpp
│   │   └── ...
│   └── ...
├── v1/
│   ├── fwd.hpp
│   ├── detail/
│   │   └── ...
│   ├── document/
│   │   ├── value-fwd.hpp
│   │   ├── value.hpp
│   │   └── ...
│   └── ...
└── CMakeLists.txt
```

#### Stable ABI Headers

Headers under `vN/` directories provide stable ABI interfaces declared within ABI namespace `vN`.

#### Unstable ABI Headers

Headers under `v_noabi/` declare both unstable (`v_noabi`) AND stable ABI interfaces. See [Unstable ABI Namespace](#unstable-abi-namespace) below.

> [!IMPORTANT]
> Headers under `v_noabi/` MUST be placed under the additional `bsoncxx/` (or `mongocxx`) subdirectory for backward compatibility with unstable ABI header direct include style: `#include <bsoncxx/document/element.hpp>`.

#### Generated Headers

The input files with the `.in` extension for generated headers are located under `lib/` in the directory mirroring its install location under a `config/` subdirectory. Generation is handled via the CMake configuration file in the `lib/` directory.

```
lib/
├── v1/
│   ├── config/
│   │   ├── config.hpp.in # Input file to generate config.hpp.
│   │   └── ...
│   └── ...
└── CMakeLists.txt # Responsible for configuration and installation.
```

#### Forward Headers

Forward headers with the `-fwd` basename suffix declare (but do not define!) class types and enumerations provided by their corresponding declaration header.

```
include/
├── fwd.hpp
├── v_noabi/bsoncxx/
│   ├── fwd.hpp
│   ├── document/
│   │   ├── value-fwd.hpp # Forward header.
│   │   ├── value.hpp
│   │   └── ...
│   └── ...
├── v1/
│   ├── fwd.hpp
│   ├── document/
│   │   ├── value-fwd.hpp # Forward header.
│   │   ├── value.hpp
│   │   └── ...
│   └── ...
└── CMakeLists.txt
```

The special `fwd.hpp` headers present in the ABI (`v_noabi/`, `v1/`, etc.) and library root (`bsoncxx/`, etc.) directories provide all forward declarations for the given ABI namespace.

#### Source Files

Source files are organized under `lib/`. Their structure generally mirrors that of the header files they correspond to. Otherwise, implementation-specific source files are expected to be located under `detail/`.

```
lib/
├── bsoncxx/
│   ├── private/
│   │   ├── immortal.hh  # Private header for internal usage only.
│   │   ├── immortal.cpp # Implementation of entities declared in immortal.hh.
│   │   └── ...
│   ├── v_noabi/
│   │   └── ...
│   └── v1/
│       ├── config/
│       │   ├── config.hpp.in # Input file to generate config.hpp.
│       │   └── ...
│       ├── document/
│       │   ├── value.hh  # Internal declarations and definitions for value.hpp.
│       │   ├── value.cpp # Definitions of entities declared in value.hpp and value.hh.
│       │   └── ...
│       └── ...
└── CMakeLists.txt # Responsible for configuration, generation, and installation.
```

> [!NOTE]
> Some source files may only contain a single include directive of the corresponding (public/internal) header. This is deliberate to ensure the header is standalone-includeable.

## C++ Style Guide

### Namespaces

- The namespace(s) provided by a given component should reflect its parent directory (e.g. namespace `v<abi>::foo::bar` in directory `v<abi>/foo/bar/`).
- Qualify references to entities which are not provided by the given component with the ABI namespace of the referenced entity (e.g. `v<abi>::bar` from component `foo`).
    - This also applies to the `detail` namespace within public headers (e.g. `detail::bar`).
    - This does not apply to other internal namespaces (e.g. `test`).

### Inline Definitions

- Do not export a template or template instantiation.
- Do not export entities declared within the `stdx` namespace (C++ standard library polyfills).
- For functions and operator overloads:
    - Prefer inline function definitions when:
        - The definition does not require additional `#include` dependencies.
        - The definition has a wide contract (no preconditions) for API forward compatibility.
    - Otherwise, define the extern function out-of-line within the implementation file.
    - Only export an extern function when it is required by the public API.
- For variables:
    - Prefer `static constexpr` variables with inline definitions for literal types.
    - Otherwise, define the extern variable out-of-line within the implementation file.
    - Only export an extern variable when it is required by the public API.

> [!NOTE]
> - `inline` variables require C++17 and newer.
> - `constexpr` implies `inline` for variables only in C++17 and newer.
> - Before C++17, non-`inline` `constexpr` variables which are ODR-used require an out-of-line definition.
> - Use `BSONCXX_PRIVATE_INLINE_CXX17` for pre-C++17 compatibility.

### Export Macros

- Use `BSONCXX_ABI_EXPORT_CDECL` to export functions and operator overloads.
    - The `v` in `BSONCXX_ABI_EXPORT_CDECL(v)` must be the return type. For constructor and destructors, use `BSONCXX_ABI_EXPORT_CDECL()` without any `v`.
    - `BSONCXX_ABI_EXPORT_CDECL` must be immediately before the name of the function or operator overload, after any and all specifiers (e.g. `static`, `explicit`, etc.).
- Use `BSONCXX_ABI_EXPORT` to export variables.
- Only polymorphic classes (e.g. exceptions) should be declared with `BSONCXX_ABI_EXPORT`.
    - `BSONCXX_ABI_EXPORT` must be applied to the _first_ declaration of the class (e.g. in the forward header when applicable).
    - All polymoprhic classes must define _at least one_ virtual function (e.g. the virtual destructor) out-of-line within the implementation file as the ["key function"](https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-vtable).
- Use `BSONCXX_ABI_CDECL` to declare all (pointer to) function types which are referenced by the ABI with the `__cdecl` calling convention.
    - For function types: `ReturnType BSONCXX_ABI_CDECL(Params...)`.
    - For pointer-to-function types: `ReturnType (BSONCXX_ABI_CDECL*)(Params...)`.
    - This includes function types used as arguments to template parameters (e.g. `std::function<R BSONCXX_ABI_CDECL(Params...)>`).

### Implicit vs. Explicit

- A (potential) single-argument constructor or user-defined conversion function (UDCF) should be `explicit` by default.
    - "Potential" includes default arguments or parameter packs which permit `To a{b};` or `To a = b;` given the type of `b` is not `To`.
    - A UDCF is analogous to a single-argument "constructor" defined by the source type rather than by the target type.
- A (potential) single-argument constructor or UDCF may be implicit when:
    - `To` is a non-owning, read-only "view" of an owning type `From`, both of which represent the same conceptual value (e.g. `From = std::string` and `To = std::string_view`).
    - `To` is an owning type whose conceptual value may be expressed as one or more `From` types (e.g. `From = T` and `To = std::optional<T>`).
    - `To` is unambiguously preferable to `From` given an overload set containing both types (e.g. `std::string` vs. `std::string_view` given `StringViewLike` favors `std::string_view`).
    - The conversion has a wide contract (no preconditions) and the operation cannot fail (throw an exception, terminate, or lead to undefined behavior).

> [!NOTE]
> Use implicit single-argument constructors and UDCFs sparingly. Prefer explicit to implicit. The "convenience" of supporting implicit conversion must sufficiently outweigh the possibility of introducing ambiguous overloads, both with types provided by the library and types defined by the user.

> [!NOTE]
> Application of `explicit` to non-single-argument constructors is beyond the scope of these guidelines.

### Exception Specification

As of [May 2025](https://github.com/mongodb/mongo-cxx-driver/pull/1402#discussion_r2096510603), this codebase adopts "Policy Statement E - Minimal `noexcept` (MIN)" as described by ["Memorializing Principled-Design Policies for WG21" (P3005R0)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3005r0.pdf), with "No Lakos Rule on C Compatibility (NLC)" applied to functions where throwing an exception (from within the mongoc library as a callback) may result in undefined behavior. Only a constructor or UDCF whose nothrow guarantee is known to be queried for algorithm selection may be specified as `noexcept`, e.g.:

```cpp
if /* constexpr */ (is_nothrow<T>::value) {
  ... // Inherit the strong exception guarantee.
} else {
  // Manually implement the strong exception safety guarantee.
  try { ... } catch (...) { ... }
}
```

- Known cases currently include:
    - `std::move_if_noexcept<T>`.
    - `std::is_nothrow_move_constructible<T>`.
    - `std::is_nothrow_move_assignable<T>`.
    - `std::is_nothrow_copy_constructible<T>`.
    - `std::is_nothrow_copy_assignable<T>`.
- The following known cases are considered out-of-scope for these guidelines and the codebase:
    - `std::is_nothrow_destructible<T>` (assumed to be always true).
    - `std::is_nothrow_constructible<To, From>` where `From != To`.
    - `std::is_nothrow_convertible<From, To>` where `From != To`.
    - `std::is_nothrow_assignable<To, From>` where `From != To`.
    - `std::hash<T>::operator()`.

> [!NOTE]
> Support for non-throwing overloads of throwing functions (e.g. as implemented by `std::filesystem`) is currently out-of-scope for these guidelines and the codebase.

### Error Codes

- A component declared in `v<abi>::foo` which provides error codes should declare the error code enumeration in namespace `v<abi>::foo::errc` and the error category as `v<abi>::foo::error_category()`.

### Hidden Friends

- For any function or operator overload expected to be primarily invoked via ADL, prefer declaration and definition as a "hidden friend" (declared `friend` _and_ defined inline within the class definition).

### Parameter Passing

- Use `T` by default for parameters taking ownership of the argument's value (long-term storage).
    - This is preferable to combinatorial `T const&` + `T&&` overloads or the use of `view_or_value`-like helper types.
- Use `T const&` for parameters requiring short-term read-only access of the argument's value.
    - Use `T` instead of `T const&` for "cheap-to-copy" parameters (trivially copyable and `sizeof(T) <= 2u * sizeof(void*)`).
- Avoid using default arguments for parameters of exported ABI functions.

### Declaration Order

- Per [Howard E. Hinnant](https://howardhinnant.github.io/classdecl.html)'s advice, prioritize declarations which directly affect the ownership semantics of a class type before others.
- Per the [Rule of All or Nothing](https://www.fluentcpp.com/2019/04/23/the-rule-of-zero-zero-constructor-zero-calorie/), "As long as you can, stick to the Rule of Zero, but if you have to write at least one of the Big Five, default the rest."

```cpp
class Example : public Bases... {
private:
  // Data members.

public:
  ~Example();

  Example(Example&& other) noexcept;
  Example& operator=(Example&& other) noexcept;

  Example(Example const& other);
  Example& operator=(Example const& other);

  Example(); // When default-constructible.

  Example(Args...); // Other constructors.

  // Public API.

private:
  // Private members.
};
```

> [!IMPORTANT]
> Any non-defaulted special member function MUST be defined out-of-line as an exported ABI functions to support ABI compatibility.
