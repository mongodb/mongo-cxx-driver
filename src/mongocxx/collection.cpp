// Copyright 2014 MongoDB Inc.
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

#include <cstdint>

#include <bsoncxx/builder/stream/document.hpp>

#include <mongocxx/private/client.hpp>
#include <mongocxx/private/collection.hpp>
#include <mongocxx/private/database.hpp>
#include <mongocxx/private/pipeline.hpp>
#include <mongocxx/private/bulk_write.hpp>
#include <mongocxx/private/read_preference.hpp>
#include <mongocxx/private/write_concern.hpp>
#include <mongocxx/write_concern.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/model/write.hpp>
#include <mongocxx/private/libbson.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/result/bulk_write.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/insert_many.hpp>
#include <mongocxx/result/insert_one.hpp>
#include <mongocxx/result/replace_one.hpp>
#include <mongocxx/result/update.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/stdx/make_unique.hpp>

namespace {
enum class cursor_flag : uint32_t {
    k_tailable,
    k_oplog_replay,
    k_no_cursor_timeout,
    k_await_data,
    k_exhaust,
    k_partial
};
}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using namespace libbson;

collection::collection(collection&&) noexcept = default;
collection& collection::operator=(collection&&) noexcept = default;
collection::~collection() = default;

const std::string& collection::name() const noexcept {
    return _impl->name;
}

collection::collection(const database& database, const std::string& collection_name)
    : _impl(bsoncxx::stdx::make_unique<impl>(
          libmongoc::database_get_collection(database._impl->database_t, collection_name.c_str()),
          database.name(), database._impl->client_impl, collection_name.c_str())) {
}

bsoncxx::stdx::optional<result::bulk_write> collection::bulk_write(const class bulk_write& bulk_write) {
    mongoc_bulk_operation_t* b = bulk_write._impl->operation_t;
    libmongoc::bulk_operation_set_database(b, _impl->database_name.c_str());
    libmongoc::bulk_operation_set_collection(b, _impl->name.c_str());
    libmongoc::bulk_operation_set_client(b, _impl->client_impl->client_t);

    scoped_bson_t reply;
    reply.flag_init();

    bson_error_t error;

    if (!libmongoc::bulk_operation_execute(b, reply.bson(), &error)) {
        throw std::runtime_error(error.message);
    }

    result::bulk_write result(reply.steal());

    return bsoncxx::stdx::optional<result::bulk_write>(std::move(result));
}

cursor collection::find(bsoncxx::document::view filter, const options::find& options) {
    using namespace bsoncxx;
    builder::stream::document filter_builder;

    scoped_bson_t filter_bson;
    scoped_bson_t projection(options.projection());

    if (options.modifiers()) {
        filter_builder << "$query" << types::b_document{filter}
                       << builder::stream::concatenate{options.modifiers().value_or(document::view{})};

        filter_bson.init_from_static(filter_builder.view());
    } else {
        filter_bson.init_from_static(filter);
    }

    const mongoc_read_prefs_t* rp_ptr = NULL;

    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    return cursor(libmongoc::collection_find(
        _impl->collection_t, mongoc_query_flags_t(0), options.skip().value_or(0),
        options.limit().value_or(0), options.batch_size().value_or(0), filter_bson.bson(),
        projection.bson(), rp_ptr));
}

bsoncxx::stdx::optional<bsoncxx::document::value> collection::find_one(bsoncxx::document::view filter,
                                                     const options::find& options) {
    options::find copy(options);
    copy.limit(1);
    cursor cursor = find(filter, copy);
    cursor::iterator it = cursor.begin();
    if (it == cursor.end()) {
        return bsoncxx::stdx::nullopt;
    }
    return bsoncxx::stdx::optional<bsoncxx::document::value>(bsoncxx::document::value{*it});
}

cursor collection::aggregate(const pipeline& pipeline, const options::aggregate& options) {
    using namespace bsoncxx::builder::stream;

    scoped_bson_t stages(pipeline._impl->view());

    bsoncxx::builder::stream::document b;

    if (options.allow_disk_use()) {
        /* TODO */
    }
    if (options.use_cursor()) {
        auto inner = b << "cursor" << open_document;

        if (options.batch_size()) {
            inner << "batchSize" << *options.batch_size();
        }

        inner << close_document;
    }

    if (options.max_time_ms()) {
        /* TODO */
    }

    scoped_bson_t options_bson(b.view());

    const mongoc_read_prefs_t* rp_ptr = NULL;

    if (options.read_preference()) {
        rp_ptr = read_preference()._impl->read_preference_t;
    }

    return cursor(libmongoc::collection_aggregate(_impl->collection_t,
                                                  static_cast<mongoc_query_flags_t>(0),
                                                  stages.bson(), options_bson.bson(), rp_ptr));
}

