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

#include <chrono>
#include <cstdint>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a MongoDB find_and_modify delete operation
///
class MONGOCXX_API find_one_and_delete {
   public:
    /// Sets the collation for this operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    find_one_and_delete& collation(bsoncxx::document::view_or_value collation);

    ///
    /// Retrieves the current collation for this operation.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& collation() const;

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time
    ///   The max amount of running time (in milliseconds).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    find_one_and_delete& max_time(std::chrono::milliseconds max_time);

    ///
    /// The current max_time setting.
    ///
    /// @return the current max time (in milliseconds).
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const stdx::optional<std::chrono::milliseconds>& max_time() const;

    ///
    /// Sets a projection that limits the fields to return.
    ///
    /// @param projection
    ///   The projection document.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    find_one_and_delete& projection(bsoncxx::document::view_or_value projection);

    ///
    /// Gets the current projection set on this operation.
    ///
    /// @return The current projection.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& projection() const;

    ///
    /// Sets the order to search for a matching document.
    ///
    /// @warning This can influence which document the operation modifies if the provided filter
    /// selects multiple documents.
    ///
    /// @param ordering
    ///   Document describing the order of the documents to be returned.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    find_one_and_delete& sort(bsoncxx::document::view_or_value ordering);

    ///
    /// Gets the current sort ordering.
    ///
    /// @return The current sort ordering.
    ///
    /// @see https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& sort() const;

    ///
    /// Sets the write concern for this operation.
    ///
    /// @param write_concern
    ///   Object representing the write concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    find_one_and_delete& write_concern(mongocxx::write_concern write_concern);

    ///
    /// Gets the current write concern.
    ///
    /// @return
    ///   The current write concern.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const stdx::optional<mongocxx::write_concern>& write_concern() const;

   private:
    stdx::optional<bsoncxx::document::view_or_value> _collation;
    stdx::optional<std::chrono::milliseconds> _max_time;
    stdx::optional<bsoncxx::document::view_or_value> _projection;
    stdx::optional<bsoncxx::document::view_or_value> _ordering;
    stdx::optional<mongocxx::write_concern> _write_concern;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
