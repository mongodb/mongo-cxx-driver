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

#include <mongocxx/v1/gridfs/upload_result-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>

namespace mongocxx {
namespace v1 {
namespace gridfs {

///
/// Result of uploading a file to a GridFS bucket.
///
/// Supported fields include:
/// - `id`
///
/// @see
/// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class upload_result {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~upload_result();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() upload_result(upload_result&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_result&) operator=(upload_result&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() upload_result(upload_result const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_result&) operator=(upload_result const& other);

    ///
    /// Return the ID of the uploaded GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::view) id() const;

    ///
    /// Compare equal when all supported fields compare equal.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(upload_result const& lhs, upload_result const& rhs);

    friend bool operator!=(upload_result const& lhs, upload_result const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::gridfs::upload_result.
///
