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

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/uri.hpp>

// NOTE: Any time this file is modified, a DOCS ticket should be opened to sync the changes with the
// corresponding page on docs.mongodb.com. See CXX-1249 and DRIVERS-356 for more info.

template <typename T>
void check_field(const T& document, const char* field, bool should_have, int example_no) {
    if (should_have) {
        if (!document[field]) {
            throw std::logic_error(std::string("document in example ") +
                                   std::to_string(example_no) + " should have field " + field);
        }
    } else {
        if (document[field]) {
            throw std::logic_error(std::string("document in example ") +
                                   std::to_string(example_no) + " should not have field " + field);
        }
    }
}

template <typename T>
void check_has_field(const T& document, const char* field, int example_no) {
    check_field(document, field, true, example_no);
}

template <typename T>
void check_has_no_field(const T& document, const char* field, int example_no) {
    check_field(document, field, false, example_no);
}

static bsoncxx::document::value get_is_master(const mongocxx::client& client) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    static auto reply = client["admin"].run_command(make_document(kvp("isMaster", 1)));
    return reply;
}

static bool is_replica_set(const mongocxx::client& client) {
    auto reply = get_is_master(client);
    return static_cast<bool>(reply.view()["setName"]);
}

void insert_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 1
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].insert_one(make_document(
            kvp("item", "canvas"),
            kvp("qty", 100),
            kvp("tags", make_array("cotton")),
            kvp("size", make_document(kvp("h", 28), kvp("w", 35.5), kvp("uom", "cm")))));
        // End Example 1

        if (db["inventory"].count_documents({}) != 1) {
            throw std::logic_error("wrong count in example 1");
        }
    }

    {
        // Start Example 2
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("item", "canvas")));
        // End Example 2

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 2");
        }
    }

    {
        // Start Example 3
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("tags", make_array("blank", "red")),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm")))),
            make_document(
                kvp("item", "mat"),
                kvp("qty", 85),
                kvp("tags", make_array("gray")),
                kvp("size", make_document(kvp("h", 27.9), kvp("w", 35.5), kvp("uom", "cm")))),
            make_document(
                kvp("item", "mousepad"),
                kvp("qty", 25),
                kvp("tags", make_array("gel", "blue")),
                kvp("size", make_document(kvp("h", 19), kvp("w", 22.85), kvp("uom", "cm")))),
        };

        db["inventory"].insert_many(docs);
        // End Example 3

        if (db["inventory"].count_documents({}) != 4) {
            throw std::logic_error("wrong count in example 3");
        }
    }
}

void query_top_level_fields_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 6
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                          kvp("status", "A")),
            make_document(kvp("item", "notebook"),
                          kvp("qty", 50),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "A")),
            make_document(kvp("item", "paper"),
                          kvp("qty", 100),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "D")),
            make_document(
                kvp("item", "planner"),
                kvp("qty", 75),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30), kvp("uom", "cm"))),
                kvp("status", "D")),
            make_document(
                kvp("item", "postcard"),
                kvp("qty", 45),
                kvp("size", make_document(kvp("h", 10), kvp("w", 15.25), kvp("uom", "cm"))),
                kvp("status", "A")),
        };

        db["inventory"].insert_many(docs);
        // End Example 6

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 6");
        }
    }

    {
        // Start Example 7
        auto cursor = db["inventory"].find({});
        // End Example 7

        if (std::distance(cursor.begin(), cursor.end()) != 5) {
            throw std::logic_error("wrong count in example 7");
        }
    }

    {
        // Start Example 9
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("status", "D")));
        // End Example 9

        if (std::distance(cursor.begin(), cursor.end()) != 2) {
            throw std::logic_error("wrong count in example 9");
        }
    }

    {
        // Start Example 10
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("status", make_document(kvp("$in", make_array("A", "D"))))));
        // End Example 10

        if (std::distance(cursor.begin(), cursor.end()) != 5) {
            throw std::logic_error("wrong count in example 10");
        }
    }

    {
        // Start Example 11
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("status", "A"), kvp("qty", make_document(kvp("$lt", 30)))));
        // End Example 11

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 11");
        }
    }

    {
        // Start Example 12
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("$or",
                make_array(make_document(kvp("status", "A")),
                           make_document(kvp("qty", make_document(kvp("$lt", 30))))))));
        // End Example 12

        if (std::distance(cursor.begin(), cursor.end()) != 3) {
            throw std::logic_error("wrong count in example 12");
        }
    }

    {
        // Start Example 13
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("status", "A"),
            kvp("$or",
                make_array(make_document(kvp("qty", make_document(kvp("$lt", 30)))),
                           make_document(kvp("item", bsoncxx::types::b_regex{"^p"}))))));
        // End Example 13

        if (std::distance(cursor.begin(), cursor.end()) != 2) {
            throw std::logic_error("wrong count in example 13");
        }
    }
}

