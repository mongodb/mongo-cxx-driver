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

#include <mongocxx/v1/insert_many_result-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/bulk_write.hpp>
#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <map>

namespace mongocxx {
namespace v1 {

///
/// The result of an "insertMany" operation.
///
/// Supported fields include:
/// - `result`
///
/// @see
/// - [Insert Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/insert-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class insert_many_result {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~insert_many_result();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() insert_many_result(insert_many_result&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_many_result&) operator=(insert_many_result&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() insert_many_result(insert_many_result const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_many_result&) operator=(insert_many_result const& other);

    ///
    /// Return the raw bulk write result.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write::result) result() const;

    ///
    /// Return the number of inserted documents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) inserted_count() const;

    ///
    /// A map from the operation index to the inserted document ID.
    ///
    using id_map = std::map<std::int64_t, bsoncxx::v1::types::view>;

    ///
    /// Return a map from the operation index to the inserted document ID.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(id_map) inserted_ids() const;

    ///
    /// Compare equal when all supported fields compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(insert_many_result const& lhs, insert_many_result const& rhs);

    friend bool operator!=(insert_many_result const& lhs, insert_many_result const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

    class internal;

   private:
    /* explicit(false) */ insert_many_result(void* impl);
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::insert_many_result.
///
