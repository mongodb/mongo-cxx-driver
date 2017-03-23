// Copyright 2017 MongoDB Inc.
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

#include <istream>
#include <memory>
#include <ostream>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/gridfs/downloader.hpp>
#include <mongocxx/gridfs/uploader.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/gridfs/bucket.hpp>
#include <mongocxx/options/gridfs/upload.hpp>
#include <mongocxx/result/gridfs/upload.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace gridfs {

///
/// Class representing a GridFS bucket.
///
/// A GridFS bucket is used to store files that may be too large to store in a single document due
/// to the 16 MB limit. The bucket comprises of two collections, `<bucketname>.files` and
/// `<bucketname>.chunks.` A file written to a GridFS bucket will be serialized into zero or more
/// chunk documents stored in the `<bucketname>.chunks` collection, and one document will be stored
/// in the `<bucketname>.files` collection containing the information about the file. Users should
/// not modify these collections directly.
///
/// @see http://www.mongodb.org/display/DOCS/GridFS
///
class MONGOCXX_API bucket {
   public:
    ///
    /// Constructs a new GridFS bucket.
    ///
    /// @param db
    ///   The database that will hold the GridFS bucket collections.
    ///
    /// @param options
    ///   Optional arguments; see options::gridfs::bucket.
    ///
    /// @throws if options are invalid.
    ///
    bucket(const database& db, const options::gridfs::bucket& options = {});

    ///
    /// Move constructs a bucket.
    ///
    bucket(bucket&&) noexcept;

    ///
    /// Move assigns a bucket.
    ///
    bucket& operator=(bucket&&) noexcept;

    ///
    /// Copy constructs a bucket.
    ///
    bucket(const bucket&);

    ///
    /// Copy assigns a bucket.
    ///
    bucket& operator=(const bucket&);

    ///
    /// Destroys a bucket.
    ///
    ~bucket();

    ///
    /// Opens a gridfs::uploader to create a new GridFS file. The id of the file will be
    /// automatically generated as an ObjectId.
    ///
    /// @param filename
    ///   The name of the file to be uploaded. A bucket can contain multiple files with the same
    ///   name.
    ///
    /// @param options
    ///   Optional arguments; see options::gridfs::upload.
    ///
    /// @return
    ///   A stream for writing to the GridFS file.
    ///
    /// @throws if opening the stream fails.
    ///
    uploader open_upload_stream(stdx::string_view filename,
                                const options::gridfs::upload& options = {});

    ///
    /// Opens a gridfs::uploader to create a new GridFS file.
    ///
    /// @param id
    ///   The unique id of the file being uploaded.
    ///
    /// @param filename
    ///   The name of the file to be uploaded. A bucket can contain multiple files with the same
    ///   name.
    ///
    /// @param options
    ///   Optional arguments; see options::gridfs::upload.
    ///
    /// @return
    ///   The gridfs::uploader to which the GridFS file should be written.
    ///
    /// @throws if opening the stream fails.
    ///
    uploader open_upload_stream_with_id(bsoncxx::types::value id,
                                        stdx::string_view filename,
                                        const options::gridfs::upload& options = {});

    ///
    /// Creates a new GridFS file by uploading bytes from an input stream. The id of the file will
    /// be automatically generated as an ObjectId.
    ///
    /// @param filename
    ///   The name of the file to be uploaded. A bucket can contain multiple files with the same
    ///   name.
    ///
    /// @param source
    ///    The non-null stream from which the GridFS file should be read.
    ///
    /// @param options
    ///   Optional arguments; see options::gridfs::upload.
    ///
    /// @return
    ///   The id of the uploaded file.
    ///
    /// @throws if reading from `source` throws an exception or if writing to the database fails.
    ///
    result::gridfs::upload upload_from_stream(stdx::string_view filename,
                                              std::istream* source,
                                              const options::gridfs::upload& options = {});

    ///
    /// Creates a new GridFS file with a user-supplied unique id by uploading bytes from an input
    /// stream.
    ///
    /// @param id
    ///   A unique id for the file being uploaded.
    ///
    /// @param filename
    ///   The name of the file to be uploaded. A bucket can contain multiple files with the same
    ///   name.
    ///
    /// @param source
    ///    The non-null stream from which the GridFS file should be read.
    ///
    /// @param options
    ///   Optional arguments; see options::gridfs::upload.
    ///
    /// @throws if reading from `source` throws an exception of if writing to the database fails.
    ///
    void upload_from_stream_with_id(bsoncxx::types::value id,
                                    stdx::string_view filename,
                                    std::istream* source,
                                    const options::gridfs::upload& options = {});

    ///
    /// Opens a gridfs::downloader to read a GridFS file.
    ///
    /// @param id
    ///   The id of the file to read.
    ///
    /// @return
    ///   The gridfs::downloader from which the GridFS file should be read.
    ///
    /// @throws if the file does not exist or is malformed.
    ///
    downloader open_download_stream(bsoncxx::types::value id);

    ///
    /// Downloads the contents of a stored GridFS file from the bucket and writes it to a stream.
    ///
    /// @param id
    ///   The id of the file to read.
    ///
    /// @param destination
    ///   The non-null stream to which the GridFS file should be written.
    ///
    /// @throws if writing to `destination` throws an exception or if reading from the database
    /// fails.
    ///
    void download_to_stream(bsoncxx::types::value id, std::ostream* destination);

    ///
    /// Deletes a GridFS file from the bucket.
    ///
    /// @param id
    ///   The id of the file to be deleted.
    ///
    /// @throws if no file with the given id exists or if the delete operation throws an exception.
    ///
    void delete_file(bsoncxx::types::value id);

    ///
    /// Finds the documents in the files collection of the bucket which match the provided filter.
    ///
    /// @param filter
    ///   Document view representing a document that should match the query.
    ///
    /// @param options
    ///   Optional arguments; see options::find.
    ///
    /// @return
    ///   A mongocxx::cursor with the results. If the query fails, the cursor throws
    ///   mongocxx::query_exception when the returned cursor is iterated.
    ///
    /// @throws mongocxx::logic_error if the options are invalid, or if the unsupported option
    /// modifiers "$query" or "$explain" are used.
    ///
    /// @see mongocxx::collection::find.
    ///
    cursor find(bsoncxx::document::view_or_value filter, const options::find& options = {});

    ///
    /// Gets the name of the GridFS bucket.
    ///
    /// @return
    ///   The name of the GridFS bucket.
    ///
    stdx::string_view bucket_name() const;

   private:
    MONGOCXX_PRIVATE void create_indexes_if_nonexistent();

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE impl& _get_impl();
    MONGOCXX_PRIVATE const impl& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

}  // namespace gridfs
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
