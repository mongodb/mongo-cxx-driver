#include <chrono>
#include <thread>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/oid.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/search_index_view.hpp>

#include <bsoncxx/test/catch.hh>

#include <mongocxx/test/client_helpers.hh>

#include <catch2/catch_case_sensitive.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using namespace mongocxx;

bool does_search_index_exist_on_cursor(cursor& c, search_index_model const& model, bool with_status) {
    for (auto const& index : c) {
        // look for a queryable index with a matching name and assert the definition matches model
        if (index["name"].get_string().value == model.name().value() && index["queryable"].get_bool().value) {
            REQUIRE(index["latestDefinition"].get_document().view() == model.definition());
            // optional addition check needed if with_status is set
            if (!with_status || bsoncxx::string::to_string(index["status"].get_string().value) == "READY") {
                return true;
            }
        }
    }
    return false;
}

// Almost identical to does_search_index_exist_on_cursor but checks type field.
bool does_search_index_exist_on_cursor_with_type(
    cursor& c,
    search_index_model const& model,
    char const* type,
    bool with_status) {
    for (auto const& index : c) {
        // look for a queryable index with a matching name
        if (index["name"].get_string().value == model.name().value() && index["queryable"].get_bool().value) {
            // assert the definition and type match
            REQUIRE(index["latestDefinition"].get_document().view() == model.definition());
            REQUIRE(!strcmp(index["type"].get_string().value.data(), type));
            // optional addition check needed if with_status is set
            if (!with_status || bsoncxx::string::to_string(index["status"].get_string().value) == "READY") {
                return true;
            }
        }
    }
    return false;
}

// `assert_soon` repeatedly calls `fn` and asserts `fn` eventually returns true.
// `fn` is called every five seconds. Fails if `fn` does not return true within five minutes.
void assert_soon(std::function<bool()> fn) {
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start < std::chrono::minutes(5)) {
        if (fn()) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    FAIL("Expected function to return true within five minutes, but did not");
}

