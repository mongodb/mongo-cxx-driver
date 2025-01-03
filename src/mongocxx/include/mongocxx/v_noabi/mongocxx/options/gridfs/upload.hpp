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

#include <mongocxx/options/gridfs/upload-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {
namespace gridfs {

///
/// Used by @ref mongocxx::v_noabi::gridfs::bucket.
///
class upload {
   public:
    ///
    /// Sets the chunk size of the GridFS file being uploaded. Defaults to the chunk size specified
    /// in options::gridfs::bucket.
    ///
    /// @param chunk_size_bytes
    ///   The size of the chunks in bytes.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload&) chunk_size_bytes(std::int32_t chunk_size_bytes);

    ///
    /// Gets the chunk size of the GridFS file being uploaded.
    ///
    /// @return
    ///   The chunk size of the GridFS file being uploaded in bytes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t> const&)
    chunk_size_bytes() const;

    ///
    /// Sets the metadata field of the GridFS file being uploaded. A GridFS file can store arbitrary
    /// metadata in the form of a BSON document.
    ///
    /// @param metadata
    ///   The metadata document for the GridFS file.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(upload&) metadata(bsoncxx::v_noabi::document::view_or_value metadata);

    ///
    /// Gets the metadata of the GridFS file being uploaded.
    ///
    /// @return
    ///   The metadata document of the GridFS file.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const&)
    metadata() const;

   private:
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _chunk_size_bytes;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _metadata;
};

} // namespace gridfs
} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::gridfs::upload.
///
