// Copyright 2009-present MongoDB, Inc.
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

#include <cstdint>
#include <map>
#include <vector>

#include <mongocxx/result/bulk_write-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// Class representing the result of a MongoDB bulk write operation.
///
class bulk_write {
   public:
    using id_map = std::map<std::size_t, bsoncxx::v_noabi::document::element>;

    // This constructor is public for testing purposes only
    explicit MONGOCXX_ABI_EXPORT_CDECL() bulk_write(bsoncxx::v_noabi::document::value raw_response);

    ///
    /// Gets the number of documents that were inserted during this operation.
    ///
    /// @return The number of documents that were inserted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) inserted_count() const;

    ///
    /// Gets the number of documents that were matched during this operation.
    ///
    /// @return The number of documents that were matched.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) matched_count() const;

    ///
    /// Gets the number of documents that were modified during this operation.
    ///
    /// @return The number of documents that were modified.
    ///
    /// @throws with server versions below 2.6 due to the field `nModified` not being returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) modified_count() const;

    ///
    /// Gets the number of documents that were deleted during this operation.
    ///
    /// @return The number of documents that were deleted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) deleted_count() const;

    ///
    /// Gets the number of documents that were upserted during this operation.
    ///
    /// @return The number of documents that were upserted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) upserted_count() const;

    ///
    /// Gets the ids of the upserted documents.
    ///
    /// @note The returned id_map must not be accessed after the bulk_write object is destroyed.
    /// @return A map from bulk write index to _id field for upserted documents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(id_map) upserted_ids() const;

    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(const bulk_write&, const bulk_write&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(const bulk_write&, const bulk_write&);

   private:
    bsoncxx::v_noabi::document::view view() const;

    bsoncxx::v_noabi::document::value _response;
};

}  // namespace result
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::bulk_write.
///
