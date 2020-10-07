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

#include <string>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/view.hpp>

namespace {
using namespace bsoncxx;

using builder::basic::kvp;
using builder::basic::make_document;
using string::to_string;

namespace test {

struct Person {
    std::string first_name;
    std::string last_name;
    int age;
};

// to_bson serializer function that can take a document::value&
void to_bson(const Person& person, bsoncxx::document::value& bson_object) {
    bson_object = make_document(kvp("first_name", person.first_name),
                                kvp("last_name", person.last_name),
                                kvp("age", person.age));
}

void from_bson(Person& person, const bsoncxx::document::view& bson_object) {
    person.first_name = to_string(bson_object["first_name"].get_string().value);
    person.last_name = to_string(bson_object["last_name"].get_string().value);
    person.age = bson_object["age"].get_int32().value;
}
}  // namespace test

TEST_CASE("Convert between Person struct and BSON object") {
    test::Person expected_person{
        "Lelouch", "Lamperouge", 18,
    };

    bsoncxx::document::value expected_doc =
        make_document(kvp("first_name", expected_person.first_name),
                      kvp("last_name", expected_person.last_name),
                      kvp("age", expected_person.age));
    auto expected_view = expected_doc.view();

    SECTION("Conversion from BSON object to Person struct works") {
        // BSON object -> Person
        test::Person test_person = expected_doc.get<test::Person>();

        REQUIRE(test_person.first_name == expected_person.first_name);
        REQUIRE(test_person.last_name == expected_person.last_name);
        REQUIRE(test_person.age == expected_person.age);
    }

    SECTION("Conversion from Person struct to document::value works") {
        // Person -> BSON object
        bsoncxx::document::value test_value{expected_person};
        auto test_view = test_value.view();

        REQUIRE(to_string(test_view["first_name"].get_string().value) ==
                expected_person.first_name);
        REQUIRE(to_string(test_view["last_name"].get_string().value) == expected_person.last_name);
        REQUIRE(test_view["age"].get_int32() == expected_person.age);
        REQUIRE(test_view == expected_view);
    }
}
}  // namespace
