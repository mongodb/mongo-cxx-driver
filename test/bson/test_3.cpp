#include <iostream>
#include <memory>
#include <cstring>
#include <vector>

#include "bson/builder.hpp"
#include "driver/private/libmongoc.hpp"

using namespace mongo::driver;

int main() {
    mongoc_init();

#if 0
    bson::builder builder;

    bson::document::view doc(builder.view());

    client client("mongodb://localhost");
    collection col(client["test"]["test"]);


    libmongoc::collection_drop.interpose([](mongoc_collection_t *, bson_error_t *) {
                                             std::cout << "interposed drop" << std::endl;

                                             return true;
                                         }).times(3);

    libmongoc::collection_drop.visit([]() { std::cout << "visited drop" << std::endl; }).times(2);

    libmongoc::collection_find.interpose([]() {
                                             std::cout << "interposed find" << std::endl;
                                             return nullptr;
                                         }).times(1);

    libmongoc::collection_find.interpose((mongoc_cursor_t *)nullptr)
        .drop_when([](mongoc_collection_t *collection, mongoc_query_flags_t flags, uint32_t skip,
                      uint32_t limit, uint32_t batch_size, const bson_t *query,
                      const bson_t *fields, const mongoc_read_prefs_t *read_prefs) {
             std::cout << "interposing find...\n";
             return skip == 0;
         });

    libmongoc::collection_find.interpose((mongoc_cursor_t *)nullptr, (mongoc_cursor_t *)nullptr,
                                         (mongoc_cursor_t *)nullptr, (mongoc_cursor_t *)nullptr);

    col.drop();
    col.drop();
    col.drop();
    col.drop();
    col.drop();
    col.drop();

    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc).skip(1));
    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc));
    col.find(model::find().criteria(doc));

    for (int i = 0; i < 10; i++) {
        builder.clear();

        builder << "foo" << i;
    }

    /*
     *        std::cout << "individual write is: " <<
     *col.insert(InsertModel(bson::document::view(bson_get_data(&bson),
     *bson.len))) << std::endl;
     *    }
     *    bson_destroy(&bson);
     *
     *    std::vector<InsertRequest> requests;
     *
     *    bson_t bsons[10];
     *
     *    for (int i = 0; i < 10; i++) {
     *        bson_init(bsons + i);
     *
     *        BSON_APPEND_INT32(bsons + i, "foo", i + 10);
     *
     *        requests.emplace_back(bson::document::view(bson_get_data(bsons +
     *i), bsons[i].len));
     *    }
     *
     *    std::cout << "bulk write is: " <<
     *col.bulk_write(make_bulk_write_model(requests, false)) << std::endl;
     *
     *    for (int i = 0; i < 10; i++) {
     *        bson_destroy(bsons + i);
     *    }
     *
     *    bson_init(&bson);
     *    for (int i = 0; i < 10; i++) {
     *        bson_t child;
     *        bson_append_document_begin(&bson, "-", 1, &child);
     *        BSON_APPEND_INT32(&child, "foo", i + 20);
     *        bson_append_document_end(&bson, &child);
     *    }
     *    bson::document::view bson_array(bson_get_data(&bson), bson.len);
     *
     *    auto adapter = make_adapter(&bson_array, [] (const bson::Reference&
     *ref) {
     *         return InsertRequest(ref.getDocument());
     *    });
     *
     *    std::cout << "bulk write w/ adapter is: " <<
     *col.bulk_write(make_bulk_write_model(adapter, false)) << std::endl;
     *
     *    Cursor cursor(col.find(FindModel(doc)));
     *
     *    for (auto x : cursor) {
     *        std::cout << "bson is: " << x["foo"].getInt32() << std::endl;
     *    }
     */

    /*
for (; x != col.end(); ++x) {
std::cout << "bson is: " << *x << std::endl;
}
*/
#endif

    mongoc_cleanup();
    return 0;
}
