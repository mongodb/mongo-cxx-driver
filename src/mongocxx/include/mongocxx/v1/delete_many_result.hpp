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

#include <mongocxx/v1/delete_many_result-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/bulk_write.hpp>
#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {

///
/// The result of a "deleteOne" operation.
///
/// @see
/// - [Delete Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/delete-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class delete_many_result {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~delete_many_result();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many_result(delete_many_result&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_result&) operator=(delete_many_result&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() delete_many_result(delete_many_result const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many_result&) operator=(delete_many_result const& other);

    ///
    /// Return the raw bulk write result.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::bulk_write::result) result() const;

    ///
    /// Return the number of documents that were deleted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int64_t) deleted_count() const;

    ///
    /// Compare equal when `lhs.result()` and `rhs.result()` compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(delete_many_result const& lhs, delete_many_result const& rhs);

    friend bool operator!=(delete_many_result const& lhs, delete_many_result const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::delete_many_result.
///