void query_embedded_documents_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 14
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                          kvp("status", "A")),
            make_document(kvp("item", "notebook"),
                          kvp("qty", 50),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "A")),
            make_document(kvp("item", "paper"),
                          kvp("qty", 100),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "D")),
            make_document(
                kvp("item", "planner"),
                kvp("qty", 75),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30), kvp("uom", "cm"))),
                kvp("status", "D")),
            make_document(
                kvp("item", "postcard"),
                kvp("qty", 45),
                kvp("size", make_document(kvp("h", 10), kvp("w", 15.25), kvp("uom", "cm"))),
                kvp("status", "A")),
        };

        db["inventory"].insert_many(docs);
        // End Example 14

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 14");
        }
    }

    {
        // Start Example 15
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm")))));
        // End Example 15

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 15");
        }
    }

    {
        // Start Example 16
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("size", make_document(kvp("w", 21), kvp("h", 14), kvp("uom", "cm")))));
        // End Example 16

        if (std::distance(cursor.begin(), cursor.end()) != 0) {
            throw std::logic_error("wrong count in example 16");
        }
    }

    {
        // Start Example 17
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("size.uom", "in")));
        // End Example 17

        if (std::distance(cursor.begin(), cursor.end()) != 2) {
            throw std::logic_error("wrong count in example 17");
        }
    }

    {
        // Start Example 18
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("size.h", make_document(kvp("$lt", 15)))));
        // End Example 18

        if (std::distance(cursor.begin(), cursor.end()) != 4) {
            throw std::logic_error("wrong count in example 18");
        }
    }

    {
        // Start Example 19
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("size.h", make_document(kvp("$lt", 15))),
                                               kvp("size.uom", "in"),
                                               kvp("status", "D")));
        // End Example 19

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 19");
        }
    }
}

void query_arrays_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 20
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("tags", make_array("blank", "red")),
                          kvp("dim_cm", make_array(14, 21))),
            make_document(kvp("item", "notebook"),
                          kvp("qty", 50),
                          kvp("tags", make_array("red", "blank")),
                          kvp("dim_cm", make_array(14, 21))),
            make_document(kvp("item", "paper"),
                          kvp("qty", 100),
                          kvp("tags", make_array("red", "blank", "plain")),
                          kvp("dim_cm", make_array(14, 21))),
            make_document(kvp("item", "planner"),
                          kvp("qty", 75),
                          kvp("tags", make_array("blank", "red")),
                          kvp("dim_cm", make_array(22.85, 30))),
            make_document(kvp("item", "postcard"),
                          kvp("qty", 45),
                          kvp("tags", make_array("blue")),
                          kvp("dim_cm", make_array(10, 15.25))),
        };

        db["inventory"].insert_many(docs);
        // End Example 20

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 20");
        }
    }

    {
        // Start Example 21
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("tags", make_array("red", "blank"))));
        // End Example 21

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 21");
        }
    }

    {
        // Start Example 22
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("tags", make_document(kvp("$all", make_array("red", "blank"))))));
        // End Example 22

        if (std::distance(cursor.begin(), cursor.end()) != 4) {
            throw std::logic_error("wrong count in example 22");
        }
    }

    {
        // Start Example 23
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("tags", "red")));
        // End Example 23

        if (std::distance(cursor.begin(), cursor.end()) != 4) {
            throw std::logic_error("wrong count in example 23");
        }
    }

    {
        // Start Example 24
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("dim_cm", make_document(kvp("$gt", 25)))));
        // End Example 24

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 24");
        }
    }

    {
        // Start Example 25
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("dim_cm", make_document(kvp("$gt", 15), kvp("$lt", 20)))));
        // End Example 25

        if (std::distance(cursor.begin(), cursor.end()) != 4) {
            throw std::logic_error("wrong count in example 25");
        }
    }

    {
        // Start Example 26
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("dim_cm",
                make_document(kvp("$elemMatch", make_document(kvp("$gt", 22), kvp("$lt", 30)))))));
        // End Example 26

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 26");
        }
    }

    {
        // Start Example 27
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("dim_cm.1", make_document(kvp("$gt", 25)))));
        // End Example 27

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 27");
        }
    }

    {
        // Start Example 28
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("tags", make_document(kvp("$size", 3)))));
        // End Example 28

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 28");
        }
    }
}