bsoncxx::stdx::optional<result::insert_one> collection::insert_one(bsoncxx::document::view document,
                                                    const options::insert& options) {
    class bulk_write bulk_op(false);
    bsoncxx::document::element oid{};

    if (!document["_id"]) {
        bsoncxx::builder::stream::document new_document;
        new_document << "_id" << bsoncxx::oid(bsoncxx::oid::init_tag);
        new_document << bsoncxx::builder::stream::concatenate{document};
        bulk_op.append(model::insert_one(new_document.view()));

        oid = new_document.view()["_id"];
    } else {
        bulk_op.append(model::insert_one(document));
        oid = document["_id"];
    }

    if (options.write_concern()) {
        bulk_op.write_concern(*options.write_concern());
    }

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::insert_one>();
    }

    return bsoncxx::stdx::optional<result::insert_one>(
        result::insert_one(std::move(result.value()), std::move(oid.get_value())));
}

bsoncxx::stdx::optional<result::replace_one> collection::replace_one(bsoncxx::document::view filter,
                                                      bsoncxx::document::view replacement,
                                                      const options::update& options) {
    class bulk_write bulk_op(false);
    model::replace_one replace_op(filter, replacement);
    bulk_op.append(replace_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::replace_one>();
    }

    return bsoncxx::stdx::optional<result::replace_one>(result::replace_one(std::move(result.value())));
};

bsoncxx::stdx::optional<result::update> collection::update_many(bsoncxx::document::view filter,
                                                 bsoncxx::document::view update,
                                                 const options::update& options) {
    class bulk_write bulk_op(false);
    model::update_many update_op(filter, update);

    if (options.upsert()) update_op.upsert(options.upsert().value());

    bulk_op.append(update_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::update>();
    }

    return bsoncxx::stdx::optional<result::update>(result::update(std::move(result.value())));
}

