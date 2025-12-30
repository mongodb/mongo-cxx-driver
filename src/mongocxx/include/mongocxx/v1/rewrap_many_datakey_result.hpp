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

#include <mongocxx/v1/rewrap_many_datakey_result-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/bulk_write.hpp>
#include <mongocxx/v1/config/export.hpp>

namespace mongocxx {
namespace v1 {

///
/// The result of a "rewrapManyDataKey" operation.
///
/// Supported fields include:
/// - `result` ("bulkWriteResult")
///
/// @see
/// - [Rotate and Rewrap Encryption Keys (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/fundamentals/manage-keys/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class rewrap_many_datakey_result {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~rewrap_many_datakey_result();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey_result(rewrap_many_datakey_result&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_result&) operator=(rewrap_many_datakey_result&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey_result(rewrap_many_datakey_result const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(rewrap_many_datakey_result&) operator=(rewrap_many_datakey_result const& other);

    ///
    /// Return the current "bulkWriteResult" field.
    ///
    /// @returns Empty when the write operation is unacknowledged.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::bulk_write::result>) result() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::rewrap_many_datakey_result.
///
