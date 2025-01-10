// Copyright 2009-present MongoDB, Inc.
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

#pragma push_macro("MONGOCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE")
#undef MONGOCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE
#define MONGOCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE

// mongocxx/v1/config/export.hpp (generated by CMake)
#pragma push_macro("MONGOCXX_ABI_EXPORT_H")
#if !defined(MONGOCXX_ABI_EXPORT_H)
#pragma push_macro("MONGOCXX_ABI_EXPORT")
#undef MONGOCXX_ABI_EXPORT
#pragma push_macro("MONGOCXX_ABI_NO_EXPORT")
#undef MONGOCXX_ABI_NO_EXPORT
#pragma push_macro("MONGOCXX_ABI_CDECL")
#undef MONGOCXX_ABI_CDECL
#pragma push_macro("MONGOCXX_ABI_EXPORT_CDECL")
#undef MONGOCXX_ABI_EXPORT_CDECL
#pragma push_macro("MONGOCXX_DEPRECATED")
#undef MONGOCXX_DEPRECATED
#endif

// mongocxx/v1/config/version.hpp (generated by CMake)
#pragma push_macro("MONGOCXX_V1_CONFIG_VERSION_HPP")
#if !defined(MONGOCXX_V1_CONFIG_VERSION_HPP)
#pragma push_macro("MONGOCXX_VERSION_STRING")
#undef MONGOCXX_VERSION_STRING
#pragma push_macro("MONGOCXX_VERSION_EXTRA")
#undef MONGOCXX_VERSION_EXTRA
#pragma push_macro("MONGOCXX_VERSION_MAJOR")
#undef MONGOCXX_VERSION_MAJOR
#pragma push_macro("MONGOCXX_VERSION_MINOR")
#undef MONGOCXX_VERSION_MINOR
#pragma push_macro("MONGOCXX_VERSION_PATCH")
#undef MONGOCXX_VERSION_PATCH
#endif

// mongocxx/v1/detail/macros.hpp
#pragma push_macro("MONGOCXX_V1_DETAIL_MACROS_HPP")
#if !defined(MONGOCXX_V1_DETAIL_MACROS_HPP)
#pragma push_macro("MONGOCXX_UNREACHABLE")
#undef MONGOCXX_UNREACHABLE
#endif

///
/// @file
/// The mongocxx v1 macro guard prelude header.
///
/// @warning For internal use only!
///
/// This header uses macro pragmas to guard macros defined by the mongocxx library for internal use
/// by "pushing" their prior definition onto the stack before use by mongocxx headers.
///
/// @see
/// - @ref mongocxx/v1/detail/postlude.hpp
///
