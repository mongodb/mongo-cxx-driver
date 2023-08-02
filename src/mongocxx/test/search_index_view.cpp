#include <chrono>
#include <thread>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/search_index_view.hpp>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using namespace mongocxx;

bool does_search_index_exist_on_cursor(cursor& c, search_index_model& model, bool with_status) {
    for (auto&& doc : c) {
        // check the name, that the index is queryable, and that the definition matches.
        if (doc["name"].get_string().value == *model.name() && doc["queryable"].get_bool().value &&
            doc["latestDefinition"].get_document().view() == model.definition()) {
            // additional check needed if we also need to check the status
            if (with_status &&
                bsoncxx::string::to_string(doc["status"].get_string().value) == "READY") {
                return true;
            } else if (!with_status) {
                return true;
            }
        }
    }
    return false;
}

bool wait_for_search_index(search_index_view& siv,
                           search_index_model& model,
                           bool to_exist,
                           bool with_status) {
    auto c = siv.list();

    // 5 minutes before timeout
    for (int i = 0; i < 60; ++i) {
        if (does_search_index_exist_on_cursor(c, model, with_status) == to_exist)
            return true;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        c = siv.list();
    }
    return false;
}

TEST_CASE("atlas search indexes prose tests", "") {
    instance::current();

    auto uri_getenv = std::getenv("MONGODB_URI");
    if (!uri_getenv) {
        WARN("Skipping - Test requires the environment variable: MONGODB_URI");
        return;
    }

    client mongodb_client{uri{uri_getenv}};

    database db = mongodb_client[std::getenv("FUNCTION_NAME")];

    SECTION("create one with name and definition") {
        // use a randomly generated collection name as there's a server side limitation that
        // prevents multiple search indexes with the same name, definition and collection name
        // from being created.
        bsoncxx::oid id;
        db.create_collection(id.to_string());
        collection coll = db[id.to_string()];
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name = "test-search-index";
        auto definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(name, definition.view()) == "test-search-index");

        REQUIRE(wait_for_search_index(siv, model, true, false));

        std::cout << "create one with name and definition SUCCESS" << std::endl;
    }

    SECTION("create one with model") {
        bsoncxx::oid id;
        db.create_collection(id.to_string());
        collection coll = db[id.to_string()];
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name = "test-search-index";
        auto definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        REQUIRE(wait_for_search_index(siv, model, true, false));

        std::cout << "create one with model SUCCESS" << std::endl;
    }

    SECTION("create many") {
        bsoncxx::oid id;
        db.create_collection(id.to_string());
        collection coll = db[id.to_string()];
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index-1',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name1 = "test-search-index-1";
        auto definition1 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model1 = search_index_model(name1, definition1.view());

        // {
        //   name: 'test-search-index-2',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name2 = "test-search-index-2";
        auto definition2 = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model2 = search_index_model(name2, definition2.view());

        std::vector<search_index_model> models = {model1, model2};

        std::vector<std::string> result = siv.create_many(models);
        std::vector<std::string> expected = {"test-search-index-1", "test-search-index-2"};
        REQUIRE(result == expected);

        REQUIRE(wait_for_search_index(siv, model1, true, false));
        REQUIRE(wait_for_search_index(siv, model2, true, false));

        std::cout << "create many SUCCESS" << std::endl;
    }

    SECTION("drop one") {
        bsoncxx::oid id;
        db.create_collection(id.to_string());
        collection coll = db[id.to_string()];
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name = "test-search-index";
        auto definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        REQUIRE(wait_for_search_index(siv, model, true, false));

        siv.drop_one(name);

        REQUIRE(wait_for_search_index(siv, model, false, false));

        std::cout << "drop one SUCCESS" << std::endl;
    }

    SECTION("update one") {
        bsoncxx::oid id;
        db.create_collection(id.to_string());
        collection coll = db[id.to_string()];
        auto siv = coll.search_indexes();
        // {
        //   name: 'test-search-index',
        //   definition : {
        //     mappings : { dynamic: false }
        //   }
        // }
        auto name = "test-search-index";
        auto definition = make_document(kvp("mappings", make_document(kvp("dynamic", false))));
        auto model = search_index_model(name, definition.view());

        REQUIRE(siv.create_one(model) == "test-search-index");

        REQUIRE(wait_for_search_index(siv, model, true, false));

        // definition : {
        //   mappings : { dynamic: true }
        // }
        auto new_definition = make_document(kvp("mappings", make_document(kvp("dynamic", true))));
        auto new_model = search_index_model(name, new_definition.view());
        siv.update_one(name, new_definition.view());

        REQUIRE(wait_for_search_index(siv, new_model, true, true));

        std::cout << "update one SUCCESS" << std::endl;
    }

    // SECTION("drop one suppress namespace not found") {
    //     bsoncxx::oid id;
    //     collection coll;
    //     coll.rename("fake-collection");
    //     coll.search_indexes().drop_one("apples");
    // }
}
}  // namespace
