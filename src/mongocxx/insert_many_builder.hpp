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

#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/options/insert.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class to build an insert_many bulk write operation.
///
/// This can be used on its own, or called through the helper mongocxx::collection::insert_many.
///
class MONGOCXX_API insert_many_builder {
   public:
    ///
    /// Initializes a new builder for an insert_many bulk write operation.
    ///
    /// @param options
    ///   The options for this insert_many.
    ///
    explicit insert_many_builder(const options::insert& options);

    ///
    /// Appends a single insert to insert_many bulk write operation. The document is copied into the
    /// bulk operation completely, so there is no dependency between the life of the document and
    /// the insert_many bulk write operation itself.
    ///
    /// @param doc
    ///   The next document to be added to the insert_many operation.
    ///
    void operator()(const bsoncxx::document::view& doc);

    ///
    /// Executes the underlying insert_many bulk operation.
    ///
    /// @param col
    ///   A pointer to the collection into which the documents will be inserted.
    ///
    stdx::optional<result::insert_many> insert(collection* col) const;

   private:
    bulk_write _writes;
    result::insert_many::id_map _inserted_ids;
    std::size_t _index;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