bsoncxx::stdx::optional<result::delete_result> collection::delete_many(bsoncxx::document::view filter,
                                                        const options::delete_options& options) {
    class bulk_write bulk_op(false);
    model::delete_many delete_op(filter);
    bulk_op.append(delete_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::delete_result>();
    }

    return bsoncxx::stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

bsoncxx::stdx::optional<result::update> collection::update_one(bsoncxx::document::view filter,
                                                bsoncxx::document::view update,
                                                const options::update& options) {
    class bulk_write bulk_op(false);
    model::update_many update_op(filter, update);

    if (options.upsert()) update_op.upsert(options.upsert().value());

    bulk_op.append(update_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::update>();
    }

    return bsoncxx::stdx::optional<result::update>(result::update(std::move(result.value())));
}

bsoncxx::stdx::optional<result::delete_result> collection::delete_one(bsoncxx::document::view filter,
                                                       const options::delete_options& options) {
    class bulk_write bulk_op(false);
    model::delete_one delete_op(filter);
    bulk_op.append(delete_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return bsoncxx::stdx::optional<result::delete_result>();
    }
    return bsoncxx::stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

bsoncxx::stdx::optional<bsoncxx::document::value> collection::find_one_and_replace(
    bsoncxx::document::view filter, bsoncxx::document::view replacement,
    const options::find_one_and_replace& options) {
    scoped_bson_t bson_filter{filter};
    scoped_bson_t bson_replacement{replacement};
    scoped_bson_t bson_sort{options.sort()};
    scoped_bson_t bson_projection{options.projection()};

    scoped_bson_t reply;
    reply.flag_init();

    bson_error_t error;

    options::return_document rd =
        options.return_document().value_or(options::return_document::k_before);

    bool r = mongoc_collection_find_and_modify(
        _impl->collection_t, bson_filter.bson(), bson_sort.bson(), bson_replacement.bson(),
        bson_projection.bson(), false, options.upsert().value_or(false),
        rd == options::return_document::k_after, reply.bson(), &error);

    if (!r) {
        throw std::runtime_error("baddd");
    }

    bsoncxx::document::view result = reply.view();

    if (result["value"].type() == bsoncxx::type::k_null) return bsoncxx::stdx::optional<bsoncxx::document::value>{};

    bsoncxx::builder::stream::document b;
    b << bsoncxx::builder::stream::concatenate{result["value"].get_document()};
    return b.extract();
}

bsoncxx::stdx::optional<bsoncxx::document::value> collection::find_one_and_update(
    bsoncxx::document::view filter, bsoncxx::document::view update,
    const options::find_one_and_update& options) {
    scoped_bson_t bson_filter{filter};
    scoped_bson_t bson_update{update};
    scoped_bson_t bson_sort{options.sort()};
    scoped_bson_t bson_projection{options.projection()};

    scoped_bson_t reply;
    reply.flag_init();

    bson_error_t error;

    options::return_document rd =
        options.return_document().value_or(options::return_document::k_before);

    bool r = libmongoc::collection_find_and_modify(
        _impl->collection_t, bson_filter.bson(), bson_sort.bson(), bson_update.bson(),
        bson_projection.bson(), false, options.upsert().value_or(false),
        rd == options::return_document::k_after, reply.bson(), &error);

    if (!r) {
        throw std::runtime_error("baddd");
    }

    bsoncxx::document::view result = reply.view();

    if (result["value"].type() == bsoncxx::type::k_null) return bsoncxx::stdx::optional<bsoncxx::document::value>{};

    bsoncxx::builder::stream::document b;
    b << bsoncxx::builder::stream::concatenate{result["value"].get_document()};
    return b.extract();
}

bsoncxx::stdx::optional<bsoncxx::document::value> collection::find_one_and_delete(
    bsoncxx::document::view filter, const options::find_one_and_delete& options) {
    scoped_bson_t bson_filter{filter};
    scoped_bson_t bson_sort{options.sort()};
    scoped_bson_t bson_projection{options.projection()};

    scoped_bson_t reply;
    reply.flag_init();

    bson_error_t error;

    bool r = libmongoc::collection_find_and_modify(
        _impl->collection_t, bson_filter.bson(), bson_sort.bson(), nullptr, bson_projection.bson(),
        true, false, false, reply.bson(), &error);

    if (!r) {
        throw std::runtime_error("baddd");
    }

    bsoncxx::document::view result = reply.view();

    if (result["value"].type() == bsoncxx::type::k_null) return bsoncxx::stdx::optional<bsoncxx::document::value>{};

    bsoncxx::builder::stream::document b;
    b << bsoncxx::builder::stream::concatenate{result["value"].get_document()};
    return b.extract();
}

std::int64_t collection::count(bsoncxx::document::view filter, const options::count& options) {
    scoped_bson_t bson_filter{filter};
    bson_error_t error;

    const mongoc_read_prefs_t* rp_ptr = NULL;

    if (options.read_preference()) {
        rp_ptr = options.read_preference()->_impl->read_preference_t;
    }

    auto result = libmongoc::collection_count(
        _impl->collection_t, static_cast<mongoc_query_flags_t>(0), bson_filter.bson(),
        options.skip().value_or(0), options.limit().value_or(0), rp_ptr, &error);

    /* TODO throw an exception if error
    if (result < 0)
    */

    return result;
}

bsoncxx::document::value collection::create_index(bsoncxx::document::view keys, bsoncxx::document::view options) {
    scoped_bson_t bson_keys{keys};
    bson_error_t error;

    auto result = libmongoc::collection_create_index(_impl->collection_t, bson_keys.bson(),
                                                     nullptr, &error);

    if (!result) {
        // TODO: throw an exception here
    }

    // TODO: return the response from the server, this is not possible now due to the way
    // libmongoc works.
    return bsoncxx::document::value{bsoncxx::document::view{}};
}

void collection::drop() {
    bson_error_t error;

    if (libmongoc::collection_drop(_impl->collection_t, &error)) {
        /* TODO handle errors */
    }
}

void collection::read_preference(class read_preference rp) {
    libmongoc::collection_set_read_prefs(_impl->collection_t, rp._impl->read_preference_t);
}

class read_preference collection::read_preference() const {
    class read_preference rp(bsoncxx::stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::collection_get_read_prefs(_impl->collection_t))));
    return rp;
}

void collection::write_concern(class write_concern wc) {
    libmongoc::collection_set_write_concern(_impl->collection_t, wc._impl->write_concern_t);
}

class write_concern collection::write_concern() const {
    class write_concern wc(bsoncxx::stdx::make_unique<write_concern::impl>(libmongoc::write_concern_copy(
        libmongoc::collection_get_write_concern(_impl->collection_t))));
    return wc;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
