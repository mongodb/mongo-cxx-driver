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

#pragma once

#include <bsoncxx/array/element-fwd.hpp>
#include <bsoncxx/array/value-fwd.hpp>
#include <bsoncxx/array/view-fwd.hpp>
#include <bsoncxx/builder/basic/array-fwd.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/sub_array-fwd.hpp>
#include <bsoncxx/builder/basic/sub_document-fwd.hpp>
#include <bsoncxx/builder/concatenate-fwd.hpp>
#include <bsoncxx/builder/core-fwd.hpp>
#include <bsoncxx/builder/list-fwd.hpp>
#include <bsoncxx/builder/stream/array-fwd.hpp>
#include <bsoncxx/builder/stream/array_context-fwd.hpp>
#include <bsoncxx/builder/stream/closed_context-fwd.hpp>
#include <bsoncxx/builder/stream/document-fwd.hpp>
#include <bsoncxx/builder/stream/helpers-fwd.hpp>
#include <bsoncxx/builder/stream/key_context-fwd.hpp>
#include <bsoncxx/builder/stream/single_context-fwd.hpp>
#include <bsoncxx/builder/stream/value_context-fwd.hpp>
#include <bsoncxx/decimal128-fwd.hpp>
#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/document/value-fwd.hpp>
#include <bsoncxx/document/view-fwd.hpp>
#include <bsoncxx/exception/error_code-fwd.hpp>
#include <bsoncxx/exception/exception-fwd.hpp>
#include <bsoncxx/json-fwd.hpp>
#include <bsoncxx/oid-fwd.hpp>
#include <bsoncxx/string/view_or_value-fwd.hpp>
#include <bsoncxx/types-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>
#include <bsoncxx/validate-fwd.hpp>
#include <bsoncxx/view_or_value-fwd.hpp>

///
/// @file
/// Aggregate of all forward headers declaring entities in @ref bsoncxx::v_noabi.
///
/// @par Includes
/// - All header files under `bsoncxx/v_noabi/bsoncxx` whose filename ends with `-fwd.hpp`.
///

///
/// @dir bsoncxx/v_noabi
/// The directory relative to which headers declaring entities in @ref bsoncxx::v_noabi are
/// typically included.
///
/// For backward compatibility, unstable ABI headers may be included using the syntax
/// `<bsoncxx/foo.hpp>`, which is equivalent to `<bsoncxx/v_noabi/bsoncxx/foo.hpp>`.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx
/// Provides headers declaring entities in @ref bsoncxx::v_noabi.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/array
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::array.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder/basic
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder::basic.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder/stream
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder::stream.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/config
/// Provides headers related to bsoncxx library configuration.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/document
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::document.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/enums
/// Provides X macro headers over BSON types and binary subtypes.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/exception
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::exception.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/stdx
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::stdx.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/string
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::string.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/types
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::types.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/types/bson_value
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::types::bson_value.
///

///
/// @namespace bsoncxx::v_noabi
/// Declares entities whose ABI stability is NOT guaranteed.
///

///
/// @namespace bsoncxx::v_noabi::array
/// @copydoc bsoncxx::array
///

///
/// @namespace bsoncxx::v_noabi::builder
/// @copydoc bsoncxx::builder
///

///
/// @namespace bsoncxx::v_noabi::builder::basic
/// @copydoc bsoncxx::builder::basic
///

///
/// @namespace bsoncxx::v_noabi::builder::stream
/// @copydoc bsoncxx::builder::stream
///

///
/// @namespace bsoncxx::v_noabi::document
/// @copydoc bsoncxx::document
///

///
/// @namespace bsoncxx::v_noabi::stdx
/// @copydoc bsoncxx::stdx
///

///
/// @namespace bsoncxx::v_noabi::string
/// @copydoc bsoncxx::string
///

///
/// @namespace bsoncxx::v_noabi::types
/// @copydoc bsoncxx::types
///
/// @see
/// - @ref bsoncxx::v_noabi::types::bson_value
///

///
/// @namespace bsoncxx::v_noabi::types::bson_value
/// Declares entities representing any BSON value type.
///