void query_array_embedded_documents_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 29
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("instock",
                              make_array(make_document(kvp("warehouse", "A"), kvp("qty", 5)),
                                         make_document(kvp("warehouse", "C"), kvp("qty", 15))))),
            make_document(
                kvp("item", "notebook"),
                kvp("instock", make_array(make_document(kvp("warehouse", "C"), kvp("qty", 5))))),
            make_document(kvp("item", "paper"),
                          kvp("instock",
                              make_array(make_document(kvp("warehouse", "A"), kvp("qty", 60)),
                                         make_document(kvp("warehouse", "B"), kvp("qty", 15))))),
            make_document(kvp("item", "planner"),
                          kvp("instock",
                              make_array(make_document(kvp("warehouse", "A"), kvp("qty", 40)),
                                         make_document(kvp("warehouse", "B"), kvp("qty", 5))))),
            make_document(kvp("item", "postcard"),
                          kvp("instock",
                              make_array(make_document(kvp("warehouse", "B"), kvp("qty", 15)),
                                         make_document(kvp("warehouse", "C"), kvp("qty", 35))))),
        };

        db["inventory"].insert_many(docs);
        // End Example 29

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 29");
        }
    }

    {
        // Start Example 30
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("instock", make_document(kvp("warehouse", "A"), kvp("qty", 5)))));
        // End Example 30

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 30");
        }
    }

    {
        // Start Example 31
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("instock", make_document(kvp("qty", 5), kvp("warehouse", "A")))));
        // End Example 31

        if (std::distance(cursor.begin(), cursor.end()) != 0) {
            throw std::logic_error("wrong count in example 31");
        }
    }

    {
        // Start Example 32
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("instock.0.qty", make_document(kvp("$lte", 20)))));
        // End Example 32

        if (std::distance(cursor.begin(), cursor.end()) != 3) {
            throw std::logic_error("wrong count in example 32");
        }
    }

    {
        // Start Example 33
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("instock.qty", make_document(kvp("$lte", 20)))));
        // End Example 33

        if (std::distance(cursor.begin(), cursor.end()) != 5) {
            throw std::logic_error("wrong count in example 33");
        }
    }

    {
        // Start Example 34
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("instock",
                make_document(
                    kvp("$elemMatch", make_document(kvp("qty", 5), kvp("warehouse", "A")))))));
        // End Example 34

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 34");
        }
    }

    {
        // Start Example 35
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(
            kvp("instock",
                make_document(kvp(
                    "$elemMatch",
                    make_document(kvp("qty", make_document(kvp("$gt", 10), kvp("$lte", 20)))))))));
        // End Example 35

        if (std::distance(cursor.begin(), cursor.end()) != 3) {
            throw std::logic_error("wrong count in example 35");
        }
    }

    {
        // Start Example 36
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("instock.qty", make_document(kvp("$gt", 10), kvp("$lte", 20)))));
        // End Example 36

        if (std::distance(cursor.begin(), cursor.end()) != 4) {
            throw std::logic_error("wrong count in example 36");
        }
    }

    {
        // Start Example 37
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("instock.qty", 5), kvp("instock.warehouse", "A")));
        // End Example 37

        if (std::distance(cursor.begin(), cursor.end()) != 2) {
            throw std::logic_error("wrong count in example 37");
        }
    }
}

