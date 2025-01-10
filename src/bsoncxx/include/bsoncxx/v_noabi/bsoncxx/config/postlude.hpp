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

// prelude.hpp
#undef BSONCXX_UNREACHABLE
#pragma pop_macro("BSONCXX_UNREACHABLE")

#pragma pop_macro("bsoncxx_cxx14_constexpr")
#pragma pop_macro("BSONCXX_RETURNS")

// util.hpp
#pragma pop_macro("BSONCXX_PUSH_WARNINGS")
#pragma pop_macro("BSONCXX_POP_WARNINGS")
#pragma pop_macro("BSONCXX_DISABLE_WARNING")

#pragma pop_macro("_bsoncxxDisableWarningImpl_for_MSVC")
#pragma pop_macro("_bsoncxxDisableWarningImpl_for_GCC")
#pragma pop_macro("_bsoncxxDisableWarningImpl_for_GNU")
#pragma pop_macro("_bsoncxxDisableWarningImpl_for_Clang")

#pragma pop_macro("BSONCXX_CONCAT")
#pragma pop_macro("BSONCXX_CONCAT_IMPL")

#pragma pop_macro("BSONCXX_PRAGMA")
#pragma pop_macro("_bsoncxxPragma")
#pragma pop_macro("BSONCXX_STRINGIFY_IMPL")
#pragma pop_macro("BSONCXX_STRINGIFY")
#pragma pop_macro("BSONCXX_FORCE_SEMICOLON")

#pragma pop_macro("BSONCXX_IF_MSVC")
#pragma pop_macro("BSONCXX_IF_GCC")
#pragma pop_macro("BSONCXX_IF_CLANG")
#pragma pop_macro("BSONCXX_IF_GNU_LIKE")

#pragma pop_macro("BSONCXX_FWD")

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// The bsoncxx v_noabi macro guard postlude header.
///
/// @warning For internal use only!
///
/// @par Includes
/// - @ref bsoncxx/v1/detail/postlude.hpp
///
/// @see
/// - @ref bsoncxx/v1/detail/prelude.hpp
///
