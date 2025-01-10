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

#include <bsoncxx/v1/detail/prelude.hpp>

// util.hpp
#pragma push_macro("BSONCXX_CONCAT")
#undef BSONCXX_CONCAT
#pragma push_macro("BSONCXX_CONCAT_IMPL")
#undef BSONCXX_CONCAT_IMPL
#pragma push_macro("BSONCXX_STRINGIFY")
#undef BSONCXX_STRINGIFY
#pragma push_macro("BSONCXX_STRINGIFY_IMPL")
#undef BSONCXX_STRINGIFY_IMPL
#pragma push_macro("BSONCXX_PRAGMA")
#undef BSONCXX_PRAGMA
#pragma push_macro("_bsoncxxPragma")
#undef _bsoncxxPragma
#pragma push_macro("BSONCXX_FORCE_SEMICOLON")
#undef BSONCXX_FORCE_SEMICOLON
#pragma push_macro("BSONCXX_RETURNS")
#undef BSONCXX_RETURNS
#pragma push_macro("bsoncxx_cxx14_constexpr")
#undef bsoncxx_cxx14_constexpr
#pragma push_macro("BSONCXX_DISABLE_WARNING")
#undef BSONCXX_DISABLE_WARNING
#pragma push_macro("BSONCXX_PUSH_WARNINGS")
#undef BSONCXX_PUSH_WARNINGS
#pragma push_macro("BSONCXX_POP_WARNINGS")
#undef BSONCXX_POP_WARNINGS
#pragma push_macro("_bsoncxxDisableWarningImpl_for_GCC")
#undef _bsoncxxDisableWarningImpl_for_GCC
#pragma push_macro("_bsoncxxDisableWarningImpl_for_Clang")
#undef _bsoncxxDisableWarningImpl_for_Clang
#pragma push_macro("_bsoncxxDisableWarningImpl_for_MSVC")
#undef _bsoncxxDisableWarningImpl_for_MSVC
#pragma push_macro("_bsoncxxDisableWarningImpl_for_GNU")
#undef _bsoncxxDisableWarningImpl_for_GNU
#pragma push_macro("BSONCXX_FWD")
#undef BSONCXX_FWD

// compiler.hpp
#pragma push_macro("BSONCXX_IF_MSVC")
#undef BSONCXX_IF_MSVC
#pragma push_macro("BSONCXX_IF_GCC")
#undef BSONCXX_IF_GCC
#pragma push_macro("BSONCXX_IF_CLANG")
#undef BSONCXX_IF_CLANG
#pragma push_macro("BSONCXX_IF_GNU_LIKE")
#undef BSONCXX_IF_GNU_LIKE

#include <bsoncxx/config/util.hpp>
//
#include <bsoncxx/config/compiler.hpp>
#include <bsoncxx/config/config.hpp>
#include <bsoncxx/config/export.hpp>
#include <bsoncxx/config/version.hpp>

#pragma push_macro("BSONCXX_UNREACHABLE")
#undef BSONCXX_UNREACHABLE
#define BSONCXX_UNREACHABLE std::abort()

///
/// @file
/// The bsoncxx v_noabi macro guard prelude header.
///
/// @warning For internal use only!
///
/// @par Includes
/// - @ref bsoncxx/v1/detail/prelude.hpp
/// - @ref bsoncxx/v_noabi/bsoncxx/config/compiler.hpp
/// - @ref bsoncxx/v_noabi/bsoncxx/config/config.hpp
/// - @ref bsoncxx/v_noabi/bsoncxx/config/export.hpp
/// - @ref bsoncxx/v_noabi/bsoncxx/config/util.hpp
/// - @ref bsoncxx/v_noabi/bsoncxx/config/version.hpp
///
/// @see
/// - @ref bsoncxx/v_noabi/bsoncxx/config/postlude.hpp
///
