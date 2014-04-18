/*    Copyright 2013 10gen Inc.
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

#include "mongo/platform/compiler.h"

#if defined(_WIN32) && (defined(__i386) || defined(_M_IX86))
// For 32-bit windows, we need to define API calls as __cdecl. For 64 bit windows
// the calling convention is always __fastcall. For non-windows, we don't care.
#if defined _MSC_VER
#define MONGO_CLIENT_FUNC __cdecl
#else
#error "Don't know how to declare cdecl on this system"
#endif
#else
// On anything other than windows 32-bit , we don't need to bother specifying.
#define MONGO_CLIENT_FUNC
#endif

/**
 * Definition of macros used to label the mongo client api.
 *
 * If a type, free function or global variable is part of the client api, it must be labeled.
 *
 * To label a type, place the MONGO_CLIENT_API macro after the struct, class or enum keyword.
 * Example:
 *   class MONGO_CLIENT_API DBClientInterface { ... };
 *
 * To label a function, place the label on the declaration before the return type.  You
 * do NOT need to label the methods of exported classes.
 * Example:
 *   MONGO_CLIENT_API Status myFreeFunction(int arg1);
 *
 * To label a global variable, place the label on the declaration, before the type and
 * after the "extern" keyword.
 * Example:
 *   extern MONGO_CLIENT_API int myGlobalVariable;
 *
 * dbclient.h sets the LIBMONGOCLIENT_CONSUMER macro, so all clients will convert the
 * MONGO_CLIENT_API macro to the the import form, while the library code will convert it to the
 * export form.
 */

#if defined(LIBMONGOCLIENT_CONSUMER) && !defined(LIBMONGOCLIENT_BUILDING)
#define MONGO_CLIENT_API MONGO_COMPILER_API_IMPORT
#elif !defined(LIBMONGOCLIENT_CONSUMER) && defined(LIBMONGOCLIENT_BUILDING)
#define MONGO_CLIENT_API MONGO_COMPILER_API_EXPORT
#elif !defined(LIBMONGOCLIENT_CONSUMER) && !defined(LIBMONGOCLIENT_BUILDING)
#define MONGO_CLIENT_API
#else
#error "Must not define both LIBMONGOCLIENT_BUILDING and LIBMONGOCLIENT_CONSUMER"
#endif


