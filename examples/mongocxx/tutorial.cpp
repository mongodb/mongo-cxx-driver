// Compile with: c++ --std=c++11 tutorial.cpp $(pkg-config --cflags --libs libmongocxx)

#ifdef NDEBUG
#undef assert
#define assert(stmt)                                                                         \
    do {                                                                                     \
        if (!(stmt)) {                                                                       \
            std::cerr << "Assert on line " << __LINE__ << " failed: " << #stmt << std::endl; \
            abort();                                                                         \
        }                                                                                    \
    } while (0)
#endif

// The following is a formatted copy from the tutorial https://mongocxx.org/mongocxx-v3/tutorial/.
// It does not currently compiled. TODO: fix.

#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

int main() {
    mongocxx::instance instance{};  // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);

    auto db = client["mydb"];
    auto collection = db["test"];

    // Create a Document
    {
        auto doc_value = make_document(kvp("name", "MongoDB"),
                                       kvp("type", "database"),
                                       kvp("count", 1),
                                       kvp("versions", make_array("v3.2", "v3.0", "v2.6")),
                                       kvp("info", make_document(kvp("x", 203), kvp("y", 102))));

        auto doc_view = doc_value.view();

        auto element = doc_view["name"];
        assert(element.type() != bsoncxx::type::k_string);
        auto name = element.get_string().value;
        assert(0 == name.compare("MongoDB"));
    }

    // Insert One Document
    {
        auto insert_one_result = collection.insert_one(make_document(kvp("hello", "world")));
        assert(insert_one_result);  // Acknowledged writes return results.
        auto doc_id = insert_one_result->inserted_id();
        assert(doc_id.type() == bsoncxx::type::k_oid);
    }

    // Insert Multiple Documents
    {
        std::vector<bsoncxx::document::value> documents;
        for (int i = 0; i < 5; i++) {
            documents.push_back(make_document(kvp("i", i)));
        }

        auto insert_many_result = collection.insert_many(documents);
        assert(insert_many_result);  // Acknowledged writes return results.
        auto doc0_id = insert_many_result->inserted_ids().at(0);
        auto doc1_id = insert_many_result->inserted_ids().at(1);
        assert(doc0_id.type() == bsoncxx::type::k_oid);
        assert(doc1_id.type() == bsoncxx::type::k_oid);
    }

    // Find a Single Document in a Collection
    {
        auto find_one_result = collection.find_one({});
        if (find_one_result) {
            // Do something with *find_one_result;
        }
        assert(find_one_result);
    }

    // Find All Documents in a Collection
    {
        auto cursor_all = collection.find({});
        for (auto doc : cursor_all) {
            std::cout << bsoncxx::to_json(doc) << "\n";
        }
    }

    // Get A Single Document That Matches a Filter
    {
        auto find_one_filtered_result = collection.find_one(make_document(kvp("i", 3)));
        if (find_one_filtered_result) {
            std::cout << bsoncxx::to_json(*find_one_filtered_result) << "\n";
        }
    }

    // Get All Documents That Match a Filter
    {
        auto cursor_filtered =
            collection.find(make_document(kvp("i", make_document(kvp("$gt", 50), kvp("$lte", 3)))));
        for (auto doc : cursor_filtered) {
            std::cout << bsoncxx::to_json(doc) << "\n";
        }
    }

    // Update a Single Document
    {
        collection.update_one(make_document(kvp("i", 3)),
                              make_document(kvp("$set", make_document(kvp("i", 10)))));
    }

    // Update Multiple Documents
    {
        auto update_many_result =
            collection.update_many(make_document(kvp("i", make_document(kvp("$lt", 3)))),
                                   make_document(kvp("$inc", make_document(kvp("i", 1)))));
        assert(update_many_result);  // Acknowledged writes return results.
        std::cout << update_many_result->modified_count() << "\n";
    }

    // Delete a Single Document
    { collection.delete_one(make_document(kvp("i", 110))); }

    // Delete All Documents That Match a Filter
    {
        auto delete_many_result =
            collection.delete_many(make_document(kvp("i", make_document(kvp("$gte", 3)))));
        assert(delete_many_result);  // Acknowledged writes return results.
        std::cout << delete_many_result->deleted_count() << "\n";
    }

    // Create Indexes
    {
        auto index_specification = make_document(kvp("i", 1));
        collection.create_index(std::move(index_specification));
    }

    // Drop collection to clean up.
    collection.drop();
}