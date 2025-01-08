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

#if !defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
#error "This file is for documentation purposes only. It should not be included."
#endif // !defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @file
/// For documentation purposes only.
///
/// @note This header is not includable!
///

///
/// @dir include/bsoncxx
/// Provides headers declaring entities in @ref bsoncxx
///

///
/// @dir include/bsoncxx/docs
/// For documentation purposes only.
///

///
/// @namespace bsoncxx
/// The top-level namespace within which all bsoncxx library entities are declared.
///
/// In addition to declaring ABI namespaces (`v_noabi`, `v1`, etc.), this namespace also provides
/// "redeclarations" of entities within ABI namespaces. These redeclarations are intended to be the
/// preferred method for using bsoncxx library entities when ABI stability of the bsoncxx library is
/// NOT a requirement by the user. See [Root Namespace
/// Redeclarations](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/abi-versioning/#std-label-cpp-abi-root-namespace-redec).
///

///
/// @namespace bsoncxx::array
/// Declares entities representing a BSON array.
///

///
/// @namespace bsoncxx::builder
/// Declares entities used to build BSON documents.
///

///
/// @namespace bsoncxx::builder::basic
/// Declares entities used with "basic" BSON builder syntax.
///

///
/// @namespace bsoncxx::builder::stream
/// Declares entities used with "streaming" BSON builder syntax.
///

///
/// @namespace bsoncxx::document
/// Declares entities representing a BSON document.
///

///
/// @namespace bsoncxx::stdx
/// Declares C++17 standard library polyfills.
///

///
/// @namespace bsoncxx::string
/// Declares entities related to handling string types.
///

///
/// @namespace bsoncxx::types
/// Declares entities representing BSON value types.
///

///
/// @namespace bsoncxx::types::bson_value
/// Declares entities representing any BSON value type.
///

///
/// @page topic-bsoncxx The bsoncxx Library
/// @brief Topics relating to the bsoncxx library.
/// @li @subpage topic-bsoncxx-examples
/// @li @subpage topic-bsoncxx-about
///

///
/// @page topic-bsoncxx-examples How-To Guides
/// @brief Examples of how to use the bsoncxx library.
/// @li @subpage topic-bsoncxx-examples-bson-documents
/// @li @subpage topic-bsoncxx-examples-bson-errors
/// @li @subpage topic-bsoncxx-examples-decimal128
/// @li @subpage topic-bsoncxx-examples-oid
/// @li @subpage topic-bsoncxx-examples-validation
///

///
/// @page topic-bsoncxx-examples-bson-documents BSON Documents
/// @brief How to create and use BSON documents.
/// @tableofcontents
/// @include{doc} api/bsoncxx/examples/bson_documents/create_doc.md
/// @include{doc} api/bsoncxx/examples/bson_documents/create_array.md
/// @include{doc} api/bsoncxx/examples/bson_documents/access_doc.md
/// @include{doc} api/bsoncxx/examples/bson_documents/access_array.md
/// @include{doc} api/bsoncxx/examples/bson_documents/elements.md
/// @include{doc} api/bsoncxx/examples/bson_documents/values.md
/// @include{doc} api/bsoncxx/examples/bson_documents/json.md
///

///
/// @page topic-bsoncxx-examples-bson-errors BSON Error Handling
/// @brief How to handle BSON document failures, errors, and exceptions.
/// @tableofcontents
/// @include{doc} api/bsoncxx/examples/bson_errors.md
///

///
/// @page topic-bsoncxx-examples-decimal128 Decimal128
/// @brief How to use Decimal128 interfaces.
/// @tableofcontents
/// @see @parblock
/// @li [BSON Decimal128 Type Handling in Drivers (MongoDB
/// Specifications)](https://specifications.readthedocs.io/en/latest/bson-decimal128/decimal128/)
/// @li [Conversions (General Decimal Arithmetic
/// Specification)](https://speleotrove.com/decimal/daconvs.html)
/// @endparblock
/// @include{doc} api/bsoncxx/examples/decimal128.md
///

///
/// @page topic-bsoncxx-examples-oid ObjectID
/// @brief How to use ObjectID (OID) interfaces.
/// @tableofcontents
/// @include{doc} api/bsoncxx/examples/oid.md
///

///
/// @page topic-bsoncxx-examples-validation Validation
/// @brief How to use BSON document validation interfaces.
/// @tableofcontents
/// @include{doc} api/bsoncxx/examples/validation.md
///

///
/// @page topic-bsoncxx-about Explanation
/// @brief Design decisions, history, and other technical details about the bsoncxx library.
///
