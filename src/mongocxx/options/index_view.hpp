// Copyright 2017 MongoDB Inc.
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

#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing optional arguments to IndexView operations
///
class MONGOCXX_API index_view {
   public:
    index_view();

    ///
    /// Sets the maximum amount of time for this operation to run (server-side) in milliseconds.
    ///
    /// @param max_time
    ///   The max amount of time (in milliseconds).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    index_view& max_time(std::chrono::milliseconds max_time);

    ///
    /// The current max_time setting.
    ///
    /// @return
    ///   The current max allowed running time (in milliseconds).
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const bsoncxx::stdx::optional<std::chrono::milliseconds>& max_time() const;

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
    index_view& write_concern(mongocxx::write_concern write_concern);

    ///
    /// Gets the current write concern.
    ///
    /// @return
    ///   The current write concern.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/command/findAndModify/
    ///
    const bsoncxx::stdx::optional<mongocxx::write_concern>& write_concern() const;

   private:
    bsoncxx::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::stdx::optional<mongocxx::write_concern> _write_concern;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
