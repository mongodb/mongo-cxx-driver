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

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/gridfs_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/gridfs/upload.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using namespace mongocxx;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

TEST_CASE("mongocxx::gridfs::bucket default constructor makes invalid bucket", "[gridfs::bucket]") {
    instance::current();

    gridfs::bucket bucket;
    REQUIRE(!bucket);
    REQUIRE_THROWS_AS(bucket.bucket_name(), logic_error);
}

TEST_CASE("mongocxx::gridfs::bucket copy constructor", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_bucket_copy_constructor"];

    SECTION("constructing from valid") {
        gridfs::bucket bucket_a = db.gridfs_bucket(options::gridfs::bucket{}.bucket_name("a"));
        gridfs::bucket bucket_b{bucket_a};
        REQUIRE(bucket_b);
        REQUIRE(bucket_b.bucket_name() == stdx::string_view{"a"});
    }

    SECTION("constructing from invalid") {
        gridfs::bucket bucket_a;
        gridfs::bucket bucket_b{bucket_a};
        REQUIRE(!bucket_b);
    }
}

TEST_CASE("mongocxx::gridfs::bucket copy assignment operator", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_bucket_copy_assignment"];

    SECTION("assigning valid to valid") {
        gridfs::bucket bucket_a = db.gridfs_bucket(options::gridfs::bucket{}.bucket_name("a"));
        gridfs::bucket bucket_b = db.gridfs_bucket(options::gridfs::bucket{}.bucket_name("b"));
        bucket_b = bucket_a;
        REQUIRE(bucket_b);
        REQUIRE(bucket_b.bucket_name() == stdx::string_view{"a"});
    }

    SECTION("assigning invalid to valid") {
        gridfs::bucket bucket_a;
        gridfs::bucket bucket_b = db.gridfs_bucket(options::gridfs::bucket{}.bucket_name("b"));
        bucket_b = bucket_a;
        REQUIRE(!bucket_b);
    }

    SECTION("assigning valid to invalid") {
        gridfs::bucket bucket_a = db.gridfs_bucket(options::gridfs::bucket{}.bucket_name("a"));
        gridfs::bucket bucket_b;
        bucket_b = bucket_a;
        REQUIRE(bucket_b);
        REQUIRE(bucket_b.bucket_name() == stdx::string_view{"a"});
    }

    SECTION("assigning invalid to invalid") {
        gridfs::bucket bucket_a;
        gridfs::bucket bucket_b;
        bucket_b = bucket_a;
        REQUIRE(!bucket_b);
    }
}

TEST_CASE("database::gridfs_bucket() throws error when options are invalid", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_bucket_error_invalid_options"];

    options::gridfs::bucket bucket_options;

    SECTION("empty bucket name") {
        bucket_options.bucket_name("");
    }
    SECTION("zero chunk size") {
        bucket_options.chunk_size_bytes(0);
    }
    SECTION("negative chunk size") {
        bucket_options.chunk_size_bytes(-1);
    }

    REQUIRE_THROWS_AS(db.gridfs_bucket(bucket_options), logic_error);
}

TEST_CASE("uploading throws error when options are invalid", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_error_invalid_options"];
    gridfs::bucket bucket = db.gridfs_bucket();

    options::gridfs::upload upload_options;

    SECTION("zero chunk size") {
        upload_options.chunk_size_bytes(0);
    }
    SECTION("negative chunk size") {
        upload_options.chunk_size_bytes(-1);
    }

    REQUIRE_THROWS_AS(bucket.open_upload_stream("filename", upload_options), logic_error);
    REQUIRE_THROWS_AS(
        bucket.open_upload_stream_with_id(
            bsoncxx::types::value{bsoncxx::types::b_int32{0}}, "filename", upload_options),
        logic_error);

    std::istringstream iss{"foo"};
    REQUIRE_THROWS_AS(bucket.upload_from_stream("filename", &iss, upload_options), logic_error);
    REQUIRE_THROWS_AS(
        bucket.upload_from_stream_with_id(
            bsoncxx::types::value{bsoncxx::types::b_int32{0}}, "filename", &iss, upload_options),
        logic_error);
}

