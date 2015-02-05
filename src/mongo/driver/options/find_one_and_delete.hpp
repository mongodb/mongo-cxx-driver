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
#include <mongo/driver/write_concern.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB find_and_modify delete operation
///
class LIBMONGOCXX_API find_one_and_delete {

   public:

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time_ms
    ///   The max amount of running time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    void max_time_ms(std::int64_t max_time_ms);

    ///
    /// The current max_time_ms setting.
    ///
    /// @return the current max time (in milliseconds).
    ///
    /// @see http://docs.mongodb.org/manual/reference/operator/meta/maxTimeMS
    ///
    const stdx::optional<std::int64_t>& max_time_ms() const;

    ///
    /// Sets a projection that limits the fields to return.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    void projection(bson::document::view projection);

    ///
    /// Gets the current projection set on this operation.
    ///
    /// @return The current projection.
    ///
    /// @see http://docs.mongodb.org/manual/tutorial/project-fields-from-query-results/
    ///
    const stdx::optional<bson::document::view>& projection() const;

    ///
    /// Sets the order to search for a matching document.
    ///
    /// @warning This can influence which document the operation modifies if the provided filter
    /// selects multiple documents.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.findAndModify/
    ///
    void sort(bson::document::view ordering);

    ///
    /// Gets the current sort ordering.
    ///
    /// @return The current sort ordering.
    ///
    /// @see http://docs.mongodb.org/manual/reference/method/db.collection.findAndModify/
    ///
    const stdx::optional<bson::document::view>& sort() const;

   private:
    stdx::optional<std::int64_t> _max_time_ms;
    stdx::optional<bson::document::view> _projection;
    stdx::optional<bson::document::view> _ordering;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
