#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

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
