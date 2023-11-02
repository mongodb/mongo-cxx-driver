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
/// @file bsoncxx/v1/bsoncxx/config/postlude.hpp
///
/// Macro guard postlude header for bsoncxx.
///
/// Ensures any config macro defined in config headers do not leak their definition after inclusion.
///
/// Restores prior definitions of conflicting macros that are guarded by the @ref
/// bsoncxx/v1/bsoncxx/config/prelude.hpp "matching prelude header".
///
/// @warning Include of the postlude header must be preceeded by a matching include of the prelude
/// header.
///
/// @note Include of the postlude header may be omitted if restoring prior definitions of
/// conflicting macros is not necessary, e.g. in source (.cpp) files.
///

#if !defined(BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE)
#error "postlude header included without matching prelude header"
#endif
#undef BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE
