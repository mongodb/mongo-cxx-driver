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

#include "helpers.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/gridfs/upload.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

namespace {
using namespace bsoncxx;
using namespace mongocxx;

using bsoncxx::builder::basic::kvp;

TEST_CASE("options::gridfs::upload accessors/mutators", "[options::gridfs::upload]") {
    instance::current();

    options::gridfs::upload upload_options;

    auto document = builder::basic::make_document(kvp("foo", 1));

    CHECK_OPTIONAL_ARGUMENT(upload_options, chunk_size_bytes, 100);
    CHECK_OPTIONAL_ARGUMENT(upload_options, metadata, document.view());
}
}  // namespace
