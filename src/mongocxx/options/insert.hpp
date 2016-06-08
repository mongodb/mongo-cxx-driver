// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB insert operation
///
class MONGOCXX_API insert {
   public:
    ///
    /// Sets the bypass_document_validation option.
    /// If true, allows the write to opt-out of document level validation.
    ///
    /// @note
    ///   On servers >= 3.2, the server applies validation by default. On servers < 3.2, this option
    ///   is ignored.
    ///
    /// @param bypass_document_validation
    ///   Whether or not to bypass document validation
    ///
    insert& bypass_document_validation(bool bypass_document_validation);

    ///
    /// Gets the current value of the bypass_document_validation option.
    ///
    /// @return The optional value of the bypass_document_validation option.
    ///
    const stdx::optional<bool>& bypass_document_validation() const;

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    insert& write_concern(class write_concern wc);

    ///
    /// The current write_concern for this operation.
    ///
    /// @return The current write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    const stdx::optional<class write_concern>& write_concern() const;

    ///
    /// @note: This applies only to insert_many and is ignored for insert_one.
    ///
    /// If true, when an insert fails, return without performing the remaining
    /// writes. If false, when a write fails, continue with the remaining
    /// writes, if any. Inserts can be performed in any order if this is false.
    /// Defaults to true.
    ///
    /// @param ordered
    ///   Whether or not the insert_many will be ordered.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.insert/
    ///
    insert& ordered(bool ordered);

    ///
    /// The current ordered value for this operation.
    ///
    /// @return The current ordered value.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.insert/
    ///
    const stdx::optional<bool>& ordered() const;

   private:
    stdx::optional<class write_concern> _write_concern;
    stdx::optional<bool> _ordered;
    stdx::optional<bool> _bypass_document_validation;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
