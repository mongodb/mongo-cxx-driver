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

#include <bsoncxx/array/element-fwd.hpp>                 // IWYU pragma: export
#include <bsoncxx/array/value-fwd.hpp>                   // IWYU pragma: export
#include <bsoncxx/array/view-fwd.hpp>                    // IWYU pragma: export
#include <bsoncxx/builder/basic/array-fwd.hpp>           // IWYU pragma: export
#include <bsoncxx/builder/basic/document-fwd.hpp>        // IWYU pragma: export
#include <bsoncxx/builder/basic/sub_array-fwd.hpp>       // IWYU pragma: export
#include <bsoncxx/builder/basic/sub_binary-fwd.hpp>      // IWYU pragma: export
#include <bsoncxx/builder/basic/sub_document-fwd.hpp>    // IWYU pragma: export
#include <bsoncxx/builder/concatenate-fwd.hpp>           // IWYU pragma: export
#include <bsoncxx/builder/core-fwd.hpp>                  // IWYU pragma: export
#include <bsoncxx/builder/list-fwd.hpp>                  // IWYU pragma: export
#include <bsoncxx/builder/stream/array-fwd.hpp>          // IWYU pragma: export
#include <bsoncxx/builder/stream/array_context-fwd.hpp>  // IWYU pragma: export
#include <bsoncxx/builder/stream/closed_context-fwd.hpp> // IWYU pragma: export
#include <bsoncxx/builder/stream/document-fwd.hpp>       // IWYU pragma: export
#include <bsoncxx/builder/stream/helpers-fwd.hpp>        // IWYU pragma: export
#include <bsoncxx/builder/stream/key_context-fwd.hpp>    // IWYU pragma: export
#include <bsoncxx/builder/stream/single_context-fwd.hpp> // IWYU pragma: export
#include <bsoncxx/builder/stream/value_context-fwd.hpp>  // IWYU pragma: export
#include <bsoncxx/decimal128-fwd.hpp>                    // IWYU pragma: export
#include <bsoncxx/document/element-fwd.hpp>              // IWYU pragma: export
#include <bsoncxx/document/value-fwd.hpp>                // IWYU pragma: export
#include <bsoncxx/document/view-fwd.hpp>                 // IWYU pragma: export
#include <bsoncxx/exception/error_code-fwd.hpp>          // IWYU pragma: export
#include <bsoncxx/exception/exception-fwd.hpp>           // IWYU pragma: export
#include <bsoncxx/json-fwd.hpp>                          // IWYU pragma: export
#include <bsoncxx/oid-fwd.hpp>                           // IWYU pragma: export
#include <bsoncxx/string/view_or_value-fwd.hpp>          // IWYU pragma: export
#include <bsoncxx/types-fwd.hpp>                         // IWYU pragma: export
#include <bsoncxx/types/bson_value/value-fwd.hpp>        // IWYU pragma: export
#include <bsoncxx/types/bson_value/view-fwd.hpp>         // IWYU pragma: export
#include <bsoncxx/validate-fwd.hpp>                      // IWYU pragma: export
#include <bsoncxx/vector/accessor-fwd.hpp>               // IWYU pragma: export
#include <bsoncxx/vector/detail-fwd.hpp>                 // IWYU pragma: export
#include <bsoncxx/vector/elements-fwd.hpp>               // IWYU pragma: export
#include <bsoncxx/vector/formats-fwd.hpp>                // IWYU pragma: export
#include <bsoncxx/vector/iterators-fwd.hpp>              // IWYU pragma: export
#include <bsoncxx/view_or_value-fwd.hpp>                 // IWYU pragma: export

///
/// @file
/// Aggregate of all forward headers declaring entities in @ref bsoncxx::v_noabi.
///
/// @par Includes
/// - All header files under `bsoncxx/v_noabi/bsoncxx` whose filename ends with `-fwd.hpp`.
///
