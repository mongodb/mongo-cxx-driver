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

#include <mongo/bson/builder.hpp>
#include <mongo/bson/libbson.hpp>

#include <mongo/driver/private/client.hpp>
#include <mongo/driver/private/collection.hpp>
#include <mongo/driver/private/database.hpp>
#include <mongo/driver/private/pipeline.hpp>
#include <mongo/driver/private/bulk_write.hpp>
#include <mongo/driver/private/read_preference.hpp>
#include <mongo/driver/private/write_concern.hpp>
#include <mongo/driver/write_concern.hpp>
#include <mongo/driver/collection.hpp>
#include <mongo/driver/client.hpp>
#include <mongo/driver/model/write.hpp>
#include <mongo/driver/private/libmongoc.hpp>
#include <mongo/driver/result/bulk_write.hpp>
#include <mongo/driver/result/delete.hpp>
#include <mongo/driver/result/insert_many.hpp>
#include <mongo/driver/result/insert_one.hpp>
#include <mongo/driver/result/replace_one.hpp>
#include <mongo/driver/result/update.hpp>
#include <mongo/bson/stdx/optional.hpp>

#include <mongo/bson/stdx/make_unique.hpp>

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

namespace mongo {
namespace driver {

using namespace bson::libbson;

collection::collection(collection&&) noexcept = default;
collection& collection::operator=(collection&&) noexcept = default;
collection::~collection() = default;

const std::string& collection::name() const noexcept {
    return _impl->name;
}

collection::collection(const database& database, const std::string& collection_name)
    : _impl(stdx::make_unique<impl>(
          libmongoc::database_get_collection(database._impl->database_t, collection_name.c_str()),
          database.name(), database._impl->client_impl, collection_name.c_str())) {
}

stdx::optional<result::bulk_write> collection::bulk_write(const class bulk_write& bulk_write) {
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

    return stdx::optional<result::bulk_write>(std::move(result));
}

cursor collection::find(bson::document::view filter, const options::find& options) {
    using namespace bson;

    builder::document filter_builder;

    scoped_bson_t filter_bson;
    scoped_bson_t projection(options.projection());

    if (options.modifiers()) {
        filter_builder << "$query" << types::b_document{filter}
                       << builder::helpers::concat{options.modifiers().value_or(document::view{})};

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

stdx::optional<bson::document::value> collection::find_one(bson::document::view filter,
                                                     const options::find& options) {
    options::find copy(options);
    copy.limit(1);
    cursor cursor = find(filter, copy);
    cursor::iterator it = cursor.begin();
    if (it == cursor.end()) {
        return stdx::nullopt;
    }
    return stdx::optional<bson::document::value>(*it);
}

cursor collection::aggregate(const pipeline& pipeline, const options::aggregate& options) {
    using namespace bson::builder::helpers;

    scoped_bson_t stages(pipeline._impl->view());

    bson::builder::document b;

    if (options.allow_disk_use()) {
        /* TODO */
    }
    if (options.use_cursor()) {
        auto inner = b << "cursor" << open_doc;

        if (options.batch_size()) {
            inner << "batchSize" << *options.batch_size();
        }

        inner << close_doc;
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

stdx::optional<result::insert_one> collection::insert_one(bson::document::view document,
                                                    const options::insert& options) {
    class bulk_write bulk_op(false);
    bson::document::element oid{};

    if (!document.has_key("_id")) {
        bson::builder::document new_document;
        new_document << "_id" << bson::oid(bson::oid::init_tag);
        new_document << bson::builder::helpers::concat{document};
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
        return stdx::optional<result::insert_one>();
    }
    return stdx::optional<result::insert_one>(result::insert_one(std::move(result.value()), oid));
}

stdx::optional<result::replace_one> collection::replace_one(bson::document::view filter,
                                                      bson::document::view replacement,
                                                      const options::update& options) {
    class bulk_write bulk_op(false);
    model::replace_one replace_op(filter, replacement);
    bulk_op.append(replace_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::optional<result::replace_one>();
    }

    return stdx::optional<result::replace_one>(result::replace_one(std::move(result.value())));
};

stdx::optional<result::update> collection::update_many(bson::document::view filter,
                                                 bson::document::view update,
                                                 const options::update& options) {
    class bulk_write bulk_op(false);
    model::update_many update_op(filter, update);

    if (options.upsert()) update_op.upsert(options.upsert().value());

    bulk_op.append(update_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::optional<result::update>();
    }

    return stdx::optional<result::update>(result::update(std::move(result.value())));
}

stdx::optional<result::delete_result> collection::delete_many(bson::document::view filter,
                                                        const options::delete_options& options) {
    class bulk_write bulk_op(false);
    model::delete_many delete_op(filter);
    bulk_op.append(delete_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::optional<result::delete_result>();
    }

    return stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

stdx::optional<result::update> collection::update_one(bson::document::view filter,
                                                bson::document::view update,
                                                const options::update& options) {
    class bulk_write bulk_op(false);
    model::update_many update_op(filter, update);

    if (options.upsert()) update_op.upsert(options.upsert().value());

    bulk_op.append(update_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::optional<result::update>();
    }

    return stdx::optional<result::update>(result::update(std::move(result.value())));
}

stdx::optional<result::delete_result> collection::delete_one(bson::document::view filter,
                                                       const options::delete_options& options) {
    class bulk_write bulk_op(false);
    model::delete_one delete_op(filter);
    bulk_op.append(delete_op);

    if (options.write_concern()) bulk_op.write_concern(*options.write_concern());

    auto result = bulk_write(bulk_op);
    if (!result) {
        return stdx::optional<result::delete_result>();
    }
    return stdx::optional<result::delete_result>(result::delete_result(std::move(result.value())));
}

stdx::optional<bson::document::value> collection::find_one_and_replace(
    bson::document::view filter, bson::document::view replacement,
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

    bson::document::view result = reply.view();

    if (result["value"].type() == bson::type::k_null) return stdx::optional<bson::document::value>{};

    using namespace bson::builder::helpers;
    bson::builder::document b;
    b << concat{result["value"].get_document()};
    return b.extract();
}

stdx::optional<bson::document::value> collection::find_one_and_update(
    bson::document::view filter, bson::document::view update,
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

    bson::document::view result = reply.view();

    if (result["value"].type() == bson::type::k_null) return stdx::optional<bson::document::value>{};

    using namespace bson::builder::helpers;
    bson::builder::document b;
    b << concat{result["value"].get_document()};
    return b.extract();
}

stdx::optional<bson::document::value> collection::find_one_and_delete(
    bson::document::view filter, const options::find_one_and_delete& options) {
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

    bson::document::view result = reply.view();

    if (result["value"].type() == bson::type::k_null) return stdx::optional<bson::document::value>{};

    using namespace bson::builder::helpers;
    bson::builder::document b;
    b << concat{result["value"].get_document()};
    return b.extract();
}

std::int64_t collection::count(bson::document::view filter, const options::count& options) {
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
    class read_preference rp(stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::collection_get_read_prefs(_impl->collection_t))));
    return rp;
}

void collection::write_concern(class write_concern wc) {
    libmongoc::collection_set_write_concern(_impl->collection_t, wc._impl->write_concern_t);
}

class write_concern collection::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(libmongoc::write_concern_copy(
        libmongoc::collection_get_write_concern(_impl->collection_t))));
    return wc;
}

}  // namespace driver
}  // namespace mongo
