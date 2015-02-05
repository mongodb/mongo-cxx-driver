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

#include <mongo/driver/config/prelude.hpp>

#include <mongo/bson/document.hpp>
#include <mongo/driver/write_concern.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB update operation.
///
class LIBMONGOCXX_API update {

   public:

    ///
    /// Sets the upsert option.
    ///
    /// By default, if no document matches the filter, the update operation does nothing.
    /// However, by specifying upsert as @c true, this operation either updates matching documents
    /// or inserts a new document using the update specification if no matching document exists.
    ///
    /// @param upsert
    ///   If set to @c true, creates a new document when no document matches the query criteria.
    ///   The server-side default is @c false, which does not insert a new document if a match
    ///   is not found.
    ///
    void upsert(bool upsert);

    ///
    /// Gets the current value of the upsert option.
    ///
    /// @return The optional value of the upsert option.
    ///
    const stdx::optional<bool>& upsert() const;

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    void write_concern(class write_concern wc);

    ///
    /// The current write_concern for this operation.
    ///
    /// @return
    ///   The current write_concern
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    const stdx::optional<class write_concern>& write_concern() const;

   private:
    stdx::optional<bool> _upsert;
    stdx::optional<class write_concern> _write_concern;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
