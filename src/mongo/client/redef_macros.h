/** @file redef_macros.h macros for mongo internals
    
    @see undef_macros.h undefines these after use to minimize name pollution.
*/

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

// If you define a new global un-prefixed macro, please add it here and in undef_macros

#define MONGO_MACROS_PUSHED 1

#if defined(_WIN32)
#pragma push_macro("min")
#undef min
#pragma push_macro("max")
#undef max
#pragma push_macro("NOMINMAX")
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

// bson/inline_decls.h
#pragma push_macro("NOINLINE_DECL")
#undef NOINLINE_DECL
#pragma push_macro("PACKED_DECL")
#undef PACKED_DECL

// util/assert_util.h
#pragma push_macro("verify")
#undef verify
#define verify(expression) MONGO_verify(expression)
#pragma push_macro("invariant")
#undef invariant
#define invariant MONGO_invariant
#pragma push_macro("invariantOK")
#undef invariantOK
#define invariantOK MONGO_invariantOK
#pragma push_macro("wassert")
#undef wassert
#define wassert MONGO_wassert
#pragma push_macro("massert")
#undef massert
#define massert MONGO_massert
#pragma push_macro("uassert")
#undef uassert
#define uassert MONGO_uassert
#pragma push_macro("DESTRUCTOR_GUARD")
#undef DESTRUCTOR_GUARD
#define DESTRUCTOR_GUARD MONGO_DESTRUCTOR_GUARD
