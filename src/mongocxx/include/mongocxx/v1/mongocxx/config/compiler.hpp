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
/// @file mongocxx/v1/mongocxx/config/compiler.hpp
///
/// Declares macros that control compiler-specific behavior.
///
/// @par Platform-Specific Behavior
/// Including this header disables MSVC warnings C4251 and C4275 via `#pragma warning(push)`. This
/// change is reverted by the postlude header via `#pragma warning(pop)`.
///

///
/// The declared entity is a member of the public API but is excluded from ABI stability guarantees.
///
#define MONGOCXX_INLINE inline MONGOCXX_PRIVATE

///
/// @def MONGOCXX_CALL
/// Ensure the declared entity uses naming and calling conventions for C and C++.
///
#if defined(_MSC_VER)
// Disable MSVC warnings that cause a lot of noise related to DLL visibility
// for types that we don't control (like std::unique_ptr).
#pragma warning(push)
#pragma warning(disable : 4251 4275)
#define MONGOCXX_CALL __cdecl
#else
#define MONGOCXX_CALL
#endif
