#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>

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

    // TODO: fix dates

    // @begin: cpp-insert-a-document
    auto restaurant_doc = document{}
        << "address" << open_document
            << "street"   << "2 Avenue"
            << "zipcode"  <<  "10075"
            << "building" << "1480"
            << "coord"    << open_array
                << -73.9557413 << 40.7720266 << close_array << close_document
        << "borough"  << "Manhattan"
        << "cuisine"  << "Italian"
        << "grades"   << open_array
            << open_document
                << "date" << bsoncxx::types::b_date{12323}
                << "grade" << "A"
                << "score" << 11 << close_document
            << open_document
                << "date" << bsoncxx::types::b_date{121212}
                << "grade" << "B"
                << "score" << 17 << close_document << close_array
        << "name" << "Vella"
        << "restaurant_id" << "41704620" << finalize;
    auto res = db["restaurants"].insert_one(restaurant_doc);
    // @end: cpp-insert-a-document
}