TEST_CASE("atlas search indexes prose tests", "[atlas][search_indexes]") {
    instance::current();

    auto uri_getenv = std::getenv("MONGODB_URI");
    if (!uri_getenv) {
        SKIP("Test requires the environment variable: MONGODB_URI");
    }

    client mongodb_client{uri{uri_getenv}};

    database db = mongodb_client["test"];

    SECTION("Prose Test Case 1: Driver can successfully create and list search indexes") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name = "test-search-index";
        auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        SUCCEED("Prose Test Case 1: Driver can successfully create and list search indexes");
    }

    SECTION("Prose test case #2: Driver can successfully create multiple indexes in batch") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();

        // {
        //   name: 'test-search-index-1',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name1 = "test-search-index-1";
        auto const definition1 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model1 = search_index_model(name1, definition1.view());

        // {
        //   name: 'test-search-index-2',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name2 = "test-search-index-2";
        auto const definition2 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model2 = search_index_model(name2, definition2.view());

        std::vector<search_index_model> const models = {model1, model2};

        std::vector<std::string> const result = siv.create_many(models);
        std::vector<std::string> const expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        assert_soon([&siv, &model1, &model2](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model1, false) &&
                   does_search_index_exist_on_cursor(cursor, model2, false);
        });

        SUCCEED("Prose Test Case 2: Driver can successfully create multiple indexes in batch");
    }

    SECTION("Prose test case #3: Driver can successfully drop search indexes") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name = "test-search-index";
        auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        siv.drop_one(name);

        // This test fails if it times out waiting for the deletion to succeed.
        assert_soon([&siv](void) -> bool {
            auto cursor = siv.list();
            // Return true if empty results are returned.
            return cursor.begin() == cursor.end();
        });

        SUCCEED("Prose Test Case 3: Driver can successfully drop search indexes");
    }

    SECTION("Prose test case #4: Driver can update a search index") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        //  {
        //    name: 'test-search-index',
        //    definition: {
        //     mappings: { dynamic: false }
        //    }
        // }
        auto const name = "test-search-index";
        auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        // {
        //   name: 'test-search-index',
        //   definition: {
        //     mappings: { dynamic: true }
        //   }
        // }
        auto const new_definition = make_document(kvp("mappings", make_document(kvp("dynamic", true))));
        auto const new_model = search_index_model(name, new_definition.view());
        siv.update_one(name, new_definition.view());

        assert_soon([&siv, &new_model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, new_model, true);
        });

        SUCCEED("Prose Test Case 4: Driver can update a search index");
    }

    SECTION("Prose test case #5: dropSearchIndex suppresses namespace not found errors") {
        bsoncxx::oid id;
        auto coll = db[id.to_string()];
        coll.search_indexes().drop_one("apples");
        SUCCEED("Prose Test Case 5: dropSearchIndex suppresses namespace not found errors");
    }

    SECTION(
        "Prose test case #6: Driver can successfully create and list search indexes with "
        "non-default readConcern and writeConcern") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());

        // Apply non-default write concern WriteConcern(w=1) to coll0.
        auto nondefault_wc = mongocxx::write_concern();
        nondefault_wc.nodes(1);
        coll0.write_concern(nondefault_wc);

        // Apply non-default read concern ReadConcern(level="majority") to coll0.
        auto nondefault_rc = mongocxx::read_concern();
        nondefault_rc.acknowledge_level(mongocxx::read_concern::level::k_majority);
        coll0.read_concern(nondefault_rc);

        auto siv = coll0.search_indexes();
        // {
        //   name: 'test-search-index-case6',
        //   definition: {
        //     mappings: { dynamic: false }
        //   }
        // }
        auto const name = "test-search-index-case6";
        auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto const model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index-case6");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        SUCCEED(
            "Prose Test Case 6: Driver can successfully create and list search indexes with "
            "non-default readConcern and writeConcern");
    }

    SECTION(
        "Prose test case #7: Driver can successfully handle search index types when creating "
        "indexes") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();

        {
            // {
            //     name: 'test-search-index-case7-implicit',
            //     definition: {
            //      mappings: { dynamic: false }
            //     }
            // }
            auto const name = "test-search-index-case7-implicit";
            auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
            auto const model = search_index_model(name, definition.view());

            REQUIRE(siv.create_one(model) == "test-search-index-case7-implicit");

            assert_soon([&siv, &model](void) -> bool {
                auto cursor = siv.list();
                return does_search_index_exist_on_cursor_with_type(cursor, model, "search", false);
            });
        }

        {
            //   {
            //     name: 'test-search-index-case7-explicit',
            //     type: 'search',
            //     definition: {
            //       mappings: { dynamic: false }
            //     }
            //   }
            auto const name = "test-search-index-case7-explicit";
            auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
            auto model = search_index_model(name, definition.view()).type("search");

            REQUIRE(siv.create_one(model) == "test-search-index-case7-explicit");

            assert_soon([&siv, &model](void) -> bool {
                auto cursor = siv.list();
                return does_search_index_exist_on_cursor_with_type(cursor, model, "search", false);
            });
        }

        {
            //   {
            //     name: 'test-search-index-case7-vector',
            //     type: 'vectorSearch',
            //     definition: {
            //       fields: [
            //          {
            //              type: 'vector',
            //              path: 'plot_embedding',
            //              numDimensions: 1536,
            //              similarity: 'euclidean',
            //          },
            //       ]
            //     }
            //   }
            auto const name = "test-search-index-case7-vector";
            auto const type = "vectorSearch";
            auto const definition = make_document(
                kvp("fields",
                    make_array(make_document(
                        kvp("type", "vector"),
                        kvp("path", "plot_embedding"),
                        kvp("numDimensions", 1536),
                        kvp("similarity", "euclidean")))));
            auto model = search_index_model(name, definition.view()).type(type);

            REQUIRE(model.type().value() == "vectorSearch");
            REQUIRE(siv.create_one(model) == "test-search-index-case7-vector");

            assert_soon([&siv, &model](void) -> bool {
                auto cursor = siv.list();
                return does_search_index_exist_on_cursor_with_type(cursor, model, "vectorSearch", false);
            });
        }

        SUCCEED(
            "Prose Test Case 7: Driver can successfully handle search index types when creating "
            "indexes");
    }

    SECTION("Prose test case #8: Driver requires explicit type to create a vector search index") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        //   {
        //     name: 'test-search-index-case8-error',
        //     definition: {
        //       fields: [
        //          {
        //              type: 'vector',
        //              path: 'plot_embedding',
        //              numDimensions: 1536,
        //              similarity: 'euclidean',
        //          },
        //       ]
        //     }
        //   }
        auto const name = "test-search-index-case8-error";
        auto const definition = make_document(
            kvp("fields",
                make_array(make_document(
                    kvp("type", "vector"),
                    kvp("path", "plot_embedding"),
                    kvp("numDimensions", 1536),
                    kvp("similarity", "euclidean")))));
        auto const model = search_index_model(name, definition.view());

        REQUIRE_THROWS_WITH(siv.create_one(model), Catch::Matchers::ContainsSubstring("Attribute mappings missing"));

        SUCCEED("Prose Test Case 8: Driver requires explicit type to create a vector search index");
    }
}

