#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{};

    auto db = conn["test"];

    // Update top-level fields in a single document.
    {
        // @begin: cpp-update-top-level-fields
        document filter, update;
        filter << "name" << "Juni";
        update << "$set" << open_document
                   << "cuisine" << "American (New)" << close_document
               << "$currentDate" << open_document
                   << "lastModified" << true << close_document;

        db["restaurants"].update_one(filter, update);
        // @end: cpp-update-top-level-fields
    }

    // Update an embedded document in a single document.
    {
        // @begin: cpp-update-embedded-field
        document filter, update;
        filter << "restaurant_id" << "41156888";
        update << "$set" << open_document <<
                   "address.street" << "East 31st Street" << close_document;

        db["restaurants"].update_one(filter, update);
        // @end: cpp-update-embedded-field
    }

    // Update multiple documents.
    {
        // @begin: cpp-update-multiple-documents
        document filter, update;
        filter << "address.zipcode" << "10016"
               << "cuisine" << "Other";
        update << "$set" << open_document
                   << "cuisine" << "Category To Be Determined" << close_document
               << "$currentDate" << open_document
                   << "lastModified" << true << close_document;

        db["restaurants"].update_many(filter, update);
        // @end: cpp-update-multiple-documents
    }

    // Replace the contents of a single document.
    {
        // @begin: cpp-replace-document
        document filter, replacement;
        filter << "restaurant_id" << "41704620";
        replacement << "name" << "Vella 2"
                    << "address" << open_document
                        << "coord" << open_array
                            << -73.9557413 << 40.7720266 << close_array
                        << "building" << "1480"
                        << "street" << "2 Avenue"
                        << "zipcode" << "10075" << close_document;

        db["restaurants"].replace_one(filter, replacement);
        // @end: cpp-replace-document
    }
}
