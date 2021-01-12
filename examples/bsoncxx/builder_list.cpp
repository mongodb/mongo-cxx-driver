// Copyright 2020 MongoDB Inc.
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

#include <chrono>

#include <bsoncxx/builder/list/array.hpp>
#include <bsoncxx/builder/list/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

using namespace bsoncxx;

int main(int, char**) {
    using namespace bsoncxx::builder;

    //
    // bsoncxx::builder::list::document and bsoncxx::builder::list::array provides a
    // JSON-like interface for creating BSON objects.
    //
    // builder::list::document builds an empty BSON document
    builder::list::document doc = {};
    // builder::list::array builds an empty BSON array
    builder::list::array arr = {};

    //
    // We can append values to a document using an initializer list of key-value pairs.
    // { "hello" : "world" }
    //
    doc = {"hello", "world"};

    //
    //  Each document key must be a string type.
    //
    // {
    //   "c-style" : "with value",
    //   "basic string" : "with value"
    // }
    //
    doc = {{"c-style", "with value"}, {std::string("basic string"), "with value"}};

    //
    // Each document value must be a bson_value::value or implicitly convertible to one.
    //
    // {
    //   "BSON boolean value" : false,
    //   "BSON 32-bit signed integer value" : -123,
    //   "BSON date value" : { "$date" : 123456789 },
    //   "BSON Decimal128 value" : { "$numberDecimal" : "1.844674407370955161800E-6155" },
    //   "BSON regex value with options" : { "$regex" : "any", "$options" : "imsx" }
    //  }
    //
    // clang-format off
    doc = {{"BSON boolean value", false},
           {"BSON 32-bit signed integer value", -123}};

    doc += {{"BSON date value", std::chrono::milliseconds(123456789)},
           {"BSON Decimal128 value", decimal128{100, 200}},
           {"BSON regex value with options", types::bson_value::value("regex", "imsx" /* opts */)}};
    // clang-format on

    //
    // Nested documents can be added in-place.
    //
    // { "Answers" :
    //      { "Everything" :
    //          { "The Universe" : { "Life" : 42 } }
    //      }
    // }
    //
    doc = {"Answers", {"Everything", {"The Universe", {"Life", 42}}}};

    //
    // Each array element must be bson_value::value or implicitly convertible to one.
    //
    // { "0" : false,
    //   "1" : -123,
    //   "2" : { "$date" : 123456789 },
    //   "3" : { "$numberDecimal" : "1.844674407370955161800E-6155" },
    //   "4" : { "$regex" : "any", "$options" : "imsx" }
    // }
    //
    arr = {false, -123};
    arr += std::chrono::milliseconds(123456789);
    arr += decimal128{100, 200};
    arr += types::bson_value::value("regex", "imsx" /* opts */);
}
