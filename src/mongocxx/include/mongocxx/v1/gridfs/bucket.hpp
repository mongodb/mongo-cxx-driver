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

#include <mongocxx/v1/gridfs/bucket-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/gridfs/downloader-fwd.hpp>
#include <mongocxx/v1/gridfs/upload_result-fwd.hpp>
#include <mongocxx/v1/gridfs/uploader-fwd.hpp>
#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/find_options.hpp>
#include <mongocxx/v1/gridfs/upload_options.hpp>

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <system_error>

namespace mongocxx {
namespace v1 {
namespace gridfs {

///
/// A GridFS bucket.
///
/// @see
/// - [GridFS (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bucket {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    class options;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~bucket();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() bucket(bucket&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bucket&) operator=(bucket&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() bucket(bucket const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bucket&) operator=(bucket const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() bucket();

    ///
    /// Return true when `*this` is NOT in an assign-or-destroy-only state.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator bool() const;

    ///
    /// Equivalent to @ref open_upload_stream_with_id with a file ID generated using @ref bsoncxx::v1::oid.
    ///
    /// @{
    v1::gridfs::uploader open_upload_stream(
        bsoncxx::v1::stdx::string_view filename,
        v1::gridfs::upload_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::uploader) open_upload_stream(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view filename,
        v1::gridfs::upload_options const& opts = {});
    /// @}
    ///

    ///
    /// Return an uploader for a new file to this bucket.
    ///
    /// @note The file is not completely uploaded until @ref mongocxx::v1::gridfs::uploader::close() is invoked.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::invalid_chunk_size_bytes if the
    /// "chunkSizeBytes" field is not a positive value.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::uploader) open_upload_stream_with_id(
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view filename,
        v1::gridfs::upload_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::uploader) open_upload_stream_with_id(
        v1::client_session const& session,
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view filename,
        v1::gridfs::upload_options const& opts = {});
    /// @}
    ///

    ///
    /// Equivalent to @ref upload_from_stream_with_id with a file ID generated using @ref bsoncxx::v1::oid.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::upload_result) upload_from_stream(
        bsoncxx::v1::stdx::string_view filename,
        std::istream& input,
        v1::gridfs::upload_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::upload_result) upload_from_stream(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view filename,
        std::istream& input,
        v1::gridfs::upload_options const& opts = {});
    /// @}
    ///

    ///
    /// Upload the contents of `input` as a new file to this bucket.
    ///
    /// @throws std::ios_base::failure if an error is encountered when reading from `input`.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) upload_from_stream_with_id(
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view filename,
        std::istream& input,
        v1::gridfs::upload_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(void) upload_from_stream_with_id(
        v1::client_session const& session,
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view filename,
        std::istream& input,
        v1::gridfs::upload_options const& opts = {});
    /// @}
    ///

    ///
    /// Return a downloader for the requested file from this bucket.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::not_found if the requested file
    /// does not exist.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::corrupt_data if the
    /// GridFS file data is invalid or inconsistent.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::downloader) open_download_stream(bsoncxx::v1::types::view id);

    MONGOCXX_ABI_EXPORT_CDECL(v1::gridfs::downloader) open_download_stream(
        v1::client_session const& session,
        bsoncxx::v1::types::view id);
    /// @}
    ///

    ///
    /// Download the entire contents of the requested file from this bucket into `output`.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::not_found if the requested file
    /// does not exist.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::corrupt_data if the
    /// GridFS file data is invalid or inconsistent.
    /// @throws std::ios_base::failure if an error is encountered when writing to `output`.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) download_to_stream(bsoncxx::v1::types::view id, std::ostream& output);

    MONGOCXX_ABI_EXPORT_CDECL(void)
    download_to_stream(v1::client_session const& session, bsoncxx::v1::types::view id, std::ostream& destination);
    /// @}
    ///

    ///
    /// Partially download the contents of the requested file from this bucket into `output`.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::not_found if the requested file
    /// does not exist.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::corrupt_data if the
    /// GridFS file data is invalid or inconsistent.
    /// @throws std::ios_base::failure if an error is encountered when writing to `output`.
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::invalid_byte_range if `[start,
    /// end)` is not a valid range of byte indexes within the requested file.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void)
    download_to_stream(bsoncxx::v1::types::view id, std::ostream& output, std::size_t start, std::size_t end);

    MONGOCXX_ABI_EXPORT_CDECL(void) download_to_stream(
        v1::client_session const& session,
        bsoncxx::v1::types::view id,
        std::ostream& output,
        std::size_t start,
        std::size_t end);
    /// @}
    ///

    ///
    /// Delete the requested file from this bucket.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::gridfs::bucket::errc::not_found if the requested file
    /// does not exist.
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(void) delete_file(bsoncxx::v1::types::view id);

    MONGOCXX_ABI_EXPORT_CDECL(void) delete_file(v1::client_session const& session, bsoncxx::v1::types::view id);
    /// @}
    ///

    ///
    /// Find files within this bucket matching the given query filter.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) find(bsoncxx::v1::document::view filter, v1::find_options const& opts = {});

    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor)
    find(v1::client_session const& session, bsoncxx::v1::document::view filter, v1::find_options const& opts = {});
    /// @}
    ///

    ///
    /// Return the name of this bucket.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) bucket_name() const;

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::gridfs::bucket.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,                     ///< Zero.
        invalid_chunk_size_bytes, ///< The "chunkSizeBytes" field must be a positive value.
        not_found,                ///< The requested GridFS file does not exist.
        corrupt_data,             ///< The GridFS file is in an invalid or inconsistent state.
        invalid_byte_range, ///< [start, end) must be a valid range of byte indexes within the requested GridFS file.
    };

    ///
    /// The error category for @ref mongocxx::v1::gridfs::bucket::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

///
/// Options for @ref mongocxx::v1::gridfs::bucket.
///
/// Supported fields include:
/// - `bucket_name` ("bucketName")
/// - `chunk_size_bytes` ("chunkSizeBytes")
/// - `read_concern` ("readConcern")
/// - `read_preference` ("readPreference")
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [GridFS (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/gridfs/gridfs-spec/)
/// - [GridFS for Self-Managed Deployments (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/gridfs/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bucket::options {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "bucketName" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) bucket_name(std::string v);

    ///
    /// Return the current "bucketName" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) bucket_name() const;

    ///
    /// Set the "chunkSizeBytes" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) chunk_size_bytes(std::int32_t v);

    ///
    /// Return the current "chunkSizeBytes" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) chunk_size_bytes() const;

    ///
    /// Set the "readConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) read_concern(v1::read_concern v);

    ///
    /// Return the current "readConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_concern>) read_concern() const;

    ///
    /// Set the "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) read_preference(v1::read_preference v);

    ///
    /// Return the current "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_preference>) read_preference() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) write_concern(v1::write_concern v);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
};

} // namespace gridfs
} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::gridfs::bucket.
///