TEST_CASE("downloading throws error when files document is corrupt", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_files_doc_corrupt"];
    gridfs::bucket bucket = db.gridfs_bucket();

    db["fs.files"].drop();

    const std::int32_t k_expected_chunk_size_bytes = 255 * 1024;  // Default chunk size.
    const std::int64_t k_expected_file_length = 1024 * 1024;
    stdx::optional<bsoncxx::types::value> chunk_size{
        bsoncxx::types::value{bsoncxx::types::b_int32{k_expected_chunk_size_bytes}}};
    stdx::optional<bsoncxx::types::value> length{
        bsoncxx::types::value{bsoncxx::types::b_int64{k_expected_file_length}}};
    bool expect_success = false;

    // Tests for invalid chunk size.
    SECTION("zero chunk size") {
        chunk_size = bsoncxx::types::value{bsoncxx::types::b_int32{0}};
    }
    SECTION("negative chunk size") {
        chunk_size = bsoncxx::types::value{bsoncxx::types::b_int32{-1}};
    }
    SECTION("chunk size too large") {
        const std::int32_t k_max_document_size = 16 * 1024 * 1024;
        chunk_size = bsoncxx::types::value{bsoncxx::types::b_int32{k_max_document_size + 1}};
    }
    SECTION("chunk size of wrong type") {
        chunk_size = bsoncxx::types::value{bsoncxx::types::b_utf8{"invalid"}};
    }
    SECTION("missing chunk size") {
        chunk_size = stdx::nullopt;
    }

    // Tests for invalid length.
    SECTION("negative length") {
        length = bsoncxx::types::value{bsoncxx::types::b_int64{-1}};
    }
    SECTION("invalid length") {
        length = bsoncxx::types::value{bsoncxx::types::b_utf8{"invalid"}};
    }
    SECTION("missing length") {
        length = stdx::nullopt;
    }

    // Test for too many chunks.
    SECTION("too many chunks") {
        chunk_size = bsoncxx::types::value{bsoncxx::types::b_int32{1}};
        length = bsoncxx::types::value{bsoncxx::types::b_int64{
            static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()) + 1}};
    }

    // Test for valid length and chunk size.
    SECTION("valid length and chunk size") {
        expect_success = true;
    }

    {
        bsoncxx::builder::basic::document files_doc;
        files_doc.append(kvp("_id", 0));
        if (length) {
            files_doc.append(kvp("length", *length));
        }
        if (chunk_size) {
            files_doc.append(kvp("chunkSize", *chunk_size));
        }

        db["fs.files"].insert_one(files_doc.extract());
    }

    auto open_download_stream = [&bucket]() {
        return bucket.open_download_stream(bsoncxx::types::value{bsoncxx::types::b_int32{0}});
    };

    if (expect_success) {
        gridfs::downloader downloader = open_download_stream();
        REQUIRE(downloader.chunk_size() == k_expected_chunk_size_bytes);
        REQUIRE(downloader.file_length() == k_expected_file_length);
    } else {
        REQUIRE_THROWS_AS(open_download_stream(), gridfs_exception);
    }
}

