/** @file dbclient.h

    Include this file when writing client C++ applications, to get access to the
    mongod C++ driver.
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

#pragma once

#ifdef MONGO_EXPOSE_MACROS
#error dbclient.h is for C++ driver consumer use only
#endif

// Consumers of the MongoDB C++ client library must define STATIC_LIBMONGOCLIENT when including
// this header if they intend to link against the static version of the library. This is best
// handled by adding STATIC_LIBMONGOCLIENT to the list of definitions passed on each compile
// invocation.
#ifndef STATIC_LIBMONGOCLIENT
#if defined(_WIN32) && !defined(_DLL)
#error "The DLL build of libmongoclient requires consuming code to be built with /MD or /MDd"
#endif
#endif

#if defined(_MSC_VER)
#pragma warning(push)
// Don't emit deprecation warnings
#pragma warning(disable : 4996)
#if defined(_DLL)
// Don't spam DLL consumers with warnings about STL symbol exports
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif
#endif

#if defined(_WIN32) && !defined(_WINSOCK2API_)
#error "You must include the windows and windows sockets headers before dbclient.h"
#endif

#include "mongo/config.h"

#include "mongo/client/redef_macros.h"

#include "mongo/client/autolib.h"

#include "mongo/client/dbclient_rs.h"
#include "mongo/client/dbclientcursor.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/client/gridfs.h"
#include "mongo/client/init.h"
#include "mongo/client/options.h"
#include "mongo/client/sasl_client_authenticate.h"
#include "mongo/geo/interface.h"
#include "mongo/version.h"

#include "mongo/client/undef_macros.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
