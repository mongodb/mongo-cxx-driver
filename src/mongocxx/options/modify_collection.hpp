// Copyright 2015 MongoDB Inc.
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

#include <chrono>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/validation_criteria.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB collMod command.
///
class MONGOCXX_API modify_collection {
   public:
    ///
    /// The index flag changes the expiration time of a TTL collection.
    ///
    /// @param index_spec the spec for an existing index in the collection
    /// @param seconds number of seconds to subtract from the current time
    ///
    /// @see https://docs.mongodb.org/manual/tutorial/expire-data/
    ///
    modify_collection& index(bsoncxx::document::view_or_value index_spec,
                             std::chrono::seconds seconds);

    ///
    /// When true, disables the power of 2 sizes allocation for the collection.
    ///
    /// @see: https://docs.mongodb.org/manual/reference/method/db.createCollection/
    ///
    /// @param no_padding
    ///   When true, disables power of 2 sizing for this collection.
    ///
    modify_collection& no_padding(bool no_padding);

    ///
    /// Specify validation criteria for this collection.
    ///
    /// @param validation
    ///   Validation criteria for this collection.
    ///
    /// @see https://docs.mongodb.org/manual/core/document-validation/
    ///
    modify_collection& validation_criteria(class validation_criteria validation);

    ///
    /// Return a bson document representing a collMod command with the given options
    /// set on this object.
    ///
    /// @return Options, as a document.
    ///
    bsoncxx::document::value to_document() const;

   private:
    stdx::optional<bsoncxx::document::view_or_value> _index;
    stdx::optional<bool> _no_padding;
    stdx::optional<class validation_criteria> _validation;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