TEST_CASE("downloading throws error when chunks document is corrupt", "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_chunk_doc_corrupt"];
    gridfs::bucket bucket = db.gridfs_bucket();

    db["fs.files"].drop();
    db["fs.chunks"].drop();

    const std::uint8_t k_expected_data_byte = 'd';

    stdx::optional<bsoncxx::types::value> n{bsoncxx::types::value{bsoncxx::types::b_int32{0}}};
    stdx::optional<bsoncxx::types::value> data{bsoncxx::types::value{
        bsoncxx::types::b_binary{bsoncxx::binary_sub_type::k_binary, 1, &k_expected_data_byte}}};
    bool expect_success = false;

    // Tests for invalid n.
    SECTION("missing n") {
        n = stdx::nullopt;
    }
    SECTION("wrong type for n") {
        n = bsoncxx::types::value{bsoncxx::types::b_int64{0}};
    }

    // Tests for invalid data.
    SECTION("missing data") {
        data = stdx::nullopt;
    }
    SECTION("wrong type for data") {
        data = bsoncxx::types::value{bsoncxx::types::b_bool{true}};
    }

    // Test for valid n and data.
    SECTION("valid n and data") {
        expect_success = true;
    }

    {
        bsoncxx::builder::basic::document chunk_doc;
        chunk_doc.append(kvp("files_id", 0));
        if (n) {
            chunk_doc.append(kvp("n", *n));
        }
        if (data) {
            chunk_doc.append(kvp("data", *data));
        }

        db["fs.chunks"].insert_one(chunk_doc.extract());
        db["fs.files"].insert_one(
            make_document(kvp("_id", 0), kvp("length", 1), kvp("chunkSize", 1)));
    }

    gridfs::downloader downloader =
        bucket.open_download_stream(bsoncxx::types::value{bsoncxx::types::b_int32{0}});
    auto downloader_read_one = [&downloader]() {
        stdx::optional<std::uint8_t> result;
        std::uint8_t byte;
        std::size_t bytes_downloaded = downloader.read(&byte, 1);
        if (bytes_downloaded > 0) {
            result = byte;
        }
        return result;
    };

    if (expect_success) {
        stdx::optional<std::uint8_t> result = downloader_read_one();
        REQUIRE(result);
        REQUIRE(*result == k_expected_data_byte);
        result = downloader_read_one();
        REQUIRE(!result);
    } else {
        REQUIRE_THROWS_AS(downloader_read_one(), gridfs_exception);
    }
}

// Add an arbitrary GridFS file with a specified length, chunk size, and id to a database's default
// GridFS bucket (i.e. the "fs.files" and "fs.chunks" collections).
//
// Returns a vector of the bytes stored in the GridFS chunks.
std::vector<std::uint8_t> manual_gridfs_initialize(database db,
                                                   std::int64_t length,
                                                   std::int32_t chunk_size,
                                                   bsoncxx::types::value id) {
    std::vector<std::uint8_t> bytes;

    // Populate the vector with arbitrary values.
    for (std::int64_t i = 0; i < length; ++i) {
        bytes.push_back((i + 200) % 256);
    }

    std::vector<bsoncxx::document::value> chunks;
    std::int64_t bytes_written = 0;

    for (std::int32_t i = 0; bytes_written < length; ++i) {
        // The last chunk should be truncated to fit specified file length.
        std::int32_t current_chunk_size = static_cast<std::int32_t>(
            std::min(static_cast<std::int64_t>(chunk_size), length - bytes_written));

        bsoncxx::types::b_binary data = {bsoncxx::binary_sub_type::k_binary,
                                         static_cast<std::uint32_t>(current_chunk_size),
                                         bytes.data() + bytes_written};

        chunks.push_back(make_document(kvp("files_id", id), kvp("n", i), kvp("data", data)));
        bytes_written += current_chunk_size;
    }

    db["fs.chunks"].insert_many(chunks);
    db["fs.files"].insert_one(make_document(kvp("_id", id),
                                            kvp("length", length),
                                            kvp("chunkSize", bsoncxx::types::b_int32{chunk_size})));

    return bytes;
}

