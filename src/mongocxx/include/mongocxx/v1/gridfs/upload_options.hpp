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

#include <mongocxx/v1/gridfs/upload_options-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>

namespace mongocxx {
namespace v1 {
namespace gridfs {

///
/// Options related to uploading a file to a GridFS bucket.
///
/// Supported fields include:
/// - `chunk_size_bytes` ("chunkSizeBytes")
/// - `metadata` ("metadata")
///
/// @see
/// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class upload_options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~upload_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() upload_options(upload_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_options&) operator=(upload_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() upload_options(upload_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_options&) operator=(upload_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() upload_options();

    ///
    /// Set the "chunkSizeBytes" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_options&) chunk_size_bytes(std::int32_t chunk_size_bytes);

    ///
    /// Return the current "chunkSizeBytes" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) chunk_size_bytes() const;

    ///
    /// Set the "metadata" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload_options&) metadata(bsoncxx::v1::document::value metadata);

    ///
    /// Return the current "metadata" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) metadata() const;
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::gridfs::upload_options.
///