void query_null_missing_fields_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 38
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("_id", 1), kvp("item", bsoncxx::types::b_null{})),
            make_document(kvp("_id", 2)),
        };

        db["inventory"].insert_many(docs);
        // End Example 38

        if (db["inventory"].count_documents({}) != 2) {
            throw std::logic_error("wrong count in example 38");
        }
    }

    {
        // Start Example 39
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("item", bsoncxx::types::b_null{})));
        // End Example 39

        if (std::distance(cursor.begin(), cursor.end()) != 2) {
            throw std::logic_error("wrong count in example 39");
        }
    }

    {
        // Start Example 40
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("item", make_document(kvp("$type", 10)))));
        // End Example 40

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 40");
        }
    }

    {
        // Start Example 41
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("item", make_document(kvp("$exists", false)))));
        // End Example 41

        if (std::distance(cursor.begin(), cursor.end()) != 1) {
            throw std::logic_error("wrong count in example 41");
        }
    }
}

void projection_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 42
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(
                kvp("item", "journal"),
                kvp("status", "A"),
                kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                kvp("instock", make_array(make_document(kvp("warehouse", "A"), kvp("qty", 5))))),
            make_document(
                kvp("item", "notebook"),
                kvp("status", "A"),
                kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                kvp("instock", make_array(make_document(kvp("warehouse", "C"), kvp("qty", 5))))),
            make_document(
                kvp("item", "paper"),
                kvp("status", "D"),
                kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                kvp("instock", make_array(make_document(kvp("warehouse", "A"), kvp("qty", 60))))),
            make_document(
                kvp("item", "planner"),
                kvp("status", "D"),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30), kvp("uom", "cm"))),
                kvp("instock", make_array(make_document(kvp("warehouse", "A"), kvp("qty", 40))))),
            make_document(
                kvp("item", "postcard"),
                kvp("status", "A"),
                kvp("size", make_document(kvp("h", 10), kvp("w", 15.25), kvp("uom", "cm"))),
                kvp("instock",
                    make_array(make_document(kvp("warehouse", "B"), kvp("qty", 15)),
                               make_document(kvp("warehouse", "C"), kvp("qty", 35))))),
        };

        db["inventory"].insert_many(docs);
        // End Example 42

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 42");
        }
    }

    {
        // Start Example 43
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("status", "A")));
        // End Example 43

        if (std::distance(cursor.begin(), cursor.end()) != 3) {
            throw std::logic_error("wrong count in example 43");
        }
    }

    {
        // Start Example 44
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("status", "A")),
            mongocxx::options::find{}.projection(make_document(kvp("item", 1), kvp("status", 1))));
        // End Example 44

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 44);
            check_has_field(document, "item", 44);
            check_has_field(document, "status", 44);
            check_has_no_field(document, "size", 44);
            check_has_no_field(document, "instock", 44);
        }
    }

    {
        // Start Example 45
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("status", "A")),
                                           mongocxx::options::find{}.projection(make_document(
                                               kvp("item", 1), kvp("status", 1), kvp("_id", 0))));
        // End Example 45

        for (auto&& document : cursor) {
            check_has_no_field(document, "_id", 45);
            check_has_field(document, "item", 45);
            check_has_field(document, "status", 45);
            check_has_no_field(document, "size", 45);
            check_has_no_field(document, "instock", 45);
        }
    }

    {
        // Start Example 46
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("status", "A")),
                                           mongocxx::options::find{}.projection(
                                               make_document(kvp("status", 0), kvp("instock", 0))));
        // End Example 46

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 46);
            check_has_field(document, "item", 46);
            check_has_no_field(document, "status", 46);
            check_has_field(document, "size", 46);
            check_has_no_field(document, "instock", 46);
        }
    }

    {
        // Start Example 47
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("status", "A")),
                                 mongocxx::options::find{}.projection(make_document(
                                     kvp("item", 1), kvp("status", 1), kvp("size.uom", 1))));
        // End Example 47

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 47);
            check_has_field(document, "item", 47);
            check_has_field(document, "status", 47);
            check_has_field(document, "size", 47);
            check_has_no_field(document, "instock", 47);

            auto size = document["size"].get_document().value;

            check_has_field(size, "uom", 47);
            check_has_no_field(size, "h", 47);
            check_has_no_field(size, "w", 47);
        }
    }

    {
        // Start Example 48
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(
            make_document(kvp("status", "A")),
            mongocxx::options::find{}.projection(make_document(kvp("size.uom", 0))));
        // End Example 48

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 48);
            check_has_field(document, "item", 48);
            check_has_field(document, "status", 48);
            check_has_field(document, "size", 48);
            check_has_field(document, "instock", 48);

            auto size = document["size"].get_document().value;

            check_has_no_field(size, "uom", 48);
            check_has_field(size, "h", 48);
            check_has_field(size, "w", 48);
        }
    }

    {
        // Start Example 49
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor =
            db["inventory"].find(make_document(kvp("status", "A")),
                                 mongocxx::options::find{}.projection(make_document(
                                     kvp("item", 1), kvp("status", 1), kvp("instock.qty", 1))));
        // End Example 49

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 49);
            check_has_field(document, "item", 49);
            check_has_field(document, "status", 49);
            check_has_no_field(document, "size", 49);
            check_has_field(document, "instock", 49);

            auto instock = document["instock"].get_array().value;

            for (auto&& sub_document : instock) {
                check_has_no_field(sub_document, "warehouse", 49);
                check_has_field(sub_document, "qty", 49);
            }
        }
    }

    {
        // Start Example 50
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        auto cursor = db["inventory"].find(make_document(kvp("status", "A")),
                                           mongocxx::options::find{}.projection(make_document(
                                               kvp("item", 1),
                                               kvp("status", 1),
                                               kvp("instock", make_document(kvp("$slice", -1))))));
        // End Example 50

        for (auto&& document : cursor) {
            check_has_field(document, "_id", 50);
            check_has_field(document, "item", 50);
            check_has_field(document, "status", 50);
            check_has_no_field(document, "size", 50);
            check_has_field(document, "instock", 50);

            auto instock = document["instock"].get_array().value;

            if (std::distance(instock.begin(), instock.end()) != 1) {
                throw std::logic_error("wrong count in example 50");
            }
        }
    }
}

