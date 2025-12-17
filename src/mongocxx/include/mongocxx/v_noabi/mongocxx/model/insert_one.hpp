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

#include <mongocxx/model/insert_one-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

///
/// A MongoDB insert operation that creates a single document.
///
class insert_one {
   public:
    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ insert_one(v1::bulk_write::insert_one op)
        : _document{bsoncxx::v_noabi::from_v1(std::move(op.value))} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::bulk_write::insert_one() const {
        return bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_document.view())};
    }

    ///
    /// Constructs an insert operation that will create a single document.
    ///
    /// @param document
    ///   The document to insert.
    ///
    insert_one(bsoncxx::v_noabi::document::view_or_value document) : _document{std::move(document)} {}

    ///
    /// Gets the document to be inserted.
    ///
    /// @return The document to be inserted.
    ///
    bsoncxx::v_noabi::document::view_or_value const& document() const {
        return _document;
    }

   private:
    bsoncxx::v_noabi::document::view_or_value _document;
};

} // namespace model
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::model::insert_one from_v1(v1::bulk_write::insert_one v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write::insert_one to_v1(v_noabi::model::insert_one const& v) {
    return v1::bulk_write::insert_one{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::insert_one.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
