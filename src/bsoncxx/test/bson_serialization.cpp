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

#include <iostream>
#include <string>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types/bson_value/view.hpp>

using namespace bsoncxx;
using builder::basic::kvp;
using builder::basic::make_array;
using builder::basic::make_document;

namespace {

namespace test {

struct Person {
    std::string first_name;
    std::string last_name;
    int age;
};

void to_bson(const Person& person, builder::basic::document& bson_object) {
    bson_object.append(kvp("first_name", person.first_name),
                       kvp("last_name", person.last_name),
                       kvp("age", person.age));
}

void from_bson(Person& person, const document::view& bson_object) {
    bson_object["first_name"].get_value().to_field(person.first_name);
    bson_object["last_name"].get_value().to_field(person.last_name);
    bson_object["age"].get_value().to_field(person.age);

    // Would it be better to call get_##type() instead of get_value() and then have each
    // bson type struct have their own version of .to_field() ?
}

class Car {
   public:
    Car() = default;
    Car(std::string manufacturer, std::string model)
        : _manufacturer(std::move(manufacturer)), _model(std::move(model)){};

    std::string get_manufacturer() const {
        return _manufacturer;
    }
    void set_manufacturer(std::string manufacturer) {
        this->_manufacturer = manufacturer;
    }

    std::string get_model() const {
        return _model;
    }
    void set_model(std::string model) {
        this->_model = model;
    }

    friend void to_bson(const Car& car, builder::basic::document& bson_object) {
        bson_object.append(kvp("manufacturer", car.get_manufacturer()),
                           // Intentionally change the key name from "model" to "type"
                           kvp("type", car.get_model()));
    }

    friend void from_bson(Car& car, const document::view& bson_object) {
        bson_object["manufacturer"].get_value().to_field(car._manufacturer);
        bson_object["type"].get_value().to_field(car._model);
    }

   private:
    std::string _manufacturer;
    std::string _model;
};

void random_func() {
    // nothing
}

}  // namespace test

TEST_CASE("person works") {
    // We will be using document::value as a representation of BSON objects

    test::Person expected_person{
        "Lelouch", "Lamperouge", 18,
    };

    document::value expected_doc = make_document(kvp("first_name", expected_person.first_name),
                                                 kvp("last_name", expected_person.last_name),
                                                 kvp("age", expected_person.age));

    // Person -> BSON object
    builder::basic::document test_doc = expected_person;
    auto test_doc_view = test_doc.view();

    // BSON object -> Person
    test::Person test_person = expected_doc.get<test::Person>();

    SECTION("Conversion from BSON object to Person struct works") {
        REQUIRE(test_person.first_name == expected_person.first_name);
        REQUIRE(test_person.last_name == expected_person.last_name);
        REQUIRE(test_person.age == expected_person.age);
    }

    SECTION("Checking to see if manual assignment yield same results") {
        // Manual assignment of values
        auto ed_view = expected_doc.view();
        test::Person other_person{ed_view["first_name"].get_string().value.to_string(),
                                  ed_view["last_name"].get_string().value.to_string(),
                                  ed_view["age"].get_int32().value};

        REQUIRE(test_person.first_name == other_person.first_name);
        REQUIRE(test_person.last_name == other_person.last_name);
        REQUIRE(test_person.age == other_person.age);
    }

    SECTION("Conversion from Person struct to BSON object works") {
        REQUIRE(test_doc_view["first_name"].get_string().value.to_string() ==
                expected_person.first_name);
        REQUIRE(test_doc_view["last_name"].get_string().value.to_string() ==
                expected_person.last_name);
        REQUIRE(test_doc_view["age"].get_int32().value == expected_person.age);

        // REQUIRE(test_doc_view == expected_doc.view());
    }
    // Add test cases for nonexistent elements/keys, arrays, nested objects, more
}

TEST_CASE("Car works") {
    test::Car expected_car{"Tesla", "Model S"};

    document::value expected_doc =
        make_document(kvp("type", expected_car.get_model()),
                      kvp("manufacturer", expected_car.get_manufacturer()));

    // Car -> BSON
    builder::basic::document test_doc = expected_car;
    auto doc_view = test_doc.view();

    // BSON -> Car
    test::Car test_car = expected_doc.get<test::Car>();

    SECTION("Car to BSON works") {
        REQUIRE(doc_view["type"].get_string().value.to_string() == expected_car.get_model());
        REQUIRE(doc_view["manufacturer"].get_string().value.to_string() ==
                expected_car.get_manufacturer());
    }

    SECTION("BSON to Car works") {
        REQUIRE(test_car.get_manufacturer() == expected_car.get_manufacturer());
        REQUIRE(test_car.get_model() == expected_car.get_model());
    }
}
}