void update_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 51
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(
                kvp("item", "canvas"),
                kvp("qty", 100),
                kvp("size", make_document(kvp("h", 28), kvp("w", 35.5), kvp("uom", "cm"))),
                kvp("status", "A")),
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                          kvp("status", "A")),
            make_document(
                kvp("item", "mat"),
                kvp("qty", 85),
                kvp("size", make_document(kvp("h", 27.9), kvp("w", 35.5), kvp("uom", "cm"))),
                kvp("status", "A")),
            make_document(
                kvp("item", "mousepad"),
                kvp("qty", 25),
                kvp("size", make_document(kvp("h", 19), kvp("w", 22.85), kvp("uom", "cm"))),
                kvp("status", "P")),
            make_document(kvp("item", "notebook"),
                          kvp("qty", 50),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "P")),
            make_document(kvp("item", "paper"),
                          kvp("qty", 100),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "D")),
            make_document(
                kvp("item", "planner"),
                kvp("qty", 75),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30), kvp("uom", "cm"))),
                kvp("status", "D")),
            make_document(
                kvp("item", "postcard"),
                kvp("qty", 45),
                kvp("size", make_document(kvp("h", 10), kvp("w", 15.25), kvp("uom", "cm"))),
                kvp("status", "A")),
            make_document(kvp("item", "sketchbook"),
                          kvp("qty", 80),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                          kvp("status", "A")),
            make_document(
                kvp("item", "sketch pad"),
                kvp("qty", 95),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30.5), kvp("uom", "cm"))),
                kvp("status", "A")),
        };

        db["inventory"].insert_many(docs);
        // End Example 51

        if (db["inventory"].count_documents({}) != 10) {
            throw std::logic_error("wrong count in example 51");
        }
    }

    {
        // Start Example 52
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].update_one(
            make_document(kvp("item", "paper")),
            make_document(kvp("$set", make_document(kvp("size.uom", "cm"), kvp("status", "P"))),
                          kvp("$currentDate", make_document(kvp("lastModified", true)))));
        // End Example 52

        for (auto&& document : db["inventory"].find(make_document(kvp("item", "paper")))) {
            if (document["size"].get_document().value["uom"].get_string().value !=
                bsoncxx::stdx::string_view{"cm"}) {
                throw std::logic_error("error in example 52");
            }
            if (document["status"].get_string().value != bsoncxx::stdx::string_view{"P"}) {
                throw std::logic_error("error in example 52");
            }
            check_has_field(document, "lastModified", 52);
        }
    }

    {
        // Start Example 53
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].update_many(
            make_document(kvp("qty", make_document((kvp("$lt", 50))))),
            make_document(kvp("$set", make_document(kvp("size.uom", "in"), kvp("status", "P"))),
                          kvp("$currentDate", make_document(kvp("lastModified", true)))));
        // End Example 53

        for (auto&& document :
             db["inventory"].find(make_document(kvp("qty", make_document(kvp("$lt", 50)))))) {
            if (document["size"].get_document().value["uom"].get_string().value !=
                bsoncxx::stdx::string_view{"in"}) {
                throw std::logic_error("error in example 53");
            }
            if (document["status"].get_string().value != bsoncxx::stdx::string_view{"P"}) {
                throw std::logic_error("error in example 53");
            }
            check_has_field(document, "lastModified", 53);
        }
    }

    {
        // Start Example 54
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_array;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].replace_one(
            make_document(kvp("item", "paper")),
            make_document(kvp("item", "paper"),
                          kvp("instock",
                              make_array(make_document(kvp("warehouse", "A"), kvp("qty", 60)),
                                         make_document(kvp("warehouse", "B"), kvp("qty", 40))))));
        // End Example 54

        for (auto&& document : db["inventory"].find(make_document(kvp("item", "paper")))) {
            if (std::distance(document.begin(), document.end()) != 3) {
                throw std::logic_error("wrong count in example 54");
            }
            check_has_field(document, "_id", 54);
            check_has_field(document, "item", 54);
            check_has_field(document, "instock", 54);

            auto instock = document["instock"].get_array().value;

            if (std::distance(instock.begin(), instock.end()) != 2) {
                throw std::logic_error("wrong count in example 54");
            }
        }
    }
}

