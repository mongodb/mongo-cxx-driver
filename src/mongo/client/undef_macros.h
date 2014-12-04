/** @file undef_macros.h remove mongo implementation macros after using */

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// If you define a new global un-prefixed macro, please add it here and in redef_macros

// #pragma once // this file is intended to be processed multiple times

#ifdef MONGO_MACROS_PUSHED

#if defined(_WIN32)
#pragma pop_macro("min")
#pragma pop_macro("max")
#pragma pop_macro("NOMINMAX")
#endif

// bson/inline_decls.h
#undef NOINLINE_DECL
#pragma pop_macro("NOINLINE_DECL")
#undef PACKED_DECL
#pragma pop_macro("PACKED_DECL")

// util/assert_util.h
#undef wassert
#pragma pop_macro("wassert")
#undef massert
#pragma pop_macro("massert")
#undef uassert
#pragma pop_macro("uassert")
#undef verify
#pragma pop_macro("verify")
#undef invariant
#pragma pop_macro("invariant")
#undef invariantOK
#pragma pop_macro("invariantOK")
#undef DESTRUCTOR_GUARD
#pragma pop_macro("DESTRUCTOR_GUARD")

#undef MONGO_MACROS_PUSHED
#endif
