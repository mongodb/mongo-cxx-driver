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

#include <cstdint>

#include <mongo/bson/document.hpp>
#include <mongo/driver/options/find_one_and_modify.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB find_and_modify update operation.
///
class LIBMONGOCXX_API find_one_and_update {

   public:

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time_ms
    ///   The max amount of time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    void max_time_ms(std::int64_t max_time_ms);

    ///
    /// The current max_time_ms setting.
    ///
    /// @return the current max allowed running time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const stdx::optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets a projection which limits the fields to return.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    void projection(bson::document::view projection);

    ///
    /// Gets the current projection.
    ///
    /// @return The current projection.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    const stdx::optional<bson::document::view>& projection() const;

    ///
    /// Sets the state of the document to be returned by the operation, either the
    /// original document or its newly-update form.
    ///
    /// @param return_document
    ///   State of document to return.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    /// @see mongo::driver::options::return_document
    ///
    void return_document(return_document return_document);

    ///
    /// Gets the state of the document to be returned by the operation.
    ///
    /// @return State of document to return.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    /// @see mongo::driver::options::return_document
    ///
    const stdx::optional<enum return_document>& return_document() const;

    ///
    /// Sets the order by which to search the collection for a matching document.
    ///
    /// @warning This can influence which document the operation modifies if the provided filter
    /// selects multiple documents.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    ///
    void sort(bson::document::view ordering);

    ///
    /// Gets the current sort ordering.
    ///
    /// @return The current sort ordering.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    ///
    const stdx::optional<bson::document::view>& sort() const;

    ///
    /// Sets the upsert flag on the operation. When @c true, the operation creates a new document if
    /// no document matches the filter. When @c false, this operation will do nothing if there are
    /// no matching documents. The server-side default is false.
    ///
    /// @param upsert
    ///   Whether or not to perform an upsert.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    ///
    void upsert(bool upsert);

    ///
    /// Gets the current upsert setting.
    ///
    /// @return The current upsert setting.
    ///
    /// @see http://docs.mongodb.org/manual/reference/command/findAndModify/
    ///
    const stdx::optional<bool>& upsert() const;

   private:
    stdx::optional<std::int64_t> _max_time_ms;
    stdx::optional<bson::document::view> _projection;
    stdx::optional<enum return_document> _return_document;
    stdx::optional<bson::document::view> _ordering;
    stdx::optional<bool> _upsert;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
