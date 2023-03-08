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

    auto builder = bsoncxx::builder::stream::document{};
    auto doc_value =
        builder << "name"
                << "MongoDB"
                << "type"
                << "database"
                << "count" << 1 << "versions" << bsoncxx::builder::stream::open_array << "v3.2"
                << "v3.0"
                << "v2.6" << close_array << "info" << bsoncxx::builder::stream::open_document << "x"
                << 203 << "y" << 102 << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::finalize;

    auto view = doc_value.view();

    auto element = view["name"];
    if (element.type() != bsoncxx::type::k_string) {
        // Error
    }
    auto name = element.get_string().value;

    auto result = collection.insert_one(view);

    std::vector<bsoncxx::document::value> documents;
    for (int i = 0; i < 100; i++) {
        documents.push_back(bsoncxx::builder::stream::document{} << "i" << i << finalize);
    }

    collection.insert_many(documents);

    auto maybe_result = collection.find_one({});
    if (maybe_result) {
        // Do something with *maybe_result;
    }

    auto cursor = collection.find({});
    for (auto doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << "\n";
    }

    auto maybe_result = collection.find_one(document{} << "i" << 71 << finalize);
    if (maybe_result) {
        std::cout << bsoncxx::to_json(*maybe_result) << "\n";
    }

    auto cursor = collection.find(document{} << "i" << open_document << "$gt" << 50 << "$lte" << 100
                                             << close_document << finalize);
    for (auto doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << "\n";
    }

    collection.update_one(
        document{} << "i" << 10 << finalize,
        document{} << "$set" << open_document << "i" << 110 << close_document << finalize);

    auto result = collection.update_many(
        document{} << "i" << open_document << "$lt" << 100 << close_document << finalize,
        document{} << "$inc" << open_document << "i" << 100 << close_document << finalize);

    if (result) {
        std::cout << result->modified_count() << "\n";
    }

    collection.delete_one(document{} << "i" << 110 << finalize);

    auto result = collection.delete_many(document{} << "i" << open_document << "$gte" << 100
                                                    << close_document << finalize);

    if (result) {
        std::cout << result->deleted_count() << "\n";
    }

    auto index_specification = document{} << "i" << 1 << finalize;
    collection.create_index(std::move(index_specification));
}