TEST_CASE("atlas search indexes tests", "[atlas][search_indexes]") {
    instance::current();

    auto uri_getenv = std::getenv("MONGODB_URI");
    if (!uri_getenv) {
        SKIP("Test requires the environment variable: MONGODB_URI");
    }

    client mongodb_client{uri{uri_getenv}};

    database db = mongodb_client["test"];

    SECTION("Name, type, and definition fields can be properly retrieved from model") {
        {
            // {
            //   name: 'test-search-index',
            //   type: "search",
            //   definition : {
            //     mappings : { dynamic: false }
            //   }
            // }
            auto const name = "test-search-index";
            auto const type = "search";
            auto const definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
            auto model = search_index_model(name, definition.view()).type(type);

            REQUIRE(model.name().value() == name);
            REQUIRE(model.type().value() == type);
            REQUIRE(model.definition() == definition.view());
        }

        {
            //   {
            //     name: 'test-search-index-vector',
            //     type: 'vectorSearch',
            //     definition: {
            //       fields: [
            //          {
            //              type: 'vector',
            //              path: 'plot_embedding',
            //              numDimensions: 1536,
            //              similarity: 'euclidean',
            //          },
            //       ]
            //     }
            //   }
            auto const name = "test-search-index-vector";
            auto const type = "vectorSearch";
            auto const definition = make_document(
                kvp("fields",
                    make_array(make_document(
                        kvp("type", "vector"),
                        kvp("path", "plot_embedding"),
                        kvp("numDimensions", 1536),
                        kvp("similarity", "euclidean")))));
            auto model = search_index_model(name, definition.view()).type(type);

            REQUIRE(model.name().value() == name);
            REQUIRE(model.type().value() == type);
            REQUIRE(model.definition() == definition.view());
        }

        SUCCEED("Name, type, and definition fields can be properly retrieved from model");
    }

    SECTION("Create many works with with search index types") {
        bsoncxx::oid id;
        auto coll = db.create_collection(id.to_string());
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index-1',
        //   type: "search",
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name1 = "test-search-index-1";
        auto const type1 = "search";
        auto const definition1 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model1 = search_index_model(name1, definition1.view()).type(type1);

        // {
        //   name: 'test-search-index-2',
        //   type: "search",
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto const name2 = "test-search-index-2";
        auto const type2 = "search";
        auto const definition2 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model2 = search_index_model(name2, definition2.view()).type(type2);

        std::vector<search_index_model> const models = {model1, model2};

        std::vector<std::string> const result = siv.create_many(models);
        std::vector<std::string> const expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        assert_soon([&siv, &model1, &model2](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor_with_type(cursor, model1, "search", false) &&
                   does_search_index_exist_on_cursor_with_type(cursor, model2, "search", false);
        });

        SUCCEED("Create many works with with search index types");
    }

    SECTION("Create many works with vector search types") {
        bsoncxx::oid id;
        auto coll = db.create_collection(id.to_string());
        auto siv = coll.search_indexes();
        //   {
        //     name: 'test-search-index-1',
        //     type: 'vectorSearch',
        //     definition: {
        //       fields: [
        //          {
        //              type: 'vector',
        //              path: 'plot_embedding',
        //              numDimensions: 1536,
        //              similarity: 'euclidean',
        //          },
        //       ]
        //     }
        //   }
        auto const name1 = "test-search-index-1";
        auto const type1 = "vectorSearch";
        auto const definition1 = make_document(
            kvp("fields",
                make_array(make_document(
                    kvp("type", "vector"),
                    kvp("path", "plot_embedding"),
                    kvp("numDimensions", 1536),
                    kvp("similarity", "euclidean")))));
        auto model1 = search_index_model(name1, definition1.view()).type(type1);

        //   {
        //     name: 'test-search-index-2',
        //     type: 'vectorSearch',
        //     definition: {
        //       fields: [
        //          {
        //              type: 'vector',
        //              path: 'plot_embedding',
        //              numDimensions: 1536,
        //              similarity: 'euclidean',
        //          },
        //       ]
        //     }
        //   }
        auto const name2 = "test-search-index-2";
        auto const type2 = "vectorSearch";
        auto const definition2 = make_document(
            kvp("fields",
                make_array(make_document(
                    kvp("type", "vector"),
                    kvp("path", "plot_embedding"),
                    kvp("numDimensions", 1536),
                    kvp("similarity", "euclidean")))));
        auto model2 = search_index_model(name2, definition2.view()).type(type2);

        std::vector<search_index_model> const models = {model1, model2};

        std::vector<std::string> const result = siv.create_many(models);
        std::vector<std::string> const expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        assert_soon([&siv, &model1, &model2](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor_with_type(cursor, model1, "vectorSearch", false) &&
                   does_search_index_exist_on_cursor_with_type(cursor, model2, "vectorSearch", false);
        });

        SUCCEED("Create many works with vector search types");
    }
}
} // namespace
