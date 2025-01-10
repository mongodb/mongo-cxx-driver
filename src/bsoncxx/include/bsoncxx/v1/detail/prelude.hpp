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

#pragma push_macro("BSONCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE")
#undef BSONCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE
#define BSONCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE

///
/// @file
/// The bsoncxx v1 macro guard prelude header.
///
/// @warning For internal use only!
///
/// This header uses macro pragmas to guard macros defined by the bsoncxx library for internal use
/// by "pushing" their prior definition onto the stack before use by bsoncxx headers.
///
/// @see
/// - @ref bsoncxx/v1/detail/postlude.hpp
///
