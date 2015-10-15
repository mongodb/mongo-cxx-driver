#include <bsoncxx/builder/stream/document.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
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
    try {
        db["restaurants"].drop();
    } catch (const std::exception&) {
        // Collection did not exist.
    }

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
        db["restaurants"].drop();
        // @begin: cpp-create-compound-index
        document index_spec;
        index_spec << "cuisine" << 1 << "address.zipcode" << -1;
        db["restaurants"].create_index(index_spec, {});
        // @end: cpp-create-compound-index
    }

    // Create a unique index.
    {
        db["restaurants"].drop();
        // @begin: cpp-create-unique-index
        document index_spec;
        mongocxx::options::index index_options{};
        index_spec << "website" << 1;
        index_options.unique(true);
        db["restaurants"].create_index(index_spec, index_options);
        // @end: cpp-create-unique-index
    }

    // Create an index with storage engine options
    {
        db["restaurants"].drop();
        // @begin: cpp-create-wt-options-index
        document index_spec;
        mongocxx::options::index index_options{};
        std::unique_ptr<mongocxx::options::index::wiredtiger_storage_options> wt_options =
            mongocxx::stdx::make_unique<mongocxx::options::index::wiredtiger_storage_options>();
        index_spec << "cuisine" << 1;
        wt_options->config_string("block_allocation=first");
        index_options.storage_options(std::move(wt_options));
        db["restaurants"].create_index(index_spec, index_options);
        // @begin: cpp-create-wt-options-index
    }
}
