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

#include <cstdint>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/test_util/catch.hh>
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

TEST_CASE("mongocxx::gridfs::uploader default constructor makes invalid uploader",
          "[gridfs::uploader]") {
    gridfs::uploader uploader;
    REQUIRE(!uploader);
    std::uint8_t c = 0x0;
    REQUIRE_THROWS_AS(uploader.write(1, &c), logic_error);
}

TEST_CASE("mongocxx::gridfs::uploader::abort works", "[gridfs::uploader]") {
    instance::current();

    client client{uri{}};
    database db = client["gridfs_upload_abort_test"];
    gridfs::bucket bucket{db};

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
