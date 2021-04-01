// Copyright 2020 MongoDB Inc.
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

#include "operations.hh"

#include <catch.hpp>
#include <sstream>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/test/spec/monitoring.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using namespace bsoncxx;

using builder::basic::kvp;
using builder::basic::make_document;

int64_t as_int64(const document::element& el) {
    if (el.type() == type::k_int32)
        return static_cast<std::int64_t>(el.get_int32().value);
    return el.get_int64().value;
}

int32_t as_int32(const document::element& el) {
    if (el.type() == type::k_int64)
        return static_cast<std::int32_t>(el.get_int64().value);
    return el.get_int32().value;
}

document::value find_one(collection& coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::value empty_filter = builder::basic::make_document();
    document::view filter;
    if (arguments["filter"]) {
        filter = arguments["filter"].get_document().value;
    } else {
        filter = empty_filter.view();
    }
    options::find options{};

    if (arguments["batchSize"]) {
        options.batch_size(as_int32(arguments["batchSize"]));
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

    if (arguments["allowDiskUse"]) {
        options.allow_disk_use(arguments["allowDiskUse"].get_bool().value);
    }

    if (arguments["modifiers"]) {
        document::view modifiers = arguments["modifiers"].get_document().value;
        if (modifiers["$comment"]) {
            options.comment(modifiers["$comment"].get_string().value);
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

    stdx::optional<cursor> result_cursor;
    result_cursor.emplace(coll.find(filter, options));

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& document : *result_cursor) {
            array.append(document);
        }
    }));

    return result.extract();
}

template <typename Model>
void add_hint_to_model(Model model, document::view doc) {
    if (doc["hint"]) {
        if (doc["hint"].type() == bsoncxx::v_noabi::type::k_string)
            model.hint(hint{doc["hint"].get_string().value});
        else
            model.hint(hint{doc["hint"].get_document().value});
    }
};

pipeline build_pipeline(array::view pipeline_docs) {
    pipeline pipeline{};
    pipeline.append_stages(std::move(pipeline_docs));
    return pipeline;
}

template <typename T>
T _build_update_model(document::view arguments) {
    document::view filter = arguments["filter"].get_document().value;

    switch (arguments["update"].type()) {
        case bsoncxx::type::k_document: {
            return T(filter, arguments["update"].get_document().value);
        }
        case bsoncxx::type::k_array: {
            pipeline update = build_pipeline(arguments["update"].get_array().value);
            return T(filter, update);
        }
        default:
            throw std::logic_error{"update must be a document or an array"};
    }
}

document::value bulk_write(collection& coll, document::view op) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    options::bulk_write opt;
    std::vector<model::write> writes;

    auto arguments = op["arguments"].get_document().value;
    if (arguments["options"]) {
        document::view options_doc = arguments["options"].get_document().value;
        if (options_doc["ordered"]) {
            opt.ordered(options_doc["ordered"].get_bool().value);
        }
    }

    auto requests = arguments["requests"].get_array().value;
    for (auto&& request_element : requests) {
        auto operation_name = request_element.get_document().value.begin()->key();
        auto request_arguments = request_element[operation_name].get_document().value;
        CAPTURE(to_json(request_arguments), operation_name);

        if (operation_name.compare("updateOne") == 0) {
            auto update_one = _build_update_model<model::update_one>(request_arguments);

            add_hint_to_model(update_one, request_arguments);

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
            auto update_many = _build_update_model<model::update_many>(request_arguments);

            add_hint_to_model(update_many, request_arguments);

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

            add_hint_to_model(replace_one, request_arguments);

            if (request_arguments["collation"]) {
                replace_one.collation(request_arguments["collation"].get_document().value);
            }

            if (request_arguments["upsert"]) {
                replace_one.upsert(request_arguments["upsert"].get_bool().value);
            }

            writes.emplace_back(replace_one);
        } else if (operation_name.compare("insertOne") == 0) {
            document::view document = request_arguments["document"].get_document().value;
            model::insert_one insert_one(document);
            writes.emplace_back(insert_one);
        } else if (operation_name.compare("deleteOne") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_one delete_one(filter);
            add_hint_to_model(delete_one, request_arguments);

            if (request_arguments["collation"]) {
                delete_one.collation(request_arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_one);
        } else if (operation_name.compare("deleteMany") == 0) {
            document::view filter = request_arguments["filter"].get_document().value;
            model::delete_many delete_many(filter);
            add_hint_to_model(delete_many, request_arguments);

            if (request_arguments["collation"]) {
                delete_many.collation(request_arguments["collation"].get_document().value);
            }

            writes.emplace_back(delete_many);
        } else {
            /* should not happen. */
            FAIL("unrecognized operation name: " + operation_name.to_string());
        }
    }

    std::int32_t deleted_count = 0;
    std::int32_t matched_count = 0;
    std::int32_t modified_count = 0;
    std::int32_t upserted_count = 0;
    result::bulk_write::id_map upserted_ids;
    std::int32_t inserted_count = 0;
    bsoncxx::stdx::optional<result::bulk_write> bulk_write_result;
    bulk_write_result = coll.bulk_write(writes, opt);
    if (bulk_write_result) {
        matched_count = bulk_write_result->matched_count();
        modified_count = bulk_write_result->modified_count();
        upserted_count = bulk_write_result->upserted_count();
        upserted_ids = bulk_write_result->upserted_ids();
        inserted_count = bulk_write_result->inserted_count();
        deleted_count = bulk_write_result->deleted_count();
    }
    builder::basic::document upserted_ids_builder;
    for (auto&& index_and_id : upserted_ids) {
        upserted_ids_builder.append(
            kvp(std::to_string(index_and_id.first), index_and_id.second.get_int32().value));
    }
    auto upserted_ids_doc = upserted_ids_builder.extract();

    // Construct the result document.
    // Note: insertedIds is currently hard coded as an empty document, because result::bulk_write
    // provides no way to see inserted ids. This is compliant with the CRUD spec, as insertedIds
    // are: "NOT REQUIRED: Drivers may choose to not provide this property." So just add an empty
    // document for insertedIds. There are no current bulk write tests testing insert operations.
    // The insertedIds field in current bulk write spec tests is always an empty document.
    auto result = make_document(kvp("matchedCount", matched_count),
                                kvp("modifiedCount", modified_count),
                                kvp("upsertedCount", upserted_count),
                                kvp("deletedCount", deleted_count),
                                kvp("insertedCount", inserted_count),
                                kvp("insertedIds", make_document()),
                                kvp("upsertedIds", upserted_ids_doc));
    return make_document(kvp("result", result));
}

document::value insert_many(collection& coll, document::view operation) {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    document::view arguments = operation["arguments"].get_document().value;
    array::view documents = arguments["documents"].get_array().value;
    std::vector<document::view> documents_to_insert{};
    options::insert insert_options;

    if (arguments["ordered"]) {
        insert_options.ordered(arguments["ordered"].get_bool().value);
    }

    for (auto&& element : documents) {
        documents_to_insert.push_back(element.get_document().value);
    }

    stdx::optional<result::insert_many> insert_many_result;
    insert_many_result = coll.insert_many(documents_to_insert, insert_options);
    std::map<size_t, document::element> inserted_ids{};

    if (insert_many_result) {
        inserted_ids = insert_many_result->inserted_ids();
    }

    auto result = builder::basic::document{};
    bsoncxx::builder::basic::document inserted_ids_builder;

    for (auto&& id_pair : inserted_ids) {
        inserted_ids_builder.append(kvp(std::to_string(id_pair.first), id_pair.second.get_value()));
    }

    result.append(kvp("result",
                      make_document(kvp("insertedIds", inserted_ids_builder.extract()),
                                    kvp("insertedCount", insert_many_result->inserted_count()))));
    return result.extract();
}

document::value replace_one(collection& coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    document::view replacement = arguments["replacement"].get_document().value;
    options::replace options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["hint"]) {
        if (arguments["hint"].type() == bsoncxx::v_noabi::type::k_string)
            options.hint(hint{arguments["hint"].get_string().value});
        else
            options.hint(hint{arguments["hint"].get_document().value});
    }

    if (arguments["upsert"]) {
        options.upsert(arguments["upsert"].get_bool().value);
    }

    std::int32_t matched_count = 0;
    bsoncxx::stdx::optional<std::int32_t> modified_count;
    std::int32_t upserted_count = 0;
    stdx::optional<result::replace_one> replace_result;

    replace_result = coll.replace_one(filter, replacement, options);
    bsoncxx::stdx::optional<types::bson_value::view> upserted_id{};

    if (replace_result) {
        matched_count = replace_result->matched_count();

        // Server versions below 2.4 do not return an `nModified` count.
        try {
            modified_count = replace_result->modified_count();
        } catch (const std::exception& e) {
        }

        upserted_count = replace_result->result().upserted_count();

        if (auto upserted_element = replace_result->upserted_id()) {
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

template <typename Entity>
document::value aggregate(Entity& entity, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    pipeline pipeline = build_pipeline(arguments["pipeline"].get_array().value);
    options::aggregate options{};

    if (arguments["batchSize"]) {
        options.batch_size(arguments["batchSize"].get_int32().value);
    }

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    stdx::optional<cursor> result_cursor;
    result_cursor.emplace(entity.aggregate(pipeline, options));

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [&result_cursor](builder::basic::sub_array array) {
        for (auto&& document : *result_cursor) {
            array.append(document);
        }
    }));

    return result.extract();
}

document::value insert_one(collection& coll, client_session* session, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view document = arguments["document"].get_document().value;

    options::insert opts{};
    if (arguments["bypassDocumentValidation"]) {
        opts.bypass_document_validation(true);
    }

    stdx::optional<result::insert_one> insert_one_result;
    if (session) {
        insert_one_result = coll.insert_one(*session, document, opts);
    } else {
        insert_one_result = coll.insert_one(document, opts);
    }

    types::bson_value::view inserted_id{types::b_null{}};

    if (insert_one_result) {
        inserted_id = insert_one_result->inserted_id();
    }

    auto result = builder::basic::document{};
    result.append(builder::basic::kvp("result", [inserted_id](builder::basic::sub_document subdoc) {
        subdoc.append(builder::basic::kvp("insertedId", inserted_id));
    }));

    return result.extract();
}

document::value create_change_stream(entity::map& map,
                                     const std::string& object,
                                     document::view operation) {
    auto args = operation["arguments"];
    auto pipeline = build_pipeline(args["pipeline"].get_array().value);

    options::change_stream options{};
    if (args["batchSize"]) {
        options.batch_size(args["batchSize"].get_int32().value);
    }

    auto key = operation["saveResultAsEntity"].get_string().value.to_string();
    CAPTURE(object, to_json(operation), key);
    auto stream = [&] {
        const auto& type = map.type(object);
        if (type == typeid(mongocxx::database))
            return map.get_database(object).watch(pipeline, options);
        if (type == typeid(mongocxx::collection))
            return map.get_collection(object).watch(pipeline, options);
        return map.get_client(object).watch(pipeline, options);
    }();

    auto res = map.insert(key, std::move(stream));
    REQUIRE(res);
    return make_document();
}

document::value next(change_stream& stream) {
    static std::unordered_map<change_stream*, change_stream::iterator> map;
    if (map.find(&stream) == map.end())
        map[&stream] = stream.begin();

    auto iter = map[&stream];
    auto copy = document::value{*iter};
    ++iter;  // prepare for next call to this function.
    return copy;
}

document::value iterate_until_document_or_error(change_stream& stream) {
    return make_document(kvp("result", next(stream)));
}

document::value fail_point(entity::map& map, spec::apm_checker& apm, document::view op) {
    auto args = op["arguments"];
    auto client_name = args["client"].get_string().value.to_string();
    auto& client = map.get_client(client_name);

    client["admin"].run_command(args["failPoint"].get_document().value);

    auto event = spec::apm_checker::to_event("configureFailPoint");
    apm.set_ignore_command_monitoring_event(event);

    return make_document(kvp("uri", client.uri().to_string()),
                         kvp("failPoint", args["failPoint"]["configureFailPoint"].get_string()));
}

document::value find_one_and_update(collection& coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::find_one_and_update options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["hint"]) {
        if (arguments["hint"].type() == bsoncxx::v_noabi::type::k_string)
            options.hint(hint{arguments["hint"].get_string().value});
        else
            options.hint(hint{arguments["hint"].get_document().value});
    }

    if (arguments["projection"]) {
        options.projection(arguments["projection"].get_document().value);
    }

    if (arguments["arrayFilters"]) {
        options.array_filters(arguments["arrayFilters"].get_array().value);
    }

    if (arguments["returnDocument"]) {
        auto return_document = arguments["returnDocument"].get_string().value.to_string();

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
    stdx::optional<document::value> document;

    switch (arguments["update"].type()) {
        case bsoncxx::type::k_document: {
            document::view update = arguments["update"].get_document().value;
            document = coll.find_one_and_update(filter, update, options);
            break;
        }
        case bsoncxx::type::k_array: {
            pipeline update = build_pipeline(arguments["update"].get_array().value);
            document = coll.find_one_and_update(filter, update, options);
            break;
        }
        default:
            throw std::logic_error{"update must be a document or an array"};
    }

    // Server versions below 3.0 sometimes return an empty document rather than null when no
    // documents match.
    if (document && !(document->view().empty())) {
        result.append(builder::basic::kvp("result", *document));
    } else {
        result.append(builder::basic::kvp("result", types::b_null{}));
    }

    return result.extract();
}

bsoncxx::stdx::optional<read_concern> lookup_read_concern(document::view doc) {
    if (doc["readConcern"] && doc["readConcern"]["level"]) {
        read_concern rc;
        rc.acknowledge_string(doc["readConcern"]["level"].get_string().value.to_string());
        return rc;
    }

    return {};
}

bsoncxx::stdx::optional<write_concern> lookup_write_concern(document::view doc) {
    if (doc["writeConcern"] && doc["writeConcern"]["w"]) {
        write_concern wc;
        document::element w = doc["writeConcern"]["w"];
        if (w.type() == bsoncxx::type::k_string) {
            std::string level = w.get_string().value.to_string();
            if (level.compare("majority") == 0) {
                wc.acknowledge_level(write_concern::level::k_majority);
            } else if (level.compare("acknowledged") == 0) {
                wc.acknowledge_level(write_concern::level::k_acknowledged);
            } else if (level.compare("unacknowledged") == 0) {
                wc.acknowledge_level(write_concern::level::k_unacknowledged);
            }
        } else if (w.type() == bsoncxx::type::k_int32) {
            wc.nodes(w.get_int32());
        }
        return wc;
    }

    return {};
}

bsoncxx::stdx::optional<read_preference> lookup_read_preference(document::view doc) {
    if (doc["readPreference"] && doc["readPreference"]["mode"]) {
        read_preference rp;
        std::string mode = doc["readPreference"]["mode"].get_string().value.to_string();
        if (mode.compare("Primary") == 0) {
            rp.mode(read_preference::read_mode::k_primary);
        } else if (mode.compare("PrimaryPreferred") == 0) {
            rp.mode(read_preference::read_mode::k_primary_preferred);
        } else if (mode.compare("Secondary") == 0) {
            rp.mode(read_preference::read_mode::k_secondary);
        } else if (mode.compare("SecondaryPreferred") == 0) {
            rp.mode(read_preference::read_mode::k_secondary_preferred);
        } else if (mode.compare("Nearest") == 0) {
            rp.mode(read_preference::read_mode::k_nearest);
        }
        return rp;
    }

    return {};
}

options::transaction set_opts(document::view args) {
    options::transaction txn_opts;

    if (auto rc = lookup_read_concern(args)) {
        txn_opts.read_concern(*rc);
    }

    if (auto wc = lookup_write_concern(args)) {
        txn_opts.write_concern(*wc);
    }

    if (auto rp = lookup_read_preference(args)) {
        txn_opts.read_preference(*rp);
    }

    return txn_opts;
}

document::value start_transaction(client_session& session, bsoncxx::document::view operation) {
    if (operation["arguments"]) {
        options::transaction txn_opts = set_opts(operation["arguments"].get_document());
        session.start_transaction(txn_opts);
    } else {
        session.start_transaction();
    }

    return bsoncxx::builder::basic::make_document();
}

document::value assert_session_transaction_state(client_session& session, document::view op) {
    auto state = op["arguments"]["state"].get_string().value.to_string();
    switch (session.get_transaction_state()) {
        case client_session::transaction_state::k_mongoc_transaction_none:
            REQUIRE(state == "none");
            break;
        case client_session::transaction_state::k_mongoc_transaction_starting:
            REQUIRE(state == "starting");
            break;
        case client_session::transaction_state::k_mongoc_transaction_in_progress:
            REQUIRE(state == "in_progress");
            break;
        case client_session::transaction_state::k_mongoc_transaction_committed:
            REQUIRE(state == "committed");
            break;
        case client_session::transaction_state::k_mongoc_transaction_aborted:
            REQUIRE(state == "aborted");
            break;
    }
    return make_document();
}

bool collection_exists(document::view op) {
    client client{uri{}};
    auto coll_name = op["arguments"]["collectionName"].get_string().value.to_string();
    auto db_name = op["arguments"]["databaseName"].get_string().value.to_string();
    return client.database(db_name).has_collection(coll_name);
}

document::value create_index(collection& coll,
                             client_session* session,
                             const document::view& operation) {
    auto arguments = operation["arguments"];

    auto name = arguments["name"].get_string().value;
    auto keys = arguments["keys"].get_document().value;

    bsoncxx::builder::basic::document opts;
    opts.append(bsoncxx::builder::basic::kvp("name", name));

    if (session)
        return make_document(kvp("result", coll.create_index(*session, keys, opts.extract())));
    return make_document(kvp("result", coll.create_index(keys, opts.extract())));
}

bool index_exists(document::view op) {
    client client{uri{}};
    auto coll_name = op["arguments"]["collectionName"].get_string().value.to_string();
    auto db_name = op["arguments"]["databaseName"].get_string().value.to_string();
    auto coll = client.database(db_name).collection(coll_name);

    auto indexes = coll.indexes().list();
    auto expected_name = op["arguments"]["indexName"].get_string().value.to_string();

    return std::any_of(indexes.begin(), indexes.end(), [&](document::view index) {
        return expected_name == index["name"].get_string().value.to_string();
    });
}

struct with_transaction_cb {
    array::value operations;
    entity::map& map;
    spec::apm_checker& apm;

    void operator()(client_session*) {
        for (auto&& op : operations.view()) {
            operations::run(map, apm, op);
        }
    }
};

document::value download(gridfs::bucket& bucket, document::view op) {
    using bsoncxx::types::bson_value::value;

    auto id = op["arguments"]["id"].get_value();

    std::ostringstream result{};
    bucket.download_to_stream(id, &result);

    auto result_str = result.str();
    std::vector<std::uint8_t> bytes{result_str.begin(), result_str.end()};

    return make_document(kvp("result", value(bytes)));
}

document::value upload(gridfs::bucket& bucket, document::view op) {
    auto key = op["saveResultAsEntity"].get_string().value.to_string();
    auto arguments = op["arguments"].get_document().value;

    options::gridfs::upload upload_options;

    if (arguments["chunkSizeBytes"]) {
        upload_options.chunk_size_bytes(arguments["chunkSizeBytes"].get_int32().value);
    }

    if (arguments["metadata"]) {
        upload_options.metadata(arguments["metadata"].get_document().value);
    }

    auto filename = arguments["filename"].get_string().value;
    auto uploader = bucket.open_upload_stream(filename, upload_options);
    auto source = arguments["source"].get_document().value;
    auto hex = source["$$hexBytes"].get_string().value;
    auto source_bytes = test_util::convert_hex_string_to_bytes(hex);

    uploader.write(source_bytes.data(), source_bytes.size());
    auto upload_result = uploader.close();
    auto id = upload_result.id();

    return make_document(kvp("result", id));
}

