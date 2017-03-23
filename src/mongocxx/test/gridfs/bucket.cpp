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

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/gridfs/bucket.hpp>
#include <mongocxx/instance.hpp>

using namespace mongocxx;

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
