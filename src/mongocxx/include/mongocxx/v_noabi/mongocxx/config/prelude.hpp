// Copyright 2014 MongoDB Inc.
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

// compiler.hpp
#pragma push_macro("MONGOCXX_INLINE")
#undef MONGOCXX_INLINE
#pragma push_macro("MONGOCXX_CALL")
#undef MONGOCXX_CALL

// config.hpp (generated by CMake)
#pragma push_macro("MONGOCXX_INLINE_NAMESPACE_BEGIN")
#undef MONGOCXX_INLINE_NAMESPACE_BEGIN
#pragma push_macro("MONGOCXX_INLINE_NAMESPACE_END")
#undef MONGOCXX_INLINE_NAMESPACE_END

// version.hpp (generated by CMake)
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

// export.hpp (generated by CMake)
#pragma push_macro("MONGOCXX_API_H")
#undef MONGOCXX_API_H
#pragma push_macro("MONGOCXX_API")
#undef MONGOCXX_API
#pragma push_macro("MONGOCXX_PRIVATE")
#undef MONGOCXX_PRIVATE
#pragma push_macro("MONGOCXX_DEPRECATED")
#undef MONGOCXX_DEPRECATED
#pragma push_macro("MONGOCXX_DEPRECATED_EXPORT")
#undef MONGOCXX_DEPRECATED_EXPORT
#pragma push_macro("MONGOCXX_DEPRECATED_NO_EXPORT")
#undef MONGOCXX_DEPRECATED_NO_EXPORT
#pragma push_macro("DEFINE_NO_DEPRECATED")
#undef DEFINE_NO_DEPRECATED
#pragma push_macro("MONGOCXX_NO_DEPRECATED")
#undef MONGOCXX_NO_DEPRECATED

#include <mongocxx/config/compiler.hpp>
#include <mongocxx/config/config.hpp>
#include <mongocxx/config/export.hpp>
#include <mongocxx/config/version.hpp>

// TODO: Find a way to DRY this with BSONCXX_UNREACHABLE
#pragma push_macro("MONGOCXX_UNREACHABLE")
#undef MONGOCXX_UNREACHABLE
#define MONGOCXX_UNREACHABLE std::abort()
