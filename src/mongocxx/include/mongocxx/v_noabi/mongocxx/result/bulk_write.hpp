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

#include <mongocxx/result/bulk_write-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <cstddef>
#include <cstdint>
#include <map>
#include <utility>
#include <vector> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types.hpp>         // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB bulk write operation.
///
class bulk_write {
   private:
    v1::bulk_write::result _result;

   public:
    using id_map = std::map<std::size_t, bsoncxx::v_noabi::document::element>;

    ///
    /// @deprecated For internal use only.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() bulk_write(bsoncxx::v_noabi::document::value raw_response);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ bulk_write(v1::bulk_write::result result) : _result{std::move(result)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated views.
    ///
    explicit operator v1::bulk_write::result() && {
        return std::move(_result);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::bulk_write::result() const& {
        return _result;
    }

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

    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(bulk_write const& lhs, bulk_write const& rhs);

    friend bool operator!=(bulk_write const& lhs, bulk_write const& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::bulk_write from_v1(v1::bulk_write::result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write::result to_v1(v_noabi::result::bulk_write v) {
    return v1::bulk_write::result{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::bulk_write.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