document::value delete_one(collection& coll, document::view operation) {
    document::view arguments = operation["arguments"].get_document().value;
    document::view filter = arguments["filter"].get_document().value;
    options::delete_options options{};

    if (arguments["collation"]) {
        options.collation(arguments["collation"].get_document().value);
    }

    if (arguments["hint"]) {
        if (arguments["hint"].type() == bsoncxx::v_noabi::type::k_string)
            options.hint(hint{arguments["hint"].get_string().value});
        else
            options.hint(hint{arguments["hint"].get_document().value});
    }

    auto result = builder::basic::document{};
    std::int32_t deleted_count = 0;

    if (auto delete_result = coll.delete_one(filter, options)) {
        deleted_count = delete_result->deleted_count();
    }

    result.append(
        builder::basic::kvp("result", [deleted_count](builder::basic::sub_document subdoc) {
            subdoc.append(builder::basic::kvp("deletedCount", deleted_count));
        }));

    return result.extract();
}

document::value with_transaction(client_session& session,
                                 document::view op,
                                 with_transaction_cb cb) {
    if (op["arguments"]) {
        auto opts = set_opts(op["arguments"].get_document());
        session.with_transaction(cb, opts);
    } else {
        session.with_transaction(cb);
    }
    return make_document();
}

document::value end_session(entity::map& map, const std::string& name) {
    auto& session = map.get_client_session(name);
    auto id = session.id();

    map.erase(name);
    map.insert(name, id);
    return make_document();
}

