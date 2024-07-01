#include <chrono>
#include <thread>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/test/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/search_index_view.hpp>
#include <mongocxx/test/client_helpers.hh>

namespace {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using namespace mongocxx;

bool does_search_index_exist_on_cursor(cursor& c,
                                       const search_index_model& model,
                                       bool with_status) {
    for (const auto& index : c) {
        // look for a queryable index with a matching name and assert the definition matches model
        if (index["name"].get_string().value == model.name().value() &&
            index["queryable"].get_bool().value) {
            REQUIRE(index["latestDefinition"].get_document().view() == model.definition());
            // optional addition check needed if with_status is set
            if (!with_status ||
                bsoncxx::string::to_string(index["status"].get_string().value) == "READY") {
                return true;
            }
        }
    }
    return false;
}

// Almost identical to does_search_index_exist_on_cursor but checks type field.
bool does_search_index_exist_on_cursor_with_type(cursor& c,
                                                 const search_index_model& model,
                                                 const char* type,
                                                 bool with_status) {
    for (const auto& index : c) {
        // look for a queryable index with a matching name
        if (index["name"].get_string().value == model.name().value() &&
            index["queryable"].get_bool().value) {
            // assert the definition and type match
            REQUIRE(index["latestDefinition"].get_document().view() == model.definition());
            REQUIRE(!strcmp(index["type"].get_string().value.data(), type));
            // optional addition check needed if with_status is set
            if (!with_status ||
                bsoncxx::string::to_string(index["status"].get_string().value) == "READY") {
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
        WARN("Skipping - Test requires the environment variable: MONGODB_URI");
        return;
    }

    client mongodb_client{uri{uri_getenv}};

    database db = mongodb_client["test"];

    // Prose test case #1.
    SECTION("Driver can successfully create and list search indexes") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        //   {
        //      name: 'test-search-index',
        //     definition: {
        //     mappings: { dynamic: false }
        //   }
        const auto name = "test-search-index";
        const auto definition =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        SUCCEED("Prose Test Case 1: Driver can successfully create and list search indexes");
    }

    // Prose test case #2.
    SECTION("Driver can successfully create multiple indexes in batch") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();

        // {
        //   name: 'test-search-index-1',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        const auto name1 = "test-search-index-1";
        const auto definition1 =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model1 = search_index_model(name1, definition1.view());

        // {
        //   name: 'test-search-index-2',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        const auto name2 = "test-search-index-2";
        const auto definition2 =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model2 = search_index_model(name2, definition2.view());

        const std::vector<search_index_model> models = {model1, model2};

        const std::vector<std::string> result = siv.create_many(models);
        const std::vector<std::string> expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        assert_soon([&siv, &model1, &model2](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model1, false) &&
                   does_search_index_exist_on_cursor(cursor, model2, false);
        });

        SUCCEED("Prose Test Case 2: Driver can successfully create multiple indexes in batch");
    }

    // Prose test case #3.
    SECTION("Driver can successfully drop search indexes") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        const auto name = "test-search-index";
        const auto definition =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model = search_index_model(name, definition.view());

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

    // Prose test case #4.
    SECTION("Driver can update a search index") {
        bsoncxx::oid id;
        auto coll0 = db.create_collection(id.to_string());
        auto siv = coll0.search_indexes();
        //  {
        //    name: 'test-search-index',
        //    definition: {
        //     mappings: { dynamic: false }
        //    }
        // }
        const auto name = "test-search-index";
        const auto definition =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model = search_index_model(name, definition.view());

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
        const auto new_definition =
            make_document(kvp("mappings", make_document(kvp("dynamic", true))));
        const auto new_model = search_index_model(name, new_definition.view());
        siv.update_one(name, new_definition.view());

        assert_soon([&siv, &new_model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, new_model, true);
        });