void delete_examples(mongocxx::database db) {
    db["inventory"].drop();

    {
        // Start Example 55
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        std::vector<bsoncxx::document::value> docs{
            make_document(kvp("item", "journal"),
                          kvp("qty", 25),
                          kvp("size", make_document(kvp("h", 14), kvp("w", 21), kvp("uom", "cm"))),
                          kvp("status", "A")),
            make_document(kvp("item", "notebook"),
                          kvp("qty", 50),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "P")),
            make_document(kvp("item", "paper"),
                          kvp("qty", 100),
                          kvp("size", make_document(kvp("h", 8.5), kvp("w", 11), kvp("uom", "in"))),
                          kvp("status", "D")),
            make_document(
                kvp("item", "planner"),
                kvp("qty", 75),
                kvp("size", make_document(kvp("h", 22.85), kvp("w", 30), kvp("uom", "cm"))),
                kvp("status", "D")),
            make_document(
                kvp("item", "postcard"),
                kvp("qty", 45),
                kvp("size", make_document(kvp("h", 10), kvp("w", 15.25), kvp("uom", "cm"))),
                kvp("status", "A")),
        };

        db["inventory"].insert_many(docs);
        // End Example 55

        if (db["inventory"].count_documents({}) != 5) {
            throw std::logic_error("wrong count in example 55");
        }
    }

    {
        // Start Example 57
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].delete_many(make_document(kvp("status", "A")));
        // End Example 57

        if (db["inventory"].count_documents({}) != 3) {
            throw std::logic_error("wrong count in example 57");
        }
    }

    {
        // Start Example 58
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        db["inventory"].delete_one(make_document(kvp("status", "D")));
        // End Example 58

        if (db["inventory"].count_documents({}) != 2) {
            throw std::logic_error("wrong count in example 58");
        }
    }

    {
        // Start Example 56
        db["inventory"].delete_many({});
        // End Example 56

        if (db["inventory"].count_documents({}) != 0) {
            throw std::logic_error("wrong count in example 56");
        }
    }
}

