#include <iostream>

#include <bsoncxx/builder.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>

using namespace bsoncxx::builder::helpers;
using bsoncxx::builder::document;

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
        document filter;
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
        document filter;
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
        document filter;
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
        document filter;
        filter << "grades.score" << open_doc
                   << "$gt" << 30 << close_doc;

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-greater-than
    }

    // Query with the less-than operator ($lt).
    {
        // @begin: cpp-query-less-than
        document filter;
        filter << "grades.score" << open_doc
                   << "$lt" << 10 << close_doc;

        auto cursor = db["restaurants"].find(filter.view());
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-less-than
    }

    // Query with a logical conjunction (AND) of query conditions.
    {
        // @begin: cpp-query-logical-and
        document filter;
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
        document filter;
        filter << "$or" << open_array
                   << open_doc
                       << "cuisine" << "Italian" << close_doc
                   << open_doc
                      << "address.zipcode" << "10075" << close_doc 
               << close_array;

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
        document ordering;
        ordering << "borough" << 1
                 << "address.zipcode" << -1;

        auto cursor = db["restaurants"].find({}, opts);
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-query-sort
    }
}