document::value operations::run(entity::map& map,
                                spec::apm_checker& apm,
                                const array::element& op) {
    auto name = op["name"].get_string().value.to_string();
    auto object = op["object"].get_string().value.to_string();

    auto empty_doc = make_document();
    auto op_view = op.get_document().view();
    CAPTURE(name, object, to_json(op_view));
    if (name == "find")
        return find_one(map.get_collection(object), op_view);
    if (name == "bulkWrite")
        return bulk_write(map.get_collection(object), op_view);
    if (name == "insertMany")
        return insert_many(map.get_collection(object), op_view);
    if (name == "replaceOne")
        return replace_one(map.get_collection(object), op_view);
    if (name == "aggregate") {
        const auto& type = map.type(object);
        if (type == typeid(mongocxx::database))
            return aggregate(map.get_database(object), op_view);
        if (type == typeid(mongocxx::collection))
            return aggregate(map.get_collection(object), op_view);

        CAPTURE(object, type.name());
        throw std::logic_error{"unrecognized object"};
    }
    if (name == "createChangeStream")
        return create_change_stream(map, object, op_view);
    if (name == "insertOne") {
        if (op["arguments"]["session"]) {
            auto session_name = op["arguments"]["session"].get_string().value.to_string();
            auto& session = map.get_client_session(session_name);
            return insert_one(map.get_collection(object), &session, op_view);
        }
        return insert_one(map.get_collection(object), nullptr, op_view);
    }
    if (name == "iterateUntilDocumentOrError")
        return iterate_until_document_or_error(map.get_change_stream(object));
    if (name == "failPoint")
        return fail_point(map, apm, op_view);
    if (name == "findOneAndUpdate")
        return find_one_and_update(map.get_collection(object), op_view);
    if (name == "listDatabases") {
        map.get_client(object).list_databases().begin();
        return empty_doc;
    }
    if (name == "assertSessionNotDirty") {
        auto session_name = op["arguments"]["session"].get_string().value.to_string();
        auto& session = map.get_client_session(session_name);
        REQUIRE(!session.get_dirty());
        return empty_doc;
    }
    if (name == "assertSessionDirty") {
        auto session_name = op["arguments"]["session"].get_string().value.to_string();
        auto& session = map.get_client_session(session_name);
        REQUIRE(session.get_dirty());
        return empty_doc;
    }
    if (name == "endSession") {
        return end_session(map, object);
    }
    if (name == "assertSameLsidOnLastTwoCommands") {
        auto cse1 = *(apm.end() - 1);
        auto cse2 = *(apm.end() - 2);

        CAPTURE(to_json(cse1), to_json(cse2));
        REQUIRE(cse1["commandStartedEvent"]["command"]["lsid"].get_value() ==
                cse2["commandStartedEvent"]["command"]["lsid"].get_value());
        return empty_doc;
    }
    if (name == "assertDifferentLsidOnLastTwoCommands") {
        auto cse1 = *(apm.end() - 1);
        auto cse2 = *(apm.end() - 2);

        CAPTURE(to_json(cse1), to_json(cse2));
        REQUIRE(cse1["commandStartedEvent"]["command"]["lsid"].get_value() !=
                cse2["commandStartedEvent"]["command"]["lsid"].get_value());
        return empty_doc;
    }
    if (name == "startTransaction") {
        auto& session = map.get_client_session(object);
        return start_transaction(session, op_view);
    }
    if (name == "commitTransaction") {
        auto& session = map.get_client_session(object);
        session.commit_transaction();
        return empty_doc;
    }
    if (name == "assertSessionTransactionState") {
        auto session_name = op["arguments"]["session"].get_string().value.to_string();
        auto& session = map.get_client_session(session_name);
        return assert_session_transaction_state(session, op_view);
    }
    if (name == "dropCollection") {
        auto coll_name = op["arguments"]["collection"].get_string().value.to_string();
        auto& db = map.get_database(object);
        db.collection(coll_name).drop();
        return empty_doc;
    }
    if (name == "createCollection") {
        auto coll_name = op["arguments"]["collection"].get_string().value.to_string();
        auto& db = map.get_database(object);
        if (op["arguments"]["session"]) {
            auto session_name = op["arguments"]["session"].get_string().value.to_string();
            auto& session = map.get_client_session(session_name);
            db.create_collection(session, coll_name);
        } else {
            db.create_collection(coll_name);
        }
        return empty_doc;
    }
    if (name == "assertCollectionNotExists") {
        REQUIRE_FALSE(collection_exists(op_view));
        return empty_doc;
    }
    if (name == "assertCollectionExists") {
        REQUIRE(collection_exists(op_view));
        return empty_doc;
    }
    if (name == "createIndex") {
        auto& coll = map.get_collection(object);
        if (op["arguments"]["session"]) {
            auto session_name = op["arguments"]["session"].get_string().value.to_string();
            auto& session = map.get_client_session(session_name);
            return create_index(coll, &session, op_view);
        }
        return create_index(coll, nullptr, op_view);
    }
    if (name == "assertIndexNotExists") {
        REQUIRE_FALSE(index_exists(op_view));
        return empty_doc;
    }
    if (name == "assertIndexExists") {
        REQUIRE(index_exists(op_view));
        return empty_doc;
    }
    if (name == "withTransaction") {
        auto cb = with_transaction_cb{
            array::value(op["arguments"]["callback"].get_array().value), map, apm};
        auto& session = map.get_client_session(object);
        return with_transaction(session, op_view, cb);
    }
    if (name == "delete") {
        auto& bucket = map.get_bucket(object);
        auto arguments = op["arguments"];
        bucket.delete_file(arguments.get_document().view()["id"].get_value());
        return empty_doc;
    }
    if (name == "download") {
        auto& bucket = map.get_bucket(object);
        return download(bucket, op_view);
    }
    if (name == "upload") {
        auto& bucket = map.get_bucket(object);
        return upload(bucket, op_view);
    }
    if (name == "deleteOne") {
        auto& coll = map.get_collection(object);
        return delete_one(coll, op_view);
    }

    throw std::logic_error{"unsupported operation: " + name};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
