// Copyright 2018-present MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <mongocxx/test/spec/operation.hh>

#include <fstream>
#include <functional>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/collection.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/options/delete.hpp>
#include <mongocxx/options/distinct.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/find_one_and_delete.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>
#include <mongocxx/options/find_one_and_update.hpp>
#include <mongocxx/options/find_one_common_options.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <mongocxx/test_util/client_helpers.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;
using namespace bsoncxx;

int64_t as_int64(const document::element& el) {
    if (el.type() == type::k_int32) {
        return static_cast<std::int64_t>(el.get_int32().value);
    } else if (el.type() == type::k_int64) {
        return el.get_int64().value;
    }
    REQUIRE(false);
    return 0;
}

pipeline build_pipeline(array::view pipeline_docs) {
    pipeline pipeline{};

    for (auto&& element : pipeline_docs) {
        document::view document = element.get_document();

        if (document["$match"]) {
            pipeline.match(document["$match"].get_document().value);
        } else if (document["$out"]) {
            pipeline.out(bsoncxx::string::to_string(document["$out"].get_utf8().value));
        } else if (document["$sort"]) {
            pipeline.sort(document["$sort"].get_document().value);
        } else {
            throw std::logic_error{"unsupported pipeline stage" + to_json(document)};
        }
    }

    return pipeline;
}

document::value run_aggregate_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    pipeline pipeline = build_pipeline(arguments["pipeline"].get_array().value);
    options::aggregate options{};

    if (arguments["batchSize"]) {
        options.batch_size(arguments["batchSize"].get_int32().value);
    }

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    cursor result_cursor = coll->aggregate(pipeline, options);

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& document : result_cursor) {
            array.append(document);
        }
    }));

    return result.extract();
}

document::value run_count_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::count options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["limit"]) {
        options.limit(arguments["limit"].get_int32().value);
    }

    if (arguments["skip"]) {
        options.skip(arguments["skip"].get_int32().value);
    }

    int64_t count = coll->count_deprecated(filter, options);

    auto result = builder::basic::document{};

    // The JSON parser reads the counts as int32, so the document will not be equal to the expected
    // result without casting.
    result.append(builder::basic::kvp("result", static_cast<std::int32_t>(count)));

    return result.extract();
}

document::value run_distinct_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter{};

    if (arguments["filter"]) {
        filter = arguments["filter"].get_document().value;
    }

    bsoncxx::stdx::string_view field_name = arguments["fieldName"].get_utf8().value;

    options::distinct options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    cursor result_cursor = coll->distinct(field_name, filter, options);

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& result_doc : result_cursor) {
            for (auto&& value : result_doc["values"].get_array().value) {
                array.append(value.get_value());
            }
        }
    }));

    return result.extract();
}

document::value run_find_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::find options{};

    if (arguments["batchSize"]) {
        options.batch_size(static_cast<std::int32_t>(as_int64(arguments["batchSize"])));
    }

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["limit"]) {
        options.limit(as_int64(arguments["limit"]));
    }

    if (arguments["skip"]) {
        options.skip(as_int64(arguments["skip"]));
    }

    if (arguments["sort"]) {
        options.sort(arguments["sort"].get_document().value);
    }

    if (arguments["modifiers"]) {
        document::view modifiers = arguments["modifiers"].get_document().value;
        if (modifiers["$comment"]) {
            options.comment(modifiers["$comment"].get_utf8().value);
        }

        if (modifiers["$hint"]) {
            hint my_hint(modifiers["$hint"].get_document().value);
            options.hint(my_hint);
        }

        if (modifiers["$max"]) {
            options.max(modifiers["$max"].get_document().value);
        }

        if (modifiers["$maxTimeMS"]) {
            std::chrono::milliseconds my_millis(modifiers["$maxTimeMS"].get_int32().value);
            options.max_time(my_millis);
        }

        if (modifiers["$min"]) {
            options.min(modifiers["$min"].get_document().value);
        }

        if (modifiers["$returnKey"]) {
            options.return_key(modifiers["$returnKey"].get_bool().value);
        }

        if (modifiers["$showDiskLoc"]) {
            options.show_record_id(modifiers["$showDiskLoc"].get_bool().value);
        }
    }

    cursor result_cursor = coll->find(filter, options);

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& document : result_cursor) {
            array.append(document);
        }
    }));

    return result.extract();
}

document::value run_delete_many_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::delete_options options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    auto result = builder::basic::document{};
    std::int32_t deleted_count = 0;

    if (auto delete_result = coll->delete_many(filter, options)) {
        deleted_count = delete_result->deleted_count();
    }

    result.append(
        builder::basic::kvp("result", [deleted_count](builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("deletedCount", deleted_count));

        }));

    return result.extract();
}

