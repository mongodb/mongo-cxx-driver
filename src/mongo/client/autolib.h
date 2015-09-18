/*    Copyright 2014 MongoDB Inc.
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

#pragma once

#if defined(_MSC_VER) && !defined(LIBMONGOCLIENT_NO_AUTOLIB)
#
#pragma push_macro("LIBMONGOCLIENT_LIB_PREFIX")
#undef LIBMONGOCLIENT_LIB_PREFIX
#
#pragma push_macro("LIBMONGOCLIENT_ABI_ADORNMENT")
#undef LIBMONGOCLIENT_ABI_ADORNMENT
#
#pragma push_macro("LIBMONGOCLIENT_LIB_SUFFIX")
#undef LIBMONGOCLIENT_LIB_SUFFIX
#
#if defined(STATIC_LIBMONGOCLIENT)
#// Our static libraries start with an extra 'lib' to separate from DLLs.
#define LIBMONGOCLIENT_LIB_PREFIX "lib"
#if defined(_DEBUG)
#if defined(_DLL)
#// This is a static client lib linking to the debug DLL runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT "-gd"
#else
#// This is a static client lib linking to the debug static runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT "-sgd"
#endif
#else
#if defined(_DLL)
#// This is a static client lib linking to the retail DLL runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT ""
#else
#// This is a static client lib linking to the retail static runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT "-s"
#endif
#endif
#else
#// Our DLL doesn't have a 'lib' prefix, unlike our static lib
#define LIBMONGOCLIENT_LIB_PREFIX ""
#if defined(_DEBUG)
#// This is a DLL libmongoclient linking to the debug DLL runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT "-gd"
#else
#// This is a DLL libmongoclient linking to the retail DLL runtime
#define LIBMONGOCLIENT_ABI_ADORNMENT ""
#endif
#endif
#
#// We always link to either the static library, or the stub for DLLs.
#define LIBMONGOCLIENT_LIB_SUFFIX ".lib"
#
#// Tack together the pieces in the right order
#pragma comment(lib,                      \
                LIBMONGOCLIENT_LIB_PREFIX \
                "mongoclient" LIBMONGOCLIENT_ABI_ADORNMENT LIBMONGOCLIENT_LIB_SUFFIX)
#
#pragma pop_macro("LIBMONGOCLIENT_LIB_PREFIX")
#pragma pop_macro("LIBMONGOCLIENT_ABI_ADORNMENT")
#pragma pop_macro("LIBMONGOCLIENT_LIB_SUFFIX")
#
#endif
