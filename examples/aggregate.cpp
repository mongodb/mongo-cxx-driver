#include <iostream>

#include <bsoncxx/builder.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/pipeline.hpp>

using namespace bsoncxx::builder::helpers;
using bsoncxx::builder::document;

int main(int, char**) {
    mongocxx::client conn{};
    
    auto db = conn["test"];

    // Group documents by field and calculate count.
    {
        // @begin: cpp-group-documents-by-a-field-and-calculate-count
        mongocxx::pipeline stages;
        document group_stage;

        group_stage << "_id" << "$borough"
                    << "count" << open_doc
                        << "$sum" << 1 << close_doc;
        
        stages.group(group_stage);
        
        auto cursor = db["restaurants"].aggregate(stages);
        
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-group-documents-by-a-field-and-calculate-count
    }

    // Filter and group documents.
    {
        // @begin: cpp-filter-and-group-documents
        mongocxx::pipeline stages;
        document match_stage, group_stage;

        match_stage << "borough" << "Queens"
                    << "cuisine" << "Brazilian";

        group_stage << "_id" << "$address.zipcode"
                    << "count" << open_doc
                        << "$sum" << 1 << close_doc;

        stages.match(match_stage).group(group_stage);

        auto cursor = db["restaurants"].aggregate(stages);

        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
        // @end: cpp-filter-and-group-documents
    }
}