document::value run_delete_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::delete_options options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    auto result = builder::basic::document{};
    std::int32_t deleted_count = 0;

    if (auto delete_result = coll->delete_one(filter, options)) {
        deleted_count = delete_result->deleted_count();
    }

    result.append(
        builder::basic::kvp("result", [deleted_count](builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("deletedCount", deleted_count));

        }));

    return result.extract();
}

document::value run_find_one_and_delete_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::find_one_and_delete options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["projection"]) {
        options.projection(arguments["projection"].get_document().value);
    }

    if (arguments["sort"]) {
        options.sort(arguments["sort"].get_document().value);
    }

    auto result = builder::basic::document{};
    auto document = coll->find_one_and_delete(filter, options);

    // Server versions below 3.0 sometimes return an empty document rather than null when no
    // documents match.
    if (document && !(document->view().empty())) {
        result.append(builder::basic::kvp("result", *document));
    } else {
        result.append(builder::basic::kvp("result", types::b_null{}));
    }

    return result.extract();
}

document::value run_find_one_and_replace_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view replacement = arguments["replacement"].get_document().value;
    options::find_one_and_replace options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["projection"]) {
        options.projection(arguments["projection"].get_document().value);
    }

    if (arguments["returnDocument"]) {
        std::string return_document =
            bsoncxx::string::to_string(arguments["returnDocument"].get_utf8().value);

        if (return_document == "After") {
            options.return_document(options::return_document::k_after);
        }

        if (return_document == "Before") {
            options.return_document(options::return_document::k_before);
        }
    }

    if (arguments["sort"]) {
        options.sort(arguments["sort"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    auto result = builder::basic::document{};
    auto document = coll->find_one_and_replace(filter, replacement, options);

    // Server versions below 3.0 sometimes return an empty document rather than null when no
    // documents match.
    if (document && !(document->view().empty())) {
        result.append(builder::basic::kvp("result", *document));
    } else {
        result.append(builder::basic::kvp("result", types::b_null{}));
    }

    return result.extract();
}

document::value run_find_one_and_update_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view update = arguments["update"].get_document().value;
    options::find_one_and_update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["projection"]) {
        options.projection(arguments["projection"].get_document().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    if (arguments["returnDocument"]) {
        std::string return_document =
            bsoncxx::string::to_string(arguments["returnDocument"].get_utf8().value);

        if (return_document == "After") {
            options.return_document(options::return_document::k_after);
        }

        if (return_document == "Before") {
            options.return_document(options::return_document::k_before);
        }
    }

    if (arguments["sort"]) {
        options.sort(arguments["sort"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    auto result = builder::basic::document{};
    auto document = coll->find_one_and_update(filter, update, options);

    // Server versions below 3.0 sometimes return an empty document rather than null when no
    // documents match.
    if (document && !(document->view().empty())) {
        result.append(builder::basic::kvp("result", *document));
    } else {
        result.append(builder::basic::kvp("result", types::b_null{}));
    }

    return result.extract();
}

document::value run_insert_many_test(collection* coll, document::view operation) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    document::view arguments = operation["arguments"].get_document().value;
    array::view documents = arguments["documents"].get_array().value;
    std::vector<document::view> documents_to_insert{};
    options::insert insert_options;

    if (arguments["options"]) {
        document::view options = arguments["options"].get_document().value;
        if (options["ordered"]) {
            insert_options.ordered(options["ordered"].get_bool().value);
        }
    }

    for (auto&& element : documents) {
        documents_to_insert.push_back(element.get_document().value);
    }

    auto insert_many_result = coll->insert_many(documents_to_insert, insert_options);
    std::map<size_t, document::element> inserted_ids{};

    if (insert_many_result) {
        inserted_ids = insert_many_result->inserted_ids();
    }

    auto result = builder::basic::document{};
    bsoncxx::builder::basic::document inserted_ids_builder;

    for (auto&& id_pair : inserted_ids) {
        inserted_ids_builder.append(kvp(std::to_string(id_pair.first), id_pair.second.get_value()));
    }

    result.append(kvp("result", make_document(kvp("insertedIds", inserted_ids_builder.extract()))));
    return result.extract();
}

document::value run_insert_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view document = arguments["document"].get_document().value;
    auto result = builder::basic::document{};
    auto insert_one_result = coll->insert_one(document);
    types::value inserted_id{types::b_null{}};

    if (insert_one_result) {
        inserted_id = insert_one_result->inserted_id();
    }

    result.append(builder::basic::kvp("result", [inserted_id](builder::basic::sub_document subdoc) {
        subdoc.append(builder::basic::kvp("insertedId", inserted_id));

    }));

    return result.extract();
}

document::value run_replace_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view replacement = arguments["replacement"].get_document().value;
    options::replace options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    auto update_result = coll->replace_one(filter, replacement, options);
    bsoncxx::stdx::optional<types::value> upserted_id{};

    if (update_result) {
        matched_count = update_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = update_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = update_result->result().upserted_count();

        if (auto upserted_element = update_result->upserted_id()) {
            upserted_id = upserted_element->get_value();
        }
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp(
        "result",
        [matched_count, modified_count, upserted_count, upserted_id](
            builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("matchedCount", matched_count));

            if (modified_count) {
                subdoc.append(builder::basic::kvp("modifiedCount", *modified_count));
            }

            subdoc.append(builder::basic::kvp("upsertedCount", upserted_count));

            if (upserted_id) {
                subdoc.append(builder::basic::kvp("upsertedId", *upserted_id));
            }
        }));

    return result.extract();
}

document::value run_update_many_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view update = arguments["update"].get_document().value;
    options::update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    auto update_result = coll->update_many(filter, update, options);
    bsoncxx::stdx::optional<types::value> upserted_id{};

    if (update_result) {
        matched_count = update_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = update_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = update_result->result().upserted_count();

        if (auto upserted_element = update_result->upserted_id()) {
            upserted_id = upserted_element->get_value();
        }
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp(
        "result",
        [matched_count, modified_count, upserted_count, upserted_id](
            builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("matchedCount", matched_count));

            if (modified_count) {
                subdoc.append(builder::basic::kvp("modifiedCount", *modified_count));
            }

            subdoc.append(builder::basic::kvp("upsertedCount", upserted_count));

            if (upserted_id) {
                subdoc.append(builder::basic::kvp("upsertedId", *upserted_id));
            }
        }));

    return result.extract();
}

