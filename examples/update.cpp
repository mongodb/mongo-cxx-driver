#include <bsoncxx/builder.hpp>

#include <mongocxx/client.hpp>

using namespace bsoncxx::builder::helpers;
using bsoncxx::builder::document;

int main(int, char**) {
    mongocxx::client conn{};

    auto db = conn["test"];

    // Update top-level fields in a single document.
    {
        // @begin: cpp-update-top-level-fields
        document filter, update;
        filter << "name" << "Juni";
        update << "$set" << open_doc
                   << "cuisine" << "American (New)" << close_doc
               << "$currentDate" << open_doc
                   << "lastModified" << true << close_doc;
        db["restaurants"].update_one(filter, update);
        // @end: cpp-update-top-level-fields
    }

    // Update an embedded document in a single document.
    {
        // @begin: cpp-update-embedded-field
        document filter, update;
        filter << "restaurant_id" << "41156888";
        update << "$set" << open_doc <<
                   "address.street" << "East 31st Street" << close_doc;
        // @end: cpp-update-embedded-field
        db["restaurants"].update_one(filter, update);
    }

    // Update multiple documents.
    {
        // @begin: cpp-update-multiple-documents
        document filter, update;
        filter << "address.zipcode" << "10016";
        update << "$set" << open_doc
                   << "borough" << "Midtown" << close_doc
               << "$currentDate" << open_doc
                   << "lastModified" << true << close_doc;

        // @end: cpp-update-multiple-documents
        db["restaurants"].update_many(filter, update);
    }

    // Replace the contents of a single document.
    {
        // @begin: cpp-replace-document
        document filter, replacement;
        filter << "restaurant_id" << "41704620";
        replacement << "name" << "Vella 2"
                    << "address" << open_doc
                        << "coord" << open_array
                            << -73.9557413 << 40.7720266 << close_array
                        << "building" << "1480"
                        << "street" << "2 Avenue"
                        << "zipcode" << "10075" << close_doc;
        db["restaurants"].replace_one(filter, replacement);
        // @end: cpp-replace-document
    }
}
