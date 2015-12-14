// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/builder/stream/document.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["test"];
    try {
        db["restaurants"].drop();
    } catch (const std::exception&) {
        // Collection did not exist.
    }

    // Create a single field index.
    {
        // @begin: cpp-single-field-index
        bsoncxx::builder::stream::document index_builder;
        index_builder << "cuisine" << 1;
        db["restaurants"].create_index(index_builder.view(), {});
        // @end: cpp-single-field-index
    }

    // Create a compound index.
    {
        db["restaurants"].drop();
        // @begin: cpp-create-compound-index
        bsoncxx::builder::stream::document index_builder;
        index_builder << "cuisine" << 1 << "address.zipcode" << -1;
        db["restaurants"].create_index(index_builder.view(), {});
        // @end: cpp-create-compound-index
    }

    // Create a unique index.
    {
        db["restaurants"].drop();
        // @begin: cpp-create-unique-index
        bsoncxx::builder::stream::document index_builder;
        mongocxx::options::index index_options{};
        index_builder << "website" << 1;
        index_options.unique(true);
        db["restaurants"].create_index(index_builder.view(), index_options);
        // @end: cpp-create-unique-index
    }

    // Create an index with storage engine options
    {
        db["restaurants"].drop();
        // @begin: cpp-create-wt-options-index
        bsoncxx::builder::stream::document index_builder;
        mongocxx::options::index index_options{};
        std::unique_ptr<mongocxx::options::index::wiredtiger_storage_options> wt_options =
            mongocxx::stdx::make_unique<mongocxx::options::index::wiredtiger_storage_options>();
        index_builder << "cuisine" << 1;
        wt_options->config_string("block_allocation=first");
        index_options.storage_options(std::move(wt_options));
        db["restaurants"].create_index(index_builder.view(), index_options);
        // @begin: cpp-create-wt-options-index
    }
}