TEST_CASE("mongocxx::gridfs::downloader::read with arbitrary sizes", "[gridfs::downloader]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_download_read_test"];
    gridfs::bucket bucket = db.gridfs_bucket();

    db["fs.files"].delete_many({});
    db["fs.chunks"].delete_many({});

    std::int64_t file_length = 100;
    std::int32_t chunk_size = 9;
    std::int32_t read_size;

    SECTION("read_size = 1") {
        read_size = 1;
    }

    SECTION("read_size = chunk_size - 1") {
        read_size = chunk_size - 1;
    }

    SECTION("read_size = chunk_size") {
        read_size = chunk_size;
    }

    SECTION("read_size = chunk_size + 1") {
        read_size = chunk_size + 1;
    }

    SECTION("read_size = 2 * chunk_size") {
        read_size = 2 * chunk_size;
    }

    SECTION("read_size = file_length") {
        read_size = file_length;
    }

    SECTION("read_size > file_length") {
        read_size = file_length + 1;
    }

    bsoncxx::types::b_oid id = {bsoncxx::oid{}};
    std::vector<std::uint8_t> expected =
        manual_gridfs_initialize(db, file_length, chunk_size, bsoncxx::types::value{id});

    // Allocate a buffer large enough to fit the data read from the downloader.
    std::vector<std::uint8_t> buffer;
    buffer.reserve(read_size);

    std::size_t total_bytes_read = 0;
    auto downloader = bucket.open_download_stream(bsoncxx::types::value{id});

    while (std::size_t bytes_read = downloader.read(buffer.data(), read_size)) {
        std::vector<std::uint8_t> expected_bytes{expected.data() + total_bytes_read,
                                                 expected.data() + total_bytes_read + bytes_read};
        std::vector<std::uint8_t> actual_bytes{buffer.data(), buffer.data() + bytes_read};

        REQUIRE(expected_bytes == actual_bytes);

        total_bytes_read += bytes_read;
    }

    REQUIRE(total_bytes_read == static_cast<std::size_t>(file_length));
}

TEST_CASE("mongocxx::gridfs::uploader::abort works", "[gridfs::uploader]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_abort_test"];
    gridfs::bucket bucket = db.gridfs_bucket();

    bsoncxx::types::b_oid id = {bsoncxx::oid{}};
    gridfs::uploader uploader =
        bucket.open_upload_stream_with_id(bsoncxx::types::value{id}, "file");

    // The intended purpose of gridfs::uploader::abort is to clean up any chunks that are already
    // uploaded and to prevent any more data from being written to the uploader. The most
    // straightforward way to test this would be to write enough data to the uploader to ensure that
    // one or more chunks is written and then to call abort. However, in the interest of efficiency,
    // the uploader will only send the chunks to the server either when the amount of data contained
    // in the unset chunks reaches the limit that can fit in a single document or when the user
    // indicates that the file is fully written by calling gridfs::uploader::close. Since it takes a
    // long time to write enough data for the uploader to flush the chunks to the server and calling
    // `close` prevents calling abort afterwards, the easiest way to test abort is to manually
    // insert a chunk with the matching `files_id`, calling abort, and then ensuring that the chunk
    // has been deleted.
    bsoncxx::builder::basic::document document{};
    document.append(bsoncxx::builder::basic::kvp("files_id", id));
    db["fs.chunks"].insert_one(document.extract());

    uploader.abort();
    REQUIRE_THROWS(uploader.close());

    REQUIRE(!db["fs.files"].find_one({}));
    REQUIRE(!db["fs.chunks"].find_one({}));
}

