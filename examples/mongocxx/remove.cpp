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

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["test"];

    // Remove all documents that match a condition.
    {
        // @begin: cpp-remove-matching-documents
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "borough"
                       << "Manhattan";

        db["restaurants"].delete_many(filter_builder.view());
        // @end: cpp-remove-matching-documents
    }

    // Remove one document that matches a condition.
    {
        // @begin: cpp-remove-justone
        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "borough"
                       << "Queens";

        db["restaurants"].delete_one(filter_builder.view());
        // @end: cpp-remove-justone
    }

    // Remove all documents in a collection.
    {
        // @begin: cpp-remove-all-documents
        db["restaurants"].delete_many({});
        // @end: cpp-remove-all-documents
    }

    // Drop a collection.
    {
        // @begin: cpp-drop-collection
        db["restaurants"].drop();
        // @end: cpp-drop-collection
    }
}
