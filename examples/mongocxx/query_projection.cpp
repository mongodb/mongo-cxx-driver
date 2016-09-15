#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

int main(int, char **) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    auto coll = conn["test"]["query_projection"];
    coll.drop();

    // Insert a test document
    auto joe = document{} << "user info" << open_document << "user name"
                          << "Joe" << close_document << finalize;
    auto result = coll.insert_one(joe.view());
    std::cout << "Inserted " << result->inserted_id().get_oid().value.to_string() << std::endl;

    // Create the query filter
    auto filter = document{} << "user info.user name"
                             << "Joe" << finalize;

    // Create the find options with the projection
    mongocxx::options::find opts{};
    opts.projection(document{} << "_id" << 1 << finalize);

    // Execute find with options
    auto cursor = coll.find(filter.view(), opts);
    for (auto &&doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }

    return EXIT_SUCCESS;
}