static bool is_snapshot_ready(mongocxx::client& client, mongocxx::collection& collection) {
    auto opts = mongocxx::options::client_session{};
    opts.snapshot(true);

    auto session = client.start_session(opts);
    try {
        auto cursor = collection.aggregate(session, {});
        for (const auto& it : cursor) {
            (void)it;
            break;
        }
    } catch (const mongocxx::operation_exception& e) {
        if (e.code().value() == 246) {  // snapshot unavailable
            return false;
        }
        throw;
    }
    return true;
}

static void seed_pets(mongocxx::database& db) {
    using namespace mongocxx;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    db.drop();
    db["cats"].insert_one(make_document(kvp("adoptable", true)));
    db["dogs"].insert_one(make_document(kvp("adoptable", true)));
    db["dogs"].insert_one(make_document(kvp("adoptable", false)));
}

static void snapshot_examples(mongocxx::client& client) {
    // Seed the pets database and wait for the snapshot to become available.
    // This follows the pattern from the Python driver as seen below:
    // https://github.com/mongodb/mongo-python-driver/commit/e325b24b78e431cb889c5902d00b8f4af2c700c3#diff-c5d782e261f04fca18024ab18c3ed38fb45ede24cde4f9092e012f6fcbbe0df5R1368
    {
        auto db = client["pets"];
        seed_pets(db);

        auto cats = db["cats"];
        auto dogs = db["dogs"];
        size_t sleep_time = 1;
        while (!is_snapshot_ready(client, cats) && !is_snapshot_ready(client, dogs)) {
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time++));
        }
    }

    // Start Snapshot Query Example 1
    using namespace mongocxx;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    auto db = client["pets"];

    int64_t adoptable_pets_count = 0;

    auto opts = mongocxx::options::client_session{};
    opts.snapshot(true);
    auto session = client.start_session(opts);

    {
        pipeline p;

        p.match(make_document(kvp("adoptable", true))).count("adoptableCatsCount");
        auto cursor = db["cats"].aggregate(session, p);

        for (auto doc : cursor) {
            adoptable_pets_count += doc.find("adoptableCatsCount")->get_int32();
        }
    }

    {
        pipeline p;

        p.match(make_document(kvp("adoptable", true))).count("adoptableDogsCount");
        auto cursor = db["dogs"].aggregate(session, p);

        for (auto doc : cursor) {
            adoptable_pets_count += doc.find("adoptableDogsCount")->get_int32();
        }
    }

    if (adoptable_pets_count != 2) {
        throw std::logic_error("wrong number of adoptable pets in example 59, expecting 2 got: " +
                               std::to_string(adoptable_pets_count));
    }
    // End Snapshot Query Example 1
}

static bool version_at_least(mongocxx::v_noabi::database& db, int minimum_major) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    auto resp = db.run_command(make_document(kvp("buildInfo", 1)));
    auto version = resp.find("version")->get_string().value;
    std::string major_string;
    for (auto i : version) {
        if (i == '.') {
            break;
        }
        major_string += i;
    }
    int server_major = std::stoi(major_string);

    return server_major >= minimum_major;
}

int main() {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance inst{};

    mongocxx::client conn{mongocxx::uri{}};
    auto db = conn["documentation_examples"];

    try {
        insert_examples(db);
        query_top_level_fields_examples(db);
        query_embedded_documents_examples(db);
        query_arrays_examples(db);
        query_array_embedded_documents_examples(db);
        query_null_missing_fields_examples(db);
        projection_examples(db);
        update_examples(db);
        delete_examples(db);
        if (is_replica_set(conn) && version_at_least(db, 5)) {
            snapshot_examples(conn);
        }
    } catch (const std::logic_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
