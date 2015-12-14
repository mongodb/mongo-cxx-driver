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

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};
    auto db = conn["test"];

    // Many driver methods take a bsoncxx::document::view_or_value as
    // a parameter. You can use these methods with either a document::view
    // or a document::value.

    // Document::views can be passed in directly:
    {
        // @begin: pass-view-to-view-or-value
        bsoncxx::document::value command = document{} << "ping" << 1 << finalize;
        bsoncxx::document::view command_view = command.view();
        auto res = db.run_command(command_view);
        // @end: pass-view-to-view-or-value
    }

    // Document::values can be passed in one of two ways:

    // 1. Pass a view of the document::value
    {
        // @begin: pass-viewed-value-to-view-or-value
        bsoncxx::document::value command = document{} << "ping" << 1 << finalize;
        auto res = db.run_command(command.view());
        // @end: pass-viewed-value-to-view-or-value
    }

    // 2. Pass ownership of the document::value into the method
    {
        // @begin: pass-owned-value-to-view-or-value
        bsoncxx::document::value command = document{} << "ping" << 1 << finalize;
        auto res = db.run_command(std::move(command));
        // @end: pass-owned-value-to-view-or-value
    }

    // Temporary document::values are captured and owned by the view_or_value type
    {
        // @begin: pass-temporary-values-to-view-or-value
        auto res = db.run_command(document{} << "ping" << 1 << finalize);

        // NOTE: there is no need to call .view() on a temporary document::value in
        // a call like this, and doing so could result in a use-after-free error.
        // BAD:
        // auto res = db.run_command((document{} << "ping" << 1 << finalize).view());

        // @end: pass-temporary-values-to-view-or-value
    }
}