        SUCCEED("Prose Test Case 4: Driver can update a search index");
    }

    // Prose test case #5.
    SECTION("dropSearchIndex suppresses namespace not found errors") {
        bsoncxx::oid id;
        auto coll = db[id.to_string()];
        coll.search_indexes().drop_one("apples");
        SUCCEED("Prose Test Case 5: dropSearchIndex suppresses namespace not found errors");
    }

    // Prose test case #6.
    SECTION(
        "Driver can successfully create and list search indexes with non-default readConcern and "
        "writeConcern") {
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
        const auto name = "test-search-index-case6";
        const auto definition =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        const auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index-case6");

        assert_soon([&siv, &model](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor(cursor, model, false);
        });

        SUCCEED(
            "Prose Test Case 6: Driver can successfully create and list search indexes with "
            "non-default readConcern and writeConcern");
    }

    // Prose test case #7.
    SECTION("Driver can successfully handle search index types when creating indexes") {
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
            const auto name = "test-search-index-case7-implicit";
            const auto definition =
                make_document(kvp("mappings", make_document(kvp("dynamic", false))));
            const auto model = search_index_model(name, definition.view());

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
            const auto name = "test-search-index-case7-explicit";
            const auto definition =
                make_document(kvp("mappings", make_document(kvp("dynamic", false))));
            const auto model = search_index_model(name, definition.view());

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
            const auto name = "test-search-index-case7-vector";
            const auto type = "vectorSearch";
            const auto definition =
                make_document(kvp("fields",
                                  make_array(make_document(kvp("type", "vector"),
                                                           kvp("path", "plot_embedding"),
                                                           kvp("numDimensions", 1536),
                                                           kvp("similarity", "euclidean")))));
            auto model = search_index_model(name, definition.view()).type(type);

            REQUIRE(model.type().value() == "vectorSearch");
            REQUIRE(siv.create_one(model) == "test-search-index-case7-vector");

            assert_soon([&siv, &model](void) -> bool {
                auto cursor = siv.list();
                return does_search_index_exist_on_cursor_with_type(
                    cursor, model, "vectorSearch", false);
            });
        }

        SUCCEED(
            "Prose Test Case 7: Driver can successfully handle search index types when creating "
            "indexes");
    }

    // Prose test case #8.
    SECTION("Driver requires explicit type to create a vector search index") {
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
        const auto name = "test-search-index-case8-error";
        const auto definition =
            make_document(kvp("fields",
                              make_array(make_document(kvp("type", "vector"),
                                                       kvp("path", "plot_embedding"),
                                                       kvp("numDimensions", 1536),
                                                       kvp("similarity", "euclidean")))));
        const auto model = search_index_model(name, definition.view());

        REQUIRE_THROWS_WITH(siv.create_one(model),
                            Catch::Matchers::Contains("Attribute mappings missing"));

        SUCCEED("Prose Test Case 8: Driver requires explicit type to create a vector search index");
    }
}

TEST_CASE("atlas search indexes tests", "[atlas][search_indexes]") {
    instance::current();

    auto uri_getenv = std::getenv("MONGODB_URI");
    if (!uri_getenv) {
        WARN("Skipping - Test requires the environment variable: MONGODB_URI");
        return;
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
            const auto name = "test-search-index";
            const auto type = "search";
            const auto definition =
                make_document(kvp("mappings", make_document(kvp("dynamic", false))));
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
            const auto name = "test-search-index-vector";
            const auto type = "vectorSearch";
            const auto definition =
                make_document(kvp("fields",
                                  make_array(make_document(kvp("type", "vector"),
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
        const auto name1 = "test-search-index-1";
        const auto type1 = "search";
        const auto definition1 =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model1 = search_index_model(name1, definition1.view()).type(type1);

        // {
        //   name: 'test-search-index-2',
        //   type: "search",
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        const auto name2 = "test-search-index-2";
        const auto type2 = "search";
        const auto definition2 =
            make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model2 = search_index_model(name2, definition2.view()).type(type2);

        const std::vector<search_index_model> models = {model1, model2};

        const std::vector<std::string> result = siv.create_many(models);
        const std::vector<std::string> expected = {"test-search-index-1", "test-search-index-2"};
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
        const auto name1 = "test-search-index-1";
        const auto type1 = "vectorSearch";
        const auto definition1 =
            make_document(kvp("fields",
                              make_array(make_document(kvp("type", "vector"),
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
        const auto name2 = "test-search-index-2";
        const auto type2 = "vectorSearch";
        const auto definition2 =
            make_document(kvp("fields",
                              make_array(make_document(kvp("type", "vector"),
                                                       kvp("path", "plot_embedding"),
                                                       kvp("numDimensions", 1536),
                                                       kvp("similarity", "euclidean")))));
        auto model2 = search_index_model(name2, definition2.view()).type(type2);

        const std::vector<search_index_model> models = {model1, model2};

        const std::vector<std::string> result = siv.create_many(models);
        const std::vector<std::string> expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        assert_soon([&siv, &model1, &model2](void) -> bool {
            auto cursor = siv.list();
            return does_search_index_exist_on_cursor_with_type(
                       cursor, model1, "vectorSearch", false) &&
                   does_search_index_exist_on_cursor_with_type(
                       cursor, model2, "vectorSearch", false);
        });

        SUCCEED("Create many works with vector search types");
    }
}
}  // namespace
