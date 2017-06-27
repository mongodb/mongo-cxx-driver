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

#include "../microbench.hpp"

#include <vector>

#include <mongocxx/client.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace benchmark {

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

class gridfs_upload : public microbench {
   public:
    gridfs_upload() : microbench{1}, _conn{mongocxx::uri{}} {}

    void setup(bsoncxx::stdx::string_view);

    void before_task();

    void teardown();

   protected:
    void task();

   private:
    mongocxx::client _conn;
    mongocxx::gridfs::bucket _bucket;
    std::vector<std::uint8_t> _gridfs_file;
};

void gridfs_upload::setup(bsoncxx::stdx::string_view file_name) {
    std::ifstream stream{file_name.to_string()};
    stream >> std::noskipws;
    _gridfs_file = std::vector<std::uint8_t>{(std::istream_iterator<unsigned char>{stream}),
                                             (std::istream_iterator<unsigned char>{})};
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void gridfs_upload::before_task() {
    auto db = _conn["perftest"];
    _bucket = db.gridfs_bucket();
    db[_bucket.bucket_name().to_string() + ".chunks"].drop();
    db[_bucket.bucket_name().to_string() + ".files"].drop();
    auto uploader = _bucket.open_upload_stream("one_byte_gridfs_file");
    std::uint8_t byte[1] = {72};
    uploader.write(byte, 1);
    uploader.close();
}

void gridfs_upload::teardown() {
    mongocxx::database db = _conn["perftest"];
    db.drop();
}

void gridfs_upload::task() {
    auto uploader = _bucket.open_upload_stream("actual_file");
    uploader.write(_gridfs_file.data(), _gridfs_file.size());
    uploader.close();
}
}