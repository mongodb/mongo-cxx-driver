// Copyright 2023 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

///
/// @file bsoncxx/v1/bsoncxx/config/prelude.hpp
///
/// Macro guard prelude header for bsoncxx.
///
/// Ensures any config macro defined in config headers do not conflict with existing definitions
/// prior to inclusion. To obtain config macros without macro guard behavior, include the
/// corresponding config header directly.
///
/// Prior definitions of conflicting macros are restored by including the @ref
/// bsoncxx/v1/bsoncxx/config/postlude.hpp "matching postlude header".
///
/// @warning Include of the prelude header twice in a row without a matching include of the postlude
/// header for the first include is forbidden (e.g. prelude -> prelude -> postlude -> postlude).
/// This is to enforce correct use of macro guard headers across transitive includes.
///
/// @warning Include of the postlude header must be preceeded by a matching include of the prelude
/// header.
///
/// @note Include of the matching postlude header may be omitted if restoring prior definitions of
/// conflicting macros is not necessary, e.g. in source (.cpp) files.
///
/// @par Includes
/// @li @ref bsoncxx-v1-bsoncxx-config-export-hpp "bsoncxx/v1/bsoncxx/config/export.hpp"
/// @li @ref bsoncxx-v1-bsoncxx-config-version-hpp "bsoncxx/v1/bsoncxx/config/version.hpp"
///

#if defined(BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE)
#error "prelude header included twice without matching postlude"
#endif

/// Used to validate correct use of macro guard headers.
#define BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE

// export.hpp (generated by CMake)
#pragma push_macro("BSONCXX_API_H")
#undef BSONCXX_API_H
#pragma push_macro("BSONCXX_API")
#undef BSONCXX_API
#pragma push_macro("BSONCXX_PRIVATE")
#undef BSONCXX_PRIVATE
#pragma push_macro("BSONCXX_DEPRECATED")
#undef BSONCXX_DEPRECATED
#pragma push_macro("BSONCXX_DEPRECATED_EXPORT")
#undef BSONCXX_DEPRECATED_EXPORT
#pragma push_macro("BSONCXX_DEPRECATED_NO_EXPORT")
#undef BSONCXX_DEPRECATED_NO_EXPORT

// version.hpp (generated by CMake)
#pragma push_macro("BSONCXX_VERSION_EXTRA")
#undef BSONCXX_VERSION_EXTRA
#pragma push_macro("BSONCXX_VERSION_MAJOR")
#undef BSONCXX_VERSION_MAJOR
#pragma push_macro("BSONCXX_VERSION_MINOR")
#undef BSONCXX_VERSION_MINOR
#pragma push_macro("BSONCXX_VERSION_PATCH")
#undef BSONCXX_VERSION_PATCH

#include <bsoncxx/v1/bsoncxx/config/export.hpp>
#include <bsoncxx/v1/bsoncxx/config/version.hpp>

// Doxygen does not account for generated header files.
// Document globally applicable macros and namespaces here.

// Doxygen complains if it cannot find at least one `#define` of a documented macro. Use dummy
// `#define` for macros in generated config headers within this block to satisfy Doxygen.
#if defined(BSONCXX_SATISFY_DOXYGEN_DEFINES)
/// @defgroup bsoncxx-v1-bsoncxx-config-export-hpp bsoncxx/v1/bsoncxx/config/export.hpp
/// A generated config header declaring macros that control symbol export behavior.
/// @sa bsoncxx/v1/bsoncxx/config/prelude.hpp
/// @{

/// Header guard macro generated by CMake.
#define BSONCXX_API_H
/// The declared entity is a member of the public API.
#define BSONCXX_API
/// The declared entity is excluded from the public API.
#define BSONCXX_PRIVATE
/// The declared entity is deprecated.
#define BSONCXX_DEPRECATED
/// The declared entity is a deprecated member of the public API.
#define BSONCXX_DEPRECATED_EXPORT
/// The declared entity is deprecated and excluded from the public API.
#define BSONCXX_DEPRECATED_NO_EXPORT

/// @}

/// @defgroup bsoncxx-v1-bsoncxx-config-version-hpp bsoncxx/v1/bsoncxx/config/version.hpp
/// A generated config header declaring macros describing the bsoncxx library API version number.
/// @sa bsoncxx/v1/bsoncxx/config/prelude.hpp
/// @{

/// The API major version number for this bsoncxx library.
#define BSONCXX_VERSION_MAJOR
/// The API minor version number for this bsoncxx library.
#define BSONCXX_VERSION_MINOR
/// The API patch version number for this bsoncxx library.
#define BSONCXX_VERSION_PATCH
/// Additional API version labels for this bsoncxx library.
#define BSONCXX_VERSION_EXTRA

/// @}

#endif  // defined(BSONCXX_SATISFY_DOXYGEN_DEFINES)

/// @namespace bsoncxx
/// The top-level namespace for bsoncxx library entities.

/// @namespace bsoncxx::v1
/// Declares entities compatible with stable ABI version 1.
