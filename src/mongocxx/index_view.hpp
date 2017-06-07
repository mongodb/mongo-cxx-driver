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

#include <string>

#include <bsoncxx/document/value.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/index_model.hpp>
#include <mongocxx/index_names.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class MONGOCXX_API index_view {
   public:
    ///
    /// Create an index_view object.
    ///
    index_view();

    ///
    /// Returns a cursor over all the indexes.
    ///
    cursor iterator();

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    std::string create_one(const bsoncxx::document::view_or_value& keys,
                           const bsoncxx::document::view_or_value& options = {});

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    std::string create_one(const index_model& index);

    ///
    /// Adds a container of indexes to the collection.
    ///
    /// @tparam containter_type
    ///   The container type. Must meet the requirements for the container concept with a value
    ///   type of index_model.
    ///
    template <typename container_type>
    index_names::iterator create_many(const container_type& indexes);

    ///
    /// Drops a single index by name.
    ///
    /// @exception:
    ///   should throw something
    ///
    void drop_one(stdx::string_view name);

    ///
    /// Attempts to drop a single index from the collection given the keys and options.
    ///
    /// @exception:
    ///   should throw something
    ///
    void drop_one(const bsoncxx::document::view_or_value& keys,
                  const bsoncxx::document::view_or_value& options = {});

    ///
    /// Attempts to drop a single index from the collection given an index.
    ///
    /// @exception:
    ///   should throw something
    ///
    void drop_one(const index_model& index);

    ///
    /// Drops all indexes in the collection.
    ///
    void drop_all();
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>