#include <bsoncxx/builder.hpp>

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::helpers;
using bsoncxx::builder::document;

int main(int, char**) {
    mongocxx::client conn{};

    auto db = conn["test"];

    // Remove all documents that match a condition.
    {
        // @begin: cpp-remove-matching-documents
        document filter;
        filter << "borough" << "Manhattan";
        
        db["restaurants"].delete_many(filter);
        // @end: cpp-remove-matching-documents
    }

    // Remove one document that matches a condition.
    {
        // @begin: cpp-remove-justone
        document filter;
        filter << "borough" << "Queens";
        
        db["restaurants"].delete_one(filter);
        // @end: cpp-remove-justone
    }
    
    // Remove all documents in a collection.
    {
        // @begin: cpp-remove-all-documents
        db["restaurants"].delete_many({});
        // @end: cpp-remove-all-documents
    }

    // Drop a collection.
    {
        // @begin: cpp-drop-collection
        db["restaurants"].drop();
        // @end: cpp-drop-collection
    }
}
