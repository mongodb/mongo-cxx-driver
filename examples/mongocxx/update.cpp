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
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto db = conn["test"];

    // Update top-level fields in a single document.
    {
        // @begin: cpp-update-top-level-fields
        bsoncxx::builder::stream::document filter_builder, update_builder;
        filter_builder << "name"
                       << "Juni";
        update_builder << "$set" << open_document << "cuisine"
                       << "American (New)" << close_document << "$currentDate" << open_document
                       << "lastModified" << true << close_document;

        db["restaurants"].update_one(filter_builder.view(), update_builder.view());
        // @end: cpp-update-top-level-fields
    }

    // Update an embedded document in a single document.
    {
        // @begin: cpp-update-embedded-field
        bsoncxx::builder::stream::document filter_builder, update_builder;
        filter_builder << "restaurant_id"
                       << "41156888";
        update_builder << "$set" << open_document << "address.street"
                       << "East 31st Street" << close_document;

        db["restaurants"].update_one(filter_builder.view(), update_builder.view());
        // @end: cpp-update-embedded-field
    }

    // Update multiple documents.
    {
        // @begin: cpp-update-multiple-documents
        bsoncxx::builder::stream::document filter_builder, update_builder;
        filter_builder << "address.zipcode"
                       << "10016"
                       << "cuisine"
                       << "Other";
        update_builder << "$set" << open_document << "cuisine"
                       << "Category To Be Determined" << close_document << "$currentDate"
                       << open_document << "lastModified" << true << close_document;

        db["restaurants"].update_many(filter_builder.view(), update_builder.view());
        // @end: cpp-update-multiple-documents
    }

    // Replace the contents of a single document.
    {
        // @begin: cpp-replace-document
        bsoncxx::builder::stream::document filter_builder, replace_builder;
        filter_builder << "restaurant_id"
                       << "41704620";
        replace_builder << "name"
                        << "Vella 2"
                        << "address" << open_document << "coord" << open_array << -73.9557413
                        << 40.7720266 << close_array << "building"
                        << "1480"
                        << "street"
                        << "2 Avenue"
                        << "zipcode"
                        << "10075" << close_document;

        db["restaurants"].replace_one(filter_builder.view(), replace_builder.view());
        // @end: cpp-replace-document
    }
}
