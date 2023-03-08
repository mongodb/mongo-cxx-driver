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

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

int main() {
    mongocxx::instance instance{};  // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);

    auto db = client["mydb"];
    auto collection = db["test"];

    // Create a Document
    {
        auto builder = bsoncxx::builder::stream::document{};
        auto doc_value =
            builder << "name"
                    << "MongoDB"
                    << "type"
                    << "database"
                    << "count" << 1 << "versions" << bsoncxx::builder::stream::open_array << "v3.2"
                    << "v3.0"
                    << "v2.6" << close_array << "info" << bsoncxx::builder::stream::open_document
                    << "x" << 203 << "y" << 102 << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::finalize;

        auto view = doc_value.view();

        auto element = view["name"];
        if (element.type() != bsoncxx::type::k_string) {
            // Error
        }
        auto name = element.get_string().value;
    }

    // Insert One Document
    {
        auto insert_one_result = collection.insert_one(view);
        // Acknowledged writes return a result.
        assert(insert_one_result);
        auto doc_id = insert_one_result->inserted_id();
    }

    // Insert Multiple Documents
    {
        std::vector<bsoncxx::document::value> documents;
        for (int i = 0; i < 100; i++) {
            documents.push_back(bsoncxx::builder::stream::document{} << "i" << i << finalize);
        }

        auto insert_many_result = collection.insert_many(documents);
        assert(insert_many_result);
        auto doc0_id = insert_many_result->inserted_ids().at(0);
        auto doc1_id = insert_many_result->inserted_ids().at(1);
    }

    // Find a Single Document in a Collection
    {
        auto find_one_result = collection.find_one({});
        if (find_one_result) {
            // Do something with *find_one_result;
        }
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
        auto find_one_filtered_result = collection.find_one(document{} << "i" << 71 << finalize);
        if (find_one_filtered_result) {
            std::cout << bsoncxx::to_json(*find_one_filtered_result) << "\n";
        }
    }

    // Get All Documents That Match a Filter
    {
        auto cursor_filtered =
            collection.find(document{} << "i" << open_document << "$gt" << 50 << "$lte" << 100
                                       << close_document << finalize);
        for (auto doc : cursor_filtered) {
            std::cout << bsoncxx::to_json(doc) << "\n";
        }
    }

    // Update a Single Document
    {
        collection.update_one(
            document{} << "i" << 10 << finalize,
            document{} << "$set" << open_document << "i" << 110 << close_document << finalize);
    }

    // Update Multiple Documents
    {
        auto update_many_result = collection.update_many(
            document{} << "i" << open_document << "$lt" << 100 << close_document << finalize,
            document{} << "$inc" << open_document << "i" << 100 << close_document << finalize);

        if (update_many_result) {
            std::cout << update_many_result->modified_count() << "\n";
        }
    }

    // Delete a Single Document
    { collection.delete_one(document{} << "i" << 110 << finalize); }

    // Delete All Documents That Match a Filter
    {
        auto delete_many_result = collection.delete_many(
            document{} << "i" << open_document << "$gte" << 100 << close_document << finalize);

        if (delete_many_result) {
            std::cout << delete_many_result->deleted_count() << "\n";
        }
    }

    // Create Indexes
    {
        auto index_specification = document{} << "i" << 1 << finalize;
        collection.create_index(std::move(index_specification));
    }
}