document::value run_update_one_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view update = arguments["update"].get_document().value;
    options::update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    auto update_result = coll->update_one(filter, update, options);
    bsoncxx::stdx::optional<types::value> upserted_id{};

    if (update_result) {
        matched_count = update_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = update_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = update_result->result().upserted_count();

        if (auto upserted_element = update_result->upserted_id()) {
            upserted_id = upserted_element->get_value();
        }
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp(
        "result",
        [matched_count, modified_count, upserted_count, upserted_id](
            builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("matchedCount", matched_count));

            if (modified_count) {
                subdoc.append(builder::basic::kvp("modifiedCount", *modified_count));
            }

            subdoc.append(builder::basic::kvp("upsertedCount", upserted_count));

            if (upserted_id) {
                subdoc.append(builder::basic::kvp("upsertedId", *upserted_id));
            }
        }));

    return result.extract();
}

document::value run_bulk_write_test(collection* coll, document::view operation) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    options::bulk_write options;
    std::vector<model::write> writes;
    if (operation["collectionOptions"]) {
        // Set write concern from collection options.
        document::view collection_options = operation["collectionOptions"].get_document().value;
        document::view write_concern_options =
            collection_options["writeConcern"].get_document().value;
        int32_t w_option = write_concern_options["w"].get_int32().value;
        write_concern w;
        w.nodes(w_option);
        options.write_concern(w);
    }
    auto arguments = operation["arguments"].get_document().value;
    if (arguments["options"]) {
        document::view options_doc = arguments["options"].get_document().value;
        if (options_doc["ordered"]) {
            options.ordered(options_doc["ordered"].get_bool().value);
        }
    }
    auto requests = arguments["requests"].get_array().value;
    for (auto&& request_element : requests) {
        auto request = request_element.get_document().value;
        auto request_arguments = request["arguments"].get_document().value;
        auto operation_name = request["name"].get_utf8().value;
        if (operation_name.compare("updateOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view update = request_arguments["update"].get_document().value;
            model::update_one update_one(filter, update);

            if (request_arguments["collation"]) {
                update_one.collation(request_arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                update_one.upsert(request_arguments["upsert"].get_bool().value);
            }

            if (request_arguments["arrayFilters"]) {
                update_one.array_filters(request_arguments["arrayFilters"].get_array().value);
            }

            writes.emplace_back(update_one);
        } else if (operation_name.compare("updateMany") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view update = request_arguments["update"].get_document().value;
            model::update_many update_many(filter, update);

            if (request_arguments["collation"]) {
                update_many.collation(request_arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                update_many.upsert(request_arguments["upsert"].get_bool().value);
            }

            if (request_arguments["arrayFilters"]) {
                update_many.array_filters(request_arguments["arrayFilters"].get_array().value);
            }

            writes.emplace_back(update_many);
        } else if (operation_name.compare("replaceOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            document::view replacement = request_arguments["replacement"].get_document().value;
            model::replace_one replace_one(filter, replacement);
            if (request_arguments["collation"]) {
                replace_one.collation(arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                replace_one.upsert(arguments["upsert"].get_bool().value);
            }

            writes.emplace_back(replace_one);
        } else if (operation_name.compare("insertOne") == 0) {
            document::view document = request_arguments["document"].get_document().value;
            model::insert_one insert_one(document);
            writes.emplace_back(insert_one);
        } else if (operation_name.compare("deleteOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_one delete_one(filter);
            if (request_arguments["collation"]) {
                delete_one.collation(arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_one);
        } else if (operation_name.compare("deleteMany") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_many delete_many(filter);
            if (request_arguments["collation"]) {
                delete_many.collation(arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_many);
        } else {
            /* should not happen. */
            REQUIRE(false);
        }
    }

    std::int32_t deleted_count = 0;
    std::int32_t matched_count = 0;
    std::int32_t modified_count = 0;
    std::int32_t upserted_count = 0;
    result::bulk_write::id_map upserted_ids;
    std::int32_t inserted_count = 0;
    auto bulk_write_result = coll->bulk_write(writes, options);
    if (bulk_write_result) {
        matched_count = bulk_write_result->matched_count();
        modified_count = bulk_write_result->modified_count();
        upserted_count = bulk_write_result->upserted_count();
        upserted_ids = bulk_write_result->upserted_ids();
        inserted_count = bulk_write_result->inserted_count();
        deleted_count = bulk_write_result->deleted_count();
    }
    bsoncxx::builder::basic::document upserted_ids_builder;
    for (auto&& index_and_id : upserted_ids) {
        upserted_ids_builder.append(
            kvp(std::to_string(index_and_id.first), index_and_id.second.get_document().value));
    }
    auto upserted_ids_doc = upserted_ids_builder.extract();
    auto result = bsoncxx::builder::basic::document{};
    // Construct the result document.
    // Note: insertedIds is currently hard coded as an empty document, because result::bulk_write
    // provides no way to see inserted ids. This is compliant with the CRUD spec, as insertedIds
    // are: "NOT REQUIRED: Drivers may choose to not provide this property." So just add an empty
    // document for insertedIds. There are no current bulk write tests testing insert operations.
    // The insertedIds field in current bulk write spec tests is always an empty document.
    result.append(kvp("result",
                      make_document(kvp("matchedCount", matched_count),
                                    kvp("modifiedCount", modified_count),
                                    kvp("upsertedCount", upserted_count),
                                    kvp("deletedCount", deleted_count),
                                    kvp("insertedCount", inserted_count),
                                    kvp("insertedIds", make_document()),
                                    kvp("upsertedIds", upserted_ids_doc))));
    return result.extract();
}

document::value run_count_documents_test(collection* coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::count options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["limit"]) {
        options.limit(arguments["limit"].get_int32().value);
    }

    if (arguments["skip"]) {
        options.skip(arguments["skip"].get_int32().value);
    }

    int64_t count = coll->count_documents(filter, options);

    auto result = builder::basic::document{};

    // The JSON parser reads the counts as int32, so the document will not be equal to the expected
    // result without casting.
    result.append(builder::basic::kvp("result", static_cast<std::int32_t>(count)));

    return result.extract();
}

document::value run_estimated_document_count_test(collection* coll, document::view) {
    int64_t count = coll->estimated_document_count();

    auto result = builder::basic::document{};

    // The JSON parser reads the counts as int32, so the document will not be equal to the expected
    // result without casting.
    result.append(builder::basic::kvp("result", static_cast<std::int32_t>(count)));

    return result.extract();
}

spec::test_runners get_test_runners() {
    static spec::test_runners runners = {
        {"aggregate", run_aggregate_test},
        {"count", run_count_test},
        {"countDocuments", run_count_documents_test},
        {"estimatedDocumentCount", run_estimated_document_count_test},
        {"distinct", run_distinct_test},
        {"find", run_find_test},
        {"deleteMany", run_delete_many_test},
        {"deleteOne", run_delete_one_test},
        {"findOneAndDelete", run_find_one_and_delete_test},
        {"findOneAndReplace", run_find_one_and_replace_test},
        {"findOneAndUpdate", run_find_one_and_update_test},
        {"insertMany", run_insert_many_test},
        {"insertOne", run_insert_one_test},
        {"replaceOne", run_replace_one_test},
        {"updateMany", run_update_many_test},
        {"updateOne", run_update_one_test},
        {"bulkWrite", run_bulk_write_test}};
    return runners;
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
