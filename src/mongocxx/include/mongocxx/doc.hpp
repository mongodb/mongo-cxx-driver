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

#if !defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
#error "This file is for documentation purposes only. It should not be included."
#endif  // !defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @file
/// For documentation purposes only.
///
/// @note This header is not includable!
///

///
/// @dir mongocxx/include/mongocxx
/// Provides headers declaring entities in @ref mongocxx
///

///
/// @namespace mongocxx
/// The top-level namespace within which all mongocxx library entities are declared.
///
/// In addition to declaring ABI namespaces (`v_noabi`, `v1`, etc.), this namespace also provides
/// "redeclarations" of entities within ABI namespaces. These redeclarations are intended to be the
/// preferred method for using bsoncxx library entities when ABI stability of the bsoncxx library is
/// NOT a requirement by the user. See [Root Namespace
/// Redeclarations](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/abi-versioning/#std-label-cpp-abi-root-namespace-redec).
///

///
/// @namespace mongocxx::events
/// Declares entities describing APM events.
///

///
/// @namespace mongocxx::gridfs
/// Declares entities related to GridFS.
///

///
/// @namespace mongocxx::model
/// Declares entities representing bulk write operations.
///

///
/// @namespace mongocxx::options
/// Declares entities representing options to use with various commands.
///

///
/// @namespace mongocxx::options::gridfs
/// Declares entities representing options specific to GridFS.
///

///
/// @namespace mongocxx::result
/// Declares entities representing the result of various commands.
///

///
/// @namespace mongocxx::result::gridfs
/// Declares entities representing the result of GridFS commands.
///

///
/// @namespace mongocxx::stdx
/// Redeclares @ref bsoncxx C++17 standard library polyfills in the @ref mongocxx library namespace
/// for internal use.
///
/// @deprecated Use @ref bsoncxx::stdx instead.
///
/// @warning These redeclarations are for internal use only! Users should reference entities
/// declared in @ref bsoncxx directly, not through @ref mongocxx.
///

///
/// @page topic-mongocxx The mongocxx Library
/// @brief Topics relating to the mongocxx library.
/// @li @subpage topic-mongocxx-examples
/// @li @subpage topic-mongocxx-about
///

///
/// @page topic-mongocxx-examples How-To Guides
/// @brief Examples of how to use the mongocxx library.
/// @li @subpage topic-mongocxx-examples-instance
/// @li @subpage topic-mongocxx-examples-logger
/// @li @subpage topic-mongocxx-examples-uri
/// @li @subpage topic-mongocxx-examples-clients
/// @li @subpage topic-mongocxx-examples-databases
/// @li @subpage topic-mongocxx-examples-collections
/// @li @subpage topic-mongocxx-examples-operation-exceptions
///

///
/// @page topic-mongocxx-examples-instance Instance
/// @brief How to use a MongoDB C++ Driver instance.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/instance.md
///

///
/// @page topic-mongocxx-examples-logger Logger
/// @brief How to use a custom logger with a MongoDB C++ Driver instance.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/logger.md
///

///
/// @page topic-mongocxx-examples-uri URI
/// @brief How to create and use URIs.
/// @tableofcontents
/// @see [Connection Strings (MongoDB
/// Manual)](https://www.mongodb.com/docs/manual/reference/connection-string/)
/// @include{doc} api/mongocxx/examples/uri.md
///

///
/// @page topic-mongocxx-examples-clients Clients
/// @brief How to use clients and client pools.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/clients.md
///

///
/// @page topic-mongocxx-examples-databases Databases
/// @brief How to obtain and use databases.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/databases.md
///

///
/// @page topic-mongocxx-examples-collections Collections
/// @brief How to obtain and use collections.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/collections.md
///

///
/// @page topic-mongocxx-examples-operation-exceptions Operation Exceptions
/// @brief How to handle exceptions thrown by database and collection operations.
/// @tableofcontents
/// @include{doc} api/mongocxx/examples/operation_exceptions.md
///

///
/// @page topic-mongocxx-about Explanation
/// @brief Design decisions, history, and other technical details about the mongocxx library.
///
