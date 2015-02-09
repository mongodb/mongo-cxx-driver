#include <iostream>

#include <bsoncxx/builder.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>

namespace helpers = bsoncxx::builder::helpers;

int main(int, char**) {
    mongocxx::client conn{};

    auto db = conn["test"];

    // Query for all the documents in a collection.
    {
        // @begin: cpp-query-all
        auto cursor = db["restaurants"].find({});
        for (auto&& doc : cursor) {
           std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-all
    }
    
    // Query for equality on a top level field.
    {
        // @begin: cpp-query-top-level-field
        bsoncxx::builder::document filter;
        filter << "borough" << "Manhattan";

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-top-level-field
    }

    // Query by a field in an embedded document.
    {
        // @begin: cpp-query-embedded-document
        bsoncxx::builder::document filter;
        filter << "address.zipcode" << "10075";

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-embedded-document
    }

    // Query by a field in an array.
    {
        // @begin: cpp-query-field-in-array
        bsoncxx::builder::document filter;
        filter << "grades.grade" << "B";

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-field-in-array
    }

    // Query with the greater-than operator ($gt).
    // TODO: need to bikeshed how to indent this
    {
        // @begin: cpp-query-greater-than
        bsoncxx::builder::document filter;
        filter << "grades.score"
               << helpers::open_doc
                   << "$gt" << 30
               << helpers::close_doc;

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-greater-than
    }

    // Query with the less-than operator ($lt).
    {
        // @begin: cpp-query-less-than
        bsoncxx::builder::document filter;
        filter << "grades.score"
               << helpers::open_doc
                   << "$lt" << 10
               << helpers::close_doc;

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-less-than
    }

    // Query with a logical conjunction (AND) of query conditions.
    {
        // @begin: cpp-query-logical-and
        bsoncxx::builder::document filter;
        filter << "cuisine" << "Italian"
               << "address.zipcode" << "10075";

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-and
    }
    
    // Query with a logical disjunction (OR) of query conditions.
    {
        // @begin: cpp-query-logical-or
        bsoncxx::builder::document filter;
        filter << "$or"
               << helpers::open_array
                   << helpers::open_doc
                       << "cuisine" << "Italian"
                   << helpers::close_doc
                   << helpers::open_doc
                       << "address.zipcode" << "10075"
                   << helpers::close_doc
               << helpers::close_array;

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-logical-or
    }

    // Sort query results.
    {
        // @begin: cpp-query-sort
        mongocxx::options::find opts;
        bsoncxx::builder::document ordering;
        ordering << "borough" << 1
                 << "address.zipcode" << -1;

        auto cursor = db["restaurants"].find({}, opts);
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-sort
    }
}
