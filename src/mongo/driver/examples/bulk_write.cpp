#include "mongo/bson/builder.hpp"
#include "mongo/driver/client.hpp"
#include "mongo/driver/collection.hpp"
#include "mongo/driver/bulk_write.hpp"
#include "mongo/driver/model/insert_one.hpp"

using namespace mongo::bson;

///
/// How to create and execute bulk writes against MongoDB
/// @example bulk_write.cpp
///
int main() {
    // Connect and get a collection
    client mongo_client;
    auto coll = mongo_client["database"]["collection"];

    // Build document to insert ('_id' field will be generated as none was provided)
    document::builder doc;
    doc << "x" << true;

    // Construct the write operations (can mix inserts / updates / deletes, etc...)
    model::insert_one insert_one(doc);
    model::insert_one insert_two(doc);

    // Create the bulk_write operation (to be executed in order because we pass 'true')
    bulk_write writes(true);

    // Append the write operations we constructed above
    writes.append(insert_one);
    writes.append(insert_two);

    // Execute the bulk operation
    auto result = collection.bulk_write(writes);
}
