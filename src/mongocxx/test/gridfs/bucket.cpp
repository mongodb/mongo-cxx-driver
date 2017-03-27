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

    while (std::size_t bytes_read = downloader.read(read_size, buffer.data())) {
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

        uploader.write(actual_write_size, bytes.data() + bytes_written);
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
    uploader.write(100, uploaded_bytes.data());
    auto result = uploader.close();

    auto downloader = bucket.open_download_stream(result.id());
    auto bytes_read = downloader.read(100, downloaded_bytes.data());
    REQUIRE(bytes_read == 100);

    std::uint8_t c;
    REQUIRE(downloader.read(1, &c) == 0);

    REQUIRE(uploaded_bytes == downloaded_bytes);
}
