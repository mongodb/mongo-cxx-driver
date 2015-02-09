#include <bsoncxx/builder.hpp>

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::helpers;
using bsoncxx::builder::document;

int main(int, char**) {
    mongocxx::client conn{};

    auto db = conn["test"];
    
    // Create a single field index.
    {
        // @begin: cpp-single-field-index
        document index_spec;
        index_spec << "cuisine" << 1;
        db["restaurants"].create_index(index_spec, {});
        // @end: cpp-single-field-index
    }
    
    // Create a compound index.
    {
        // @begin: cpp-create-compound-index
        document index_spec;
        index_spec << "cuisine" << 1 << "address.zipcode" << -1;
        db["restaurants"].create_index(index_spec, {});
        // @end: cpp-create-compound-index
    }
}
