// Copyright 2016 MongoDB Inc.
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

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/test/spec/operation.hh>
#include <mongocxx/test/spec/util.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using namespace bsoncxx;
using namespace mongocxx;
using namespace mongocxx::spec;

// Clears the collection and initialize it as the spec describes.
void initialize_collection(collection* coll, array::view initial_data) {
    // We delete all documents from the collection instead of dropping the collection, as the former
    // has much better performance for the CRUD test collections.
    coll->delete_many({});

    std::vector<document::view> documents_to_insert;

    for (auto&& document : initial_data) {
        documents_to_insert.push_back(document.get_document().value);
    }

    if (documents_to_insert.size() > 0) {
        coll->insert_many(documents_to_insert);
    }
}

void run_crud_tests_in_file(std::string test_path, client* client) {
    INFO("Test path: " << test_path);
    bsoncxx::stdx::optional<document::value> test_spec = test_util::parse_test_file(test_path);
    REQUIRE(test_spec);

    document::view test_spec_view = test_spec->view();

    if (should_skip_spec_test(*client, test_spec_view)) {
        return;
    }

    array::view tests = test_spec_view["tests"].get_array().value;

    database db = (*client)["crud_tests"];
    collection coll = db["test"];

    for (auto&& test : tests) {
        std::string description = bsoncxx::string::to_string(test["description"].get_utf8().value);
        INFO("Test description: " << description);
        initialize_collection(&coll, test_spec_view["data"].get_array().value);
        document::view outcome = test["outcome"].get_document().value;

        if (outcome["collection"] && outcome["collection"].get_document().value["name"]) {
            std::string out_coll_name = bsoncxx::string::to_string(
                outcome["collection"].get_document().value["name"].get_utf8().value);
            collection out_coll = db[out_coll_name];
            out_coll.delete_many({});
        }

        document::view operation = test["operation"].get_document().value;
        std::string operation_name = bsoncxx::string::to_string(operation["name"].get_utf8().value);

        auto run_test = get_test_runners()[operation_name];
        document::value actual_result = run_test(&coll, operation);

        if (outcome["collection"]) {
            collection out_coll = db["test"];

            document::view out_collection_doc = outcome["collection"].get_document().value;
            if (out_collection_doc["name"]) {
                auto out_coll_name =
                    bsoncxx::string::to_string(out_collection_doc["name"].get_utf8().value);
                out_coll = db[out_coll_name];
            }

            std::vector<std::string> actual_data;
            std::vector<std::string> expected_data;

            options::find options{};
            builder::basic::document sort{};
            sort.append(builder::basic::kvp("_id", 1));
            options.sort(sort.extract());

            for (auto&& doc : out_coll.find({}, options)) {
                actual_data.push_back(to_json(doc));
            }

            for (auto&& ele : out_collection_doc["data"].get_array().value) {
                expected_data.push_back(to_json(ele.get_document().value));
            }

            REQUIRE(expected_data == actual_data);

            // The C++ driver does not implement the optional returning of results for
            // aggregations with $out.
            if (operation["name"].get_utf8().value == bsoncxx::stdx::string_view{"aggregate"}) {
                continue;
            }
        }

        auto result = outcome["result"].get_value();

        auto builder = builder::basic::document{};
        builder.append(builder::basic::kvp("result", result));
        document::value expected_result = builder.extract();

        // The spec tests intentionally omit fields from the 'result' document for which the values
        // of those fields are unspecified.  As such, we strip out fields from 'actual_result' that
        // aren't also present in 'expected_result'.
        auto actual_result_ele = actual_result.view()["result"];
        auto expected_result_ele = expected_result.view()["result"];
        if (actual_result_ele.type() == bsoncxx::type::k_document &&
            expected_result_ele.type() == bsoncxx::type::k_document) {
            builder::basic::document actual_result_builder{};

            actual_result_builder.append(
                builder::basic::kvp("result", [&](builder::basic::sub_document subdoc) {
                    for (auto&& expected_ele : expected_result_ele.get_document().value) {
                        document::element actual_ele =
                            actual_result_ele.get_document().value[expected_ele.key()];
                        if (actual_ele) {
                            subdoc.append(
                                builder::basic::kvp(actual_ele.key(), actual_ele.get_value()));
                        }
                    }
                }));

            actual_result = actual_result_builder.extract();
        }

        REQUIRE(to_json(expected_result.view()) == to_json(actual_result.view()));
    }
}

TEST_CASE("CRUD spec automated tests", "[crud_spec]") {
    instance::current();

    client client{uri{}};
    char* crud_tests_path = std::getenv("CRUD_TESTS_PATH");
    REQUIRE(crud_tests_path);

    std::string path{crud_tests_path};

    if (path.back() == '/') {
        path.pop_back();
    }

    std::ifstream test_files{path + "/test_files.txt"};

    REQUIRE(test_files.good());

    std::string test_file;

    while (std::getline(test_files, test_file)) {
        run_crud_tests_in_file(path + "/" + test_file, &client);
    }
}
}  // namespace