TEST_CASE("mongocxx::gridfs::uploader::write with arbitrary sizes", "[gridfs::uploader]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_write_test"];
    gridfs::bucket bucket = db.gridfs_bucket();

    auto files_coll = db["fs.files"];
    auto chunks_coll = db["fs.chunks"];

    files_coll.delete_many({});
    chunks_coll.delete_many({});

    std::int64_t file_length = 100;
    std::int32_t chunk_size = 9;
    std::int32_t write_size;

    SECTION("write_size = 1") {
        write_size = 1;
    }

    SECTION("write_size = chunk_size - 1") {
        write_size = chunk_size - 1;
    }

    SECTION("write_size = chunk_size") {
        write_size = chunk_size;
    }

    SECTION("write_size = chunk_size + 1") {
        write_size = chunk_size + 1;
    }

    SECTION("write_size = 2 * chunk_size") {
        write_size = 2 * chunk_size;
    }

    SECTION("write_size = file_length") {
        write_size = file_length;
    }

    std::vector<std::uint8_t> bytes;

    // Populate the vector with arbitrary values.
    for (std::size_t i = 0; i < static_cast<std::size_t>(file_length); ++i) {
        bytes.push_back((i + 200) % 256);
    }

    std::size_t bytes_written = 0;
    auto upload_options = options::gridfs::upload{}.chunk_size_bytes(chunk_size);
    auto uploader = bucket.open_upload_stream("test_file", upload_options);

    while (static_cast<std::int64_t>(bytes_written) < file_length) {
        std::size_t actual_write_size = static_cast<std::size_t>(
            std::min(static_cast<std::int64_t>(write_size),
                     file_length - static_cast<std::int64_t>(bytes_written)));

        uploader.write(bytes.data() + bytes_written, actual_write_size);
        bytes_written += actual_write_size;
    }

    auto result = uploader.close();
    auto file_doc = files_coll.find_one(make_document(kvp("_id", result.id())));
    REQUIRE(file_doc);

    REQUIRE(file_doc->view()["length"].get_int64().value ==
            static_cast<std::int64_t>(bytes_written));
    REQUIRE(file_doc->view()["chunkSize"].get_int32().value == chunk_size);

    auto chunks = chunks_coll.find(make_document(kvp("files_id", result.id())));

    std::int32_t chunk_count = 0;
    std::size_t bytes_read = 0;

    for (auto chunk_doc : chunks) {
        REQUIRE(chunk_count == chunk_doc["n"].get_int32().value);
        auto data = chunk_doc["data"].get_binary();

        std::vector<std::uint8_t> expected_bytes{bytes.data() + bytes_read,
                                                 bytes.data() + bytes_read + data.size};
        std::vector<std::uint8_t> actual_bytes{data.bytes, data.bytes + data.size};

        REQUIRE(expected_bytes == actual_bytes);

        bytes_read += data.size;
        ++chunk_count;
    }

    REQUIRE(static_cast<std::int64_t>(bytes_read) == file_length);
    REQUIRE(chunk_count == 12);
}

TEST_CASE("gridfs upload/download round trip", "[gridfs::uploader] [gridfs::downloader]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_download_round_trip_test"];
    gridfs::bucket bucket = db.gridfs_bucket();

    db["fs.files"].delete_many({});
    db["fs.chunks"].delete_many({});

    std::array<std::uint8_t, 100> uploaded_bytes;

    // Initialize array with arbitrary values.
    for (std::uint8_t i = 0; i < 100; ++i) {
        uploaded_bytes[i] = 200 - i;
    }

    std::array<std::uint8_t, 100> downloaded_bytes;

    auto uploader = bucket.open_upload_stream("file");
    uploader.write(uploaded_bytes.data(), 100);
    auto result = uploader.close();

    auto downloader = bucket.open_download_stream(result.id());
    auto bytes_read = downloader.read(downloaded_bytes.data(), 100);
    REQUIRE(bytes_read == 100);

    std::uint8_t c;
    REQUIRE(downloader.read(&c, 1) == 0);

    REQUIRE(uploaded_bytes == downloaded_bytes);
}

TEST_CASE("gridfs::bucket::upload_from_stream doesn't infinite loop when passed bad ifstream",
          "[gridfs::bucket]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_from_stream_no_infinite_loop_ifstream"];
    gridfs::bucket bucket = db.gridfs_bucket();

    std::ifstream stream{"file_that_does_not_exist.txt"};

    SECTION("upload_from_stream") {
        REQUIRE_THROWS(bucket.upload_from_stream("file", &stream));
    }

    SECTION("upload_from_stream_with_id") {
        bsoncxx::types::value id{bsoncxx::types::b_oid{bsoncxx::oid{}}};
        REQUIRE_THROWS(bucket.upload_from_stream_with_id(id, "file", &stream));
    }
}