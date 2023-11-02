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
/// @file mongocxx/v1/mongocxx/config/postlude.hpp
///
/// Macro guard postlude header for mongocxx.
///
/// Ensures any config macro defined in config headers do not leak their definition after inclusion.
///
/// Restores prior definitions of conflicting macros that are guarded by the @ref
/// mongocxx/v1/mongocxx/config/prelude.hpp "matching prelude header".
///
/// @warning Include of the postlude header must be preceeded by a matching include of the prelude
/// header.
///
/// @note Include of the postlude header may be omitted if restoring prior definitions of
/// conflicting macros is not necessary, e.g. in source (.cpp) files.
///
/// @par Includes
/// @li bsoncxx/v1/bsoncxx/config/postlude.hpp.
///

// compiler.hpp
#undef MONGOCXX_INLINE
#pragma pop_macro("MONGOCXX_INLINE")
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#undef MONGOCXX_CALL
#pragma pop_macro("MONGOCXX_CALL")

// export.hpp (generated by CMake)
#undef MONGOCXX_API_H
#pragma pop_macro("MONGOCXX_API_H")
#undef MONGOCXX_API
#pragma pop_macro("MONGOCXX_API")
#undef MONGOCXX_PRIVATE
#pragma pop_macro("MONGOCXX_PRIVATE")
#undef MONGOCXX_DEPRECATED
#pragma pop_macro("MONGOCXX_DEPRECATED")
#undef MONGOCXX_DEPRECATED_EXPORT
#pragma pop_macro("MONGOCXX_DEPRECATED_EXPORT")
#undef MONGOCXX_DEPRECATED_NO_EXPORT
#pragma pop_macro("MONGOCXX_DEPRECATED_NO_EXPORT")

// version.hpp (generated by CMake)
#undef MONGOCXX_VERSION_STRING
#pragma pop_macro("MONGOCXX_VERSION_STRING")
#undef MONGOCXX_VERSION_EXTRA
#pragma pop_macro("MONGOCXX_VERSION_EXTRA")
#undef MONGOCXX_VERSION_MAJOR
#pragma pop_macro("MONGOCXX_VERSION_MAJOR")
#undef MONGOCXX_VERSION_MINOR
#pragma pop_macro("MONGOCXX_VERSION_MINOR")
#undef MONGOCXX_VERSION_PATCH
#pragma pop_macro("MONGOCXX_VERSION_PATCH")

#if !defined(MONGOCXX_V1_INSIDE_MACRO_GUARD_SCOPE)
#error "postlude header included without matching prelude header"
#endif
#undef MONGOCXX_V1_INSIDE_MACRO_GUARD_SCOPE

#include <bsoncxx/v1/bsoncxx/config/postlude.hpp>
