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

namespace mongo {
namespace driver {
namespace model {

///
/// Class representing a MongoDB insert operation that creates a single document.
///
class LIBMONGOCXX_API insert_one {

   public:

    ///
    /// Constructs an insert operation that will create a single document.
    ///
    /// @param document
    ///   The document to insert.
    ///
    insert_one(bson::document::view document);

    ///
    /// Gets the document to be inserted.
    ///
    /// @return The document to be inserted.
    ///
    const bson::document::view& document() const;

   private:
    // TODO: should this be a value?
    // Required
    bson::document::view _document;

};

}  // namespace model
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
