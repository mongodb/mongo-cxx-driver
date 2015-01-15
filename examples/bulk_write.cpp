#include "mongocxx/driver/client.hpp"
#include "mongocxx/driver/collection.hpp"
#include "mongocxx/driver/bulk_write.hpp"
#include "mongocxx/driver/model/insert_one.hpp"
#include "bsoncxx/document/builder.hpp"

using namespace bson;
using namespace mongo;

///
/// How to create and execute bulk writes against MongoDB
/// @example bulk_write.cpp
///
int main() {
    // Connect and get a collection
    client mongo_client;
    auto coll = mongo_client["database"]["collection"];

    // Build document to insert (_id will be generated as none was provided)
    document::builder doc;
    doc << "x" << true;

    // Construct the write operations (can mix inserts / updates / deletes, etc...)
    model::insert_one insert_one(doc);
    model::insert_one insert_two(doc);

    // Build the bulk_write operation (to be executed in order because of the first argument)
    bulk_write writes(true);
    writes.append(insert_one);
    writes.append(insert_two);

    // Execute the bulk operation
    auto result = collection.bulk_write(writes);
}
