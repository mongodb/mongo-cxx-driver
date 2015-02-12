#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

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
