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

#if !defined(BSONCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE)
#error "postlude header included without a matching prelude header"
#endif
#pragma pop_macro("BSONCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE")

///
/// @file
/// The bsoncxx v1 macro guard postlude header.
///
/// @warning For internal use only!
///
/// This header uses macro pragmas to guard macros defined by the bsoncxx library for internal use
/// by "popping" their prior definition onto the stack after use by bsoncxx headers.
///
/// @see
/// - @ref bsoncxx/v1/detail/prelude.hpp
///
