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

#include <bsoncxx/builder/concatenate-fwd.hpp>

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {

///
/// Container to concatenate a document.
///
/// Use this with a document builder to merge an existing document's fields with that of the
/// document being built.
///
struct concatenate_doc {
    document::view_or_value doc;

    ///
    /// Conversion operator that provides a view of the wrapped concatenate
    /// document.
    ///
    /// @return A view of the wrapped concatenate document.
    ///
    operator document::view() const {
        return doc;
    }

    ///
    /// Accessor that provides a view of the wrapped concatenate
    /// document.
    ///
    /// @return A view of the wrapped concatenate document.
    ///
    document::view view() const {
        return doc;
    }
};

///
/// Container to concatenate an array.
///
/// Use this with an array builder to merge an existing array's fields with that of the array being
/// built.
///
struct concatenate_array {
    array::view_or_value array;

    ///
    /// Conversion operator that provides a view of the wrapped concatenate
    /// array.
    ///
    /// @return A view of the wrapped concatenate array.
    ///
    operator array::view() const {
        return array;
    }

    ///
    /// Accessor that provides a view of the wrapped concatenate
    /// array.
    ///
    /// @return A view of the wrapped concatenate array.
    ///
    array::view view() const {
        return array;
    }
};

///
/// Helper method to concatenate a document.
///
/// Use this with a document builder to merge an existing document's fields with that of the
/// document being built.
///
/// @param doc The document to concatenate.
///
/// @return concatenate_doc A concatenating struct.
///
/// @see
/// - @ref bsoncxx::v_noabi::builder::concatenate_doc
///
inline concatenate_doc concatenate(document::view_or_value doc) {
    return {std::move(doc)};
}

///
/// Helper method to concatenate an array.
///
/// Use this with an array builder to merge an existing array's fields with that of the array being
/// built.
///
/// @param array The array to concatenate.
///
/// @return concatenate_array A concatenating struct.
///
/// @see
/// - @ref bsoncxx::v_noabi::builder::concatenate_array
///
inline concatenate_array concatenate(array::view_or_value array) {
    return {std::move(array)};
}

} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace builder {

using ::bsoncxx::v_noabi::builder::concatenate;

} // namespace builder
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides concatenators for use with "streaming" BSON builder syntax.
///
