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

#pragma once

#include "driver/config/prelude.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

#include "bson/builder.hpp"
#include "bson/document.hpp"
#include "driver/base/bulk_write.hpp"
#include "driver/base/cursor.hpp"
#include "driver/base/read_preference.hpp"
#include "driver/base/write_concern.hpp"
#include "driver/options/aggregate.hpp"
#include "driver/options/bulk_write.hpp"
#include "driver/options/count.hpp"
#include "driver/options/delete.hpp"
#include "driver/options/distinct.hpp"
#include "driver/options/find.hpp"
#include "driver/options/find_one_and_delete.hpp"
#include "driver/options/find_one_and_replace.hpp"
#include "driver/options/find_one_and_update.hpp"
#include "driver/options/insert.hpp"
#include "driver/options/update.hpp"
#include "driver/result/bulk_write.hpp"
#include "driver/result/delete.hpp"
#include "driver/result/insert_many.hpp"
#include "driver/result/insert_one.hpp"
#include "driver/result/replace_one.hpp"
#include "driver/result/update.hpp"

namespace mongo {
namespace driver {

class client;
class database;
class pipeline;

// TODO: make copyable when c-driver supports this

class LIBMONGOCXX_EXPORT collection {

   public:
    collection(collection&& other) noexcept;
    collection& operator=(collection&& rhs) noexcept;

    ~collection();

    cursor aggregate(
        const pipeline& pipeline,
        const options::aggregate& options = options::aggregate()
    );

    template<class Container>
    inline optional<result::bulk_write> bulk_write(
        const Container& requests,
        const options::bulk_write& options = options::bulk_write()
    );

    template<class WriteModelIterator>
    inline optional<result::bulk_write> bulk_write(
        WriteModelIterator begin,
        WriteModelIterator end,
        const options::bulk_write& options = options::bulk_write()
    );

    optional<result::bulk_write> bulk_write(
        const class bulk_write& bulk_write
    );

    std::int64_t count(
        bson::document::view filter,
        const options::count& options = options::count()
    );

    bson::document::value create_index(
        bson::document::view keys,
        bson::document::view options
    );

    optional<result::delete_result> delete_one(
        bson::document::view filter,
        const options::delete_options& options = options::delete_options()
    );

    optional<result::delete_result> delete_many(
        bson::document::view filter,
        const options::delete_options& options = options::delete_options()
    );

    cursor distinct(
        const std::string& field_name,
        bson::document::view filter,
        const options::distinct& options = options::distinct()
    );

    void drop();

    cursor find(
        bson::document::view filter,
        const options::find& options = options::find()
    );

    optional<bson::document::value> find_one(
        bson::document::view filter,
        const options::find& options = options::find()
    );

    optional<bson::document::value> find_one_and_delete(
        bson::document::view filter,
        const options::find_one_and_delete& options = options::find_one_and_delete()
    );

    optional<bson::document::value> find_one_and_update(
        bson::document::view filter,
        bson::document::view update,
        const options::find_one_and_update& options = options::find_one_and_update()
    );

    optional<bson::document::value> find_one_and_replace(
        bson::document::view filter,
        bson::document::view replacement,
        const options::find_one_and_replace& options = options::find_one_and_replace()
    );

    optional<result::insert_one> insert_one(
        bson::document::view document,
        const options::insert& options = options::insert()
    );

    template<class Container>
    inline optional<result::insert_many> insert_many(
        const Container& container,
        const options::insert& options = options::insert()
    );

    // TODO: document DocumentViewIterator concept or static assert
    template<class DocumentViewIterator>
    inline optional<result::insert_many> insert_many(
        DocumentViewIterator begin,
        DocumentViewIterator end,
        const options::insert& options = options::insert()
    );

    cursor list_indexes() const;

    const std::string& name() const;

    void read_preference(class read_preference rp);
    class read_preference read_preference() const;

    optional<result::replace_one> replace_one(
        bson::document::view filter,
        bson::document::view replacement,
        const options::update& options = options::update()
    );

    optional<result::update> update_one(
        bson::document::view filter,
        bson::document::view update,
        const options::update& options = options::update()
    );

    optional<result::update> update_many(
        bson::document::view filter,
        bson::document::view update,
        const options::update& options = options::update()
    );

    void write_concern(class write_concern wc);
    class write_concern write_concern() const;

   private:
    friend class database;

    collection(const database& database, const std::string& collection_name);

    class impl;
    std::unique_ptr<impl> _impl;

}; // class collection

template<class Container>
inline optional<result::bulk_write> collection::bulk_write(
    const Container& requests,
    const options::bulk_write& options
) {
    return bulk_write(requests.begin(), requests.end(), options);
}

template<class WriteModelIterator>
inline optional<result::bulk_write> collection::bulk_write(
    WriteModelIterator begin,
    WriteModelIterator end,
    const options::bulk_write& options
) {
    class bulk_write writes(options.ordered().value_or(true));

    std::for_each(begin, end, [&](const model::write& current){
        writes.append(current);
    });

    return bulk_write(writes);
}

template<class Container>
inline optional<result::insert_many> collection::insert_many(
    const Container& container,
    const options::insert& options
) {
    return insert_many(container.begin(), container.end(), options);
}

template<class DocumentViewIterator>
inline optional<result::insert_many> collection::insert_many(
    DocumentViewIterator begin,
    DocumentViewIterator end,
    const options::insert& options
) {
    class bulk_write writes(false);

    std::map<std::size_t, bson::document::element> inserted_ids{};
    size_t index = 0;
    std::for_each(begin, end, [&](const bson::document::view& current){
        // TODO: put this somewhere else not in header scope (bson::builder)
        if ( !current.has_key("_id")) {
            bson::builder::document new_document;
            new_document << "_id" << bson::oid(bson::oid::init_tag);
            new_document << bson::builder::helpers::concat{current};

            writes.append(model::insert_one(new_document.view()));

            inserted_ids.emplace(index++, new_document.view()["_id"]);
        } else {
            writes.append(model::insert_one(current));

            inserted_ids.emplace(index++, current["_id"]);
        }

    });

    if (options.write_concern())
        writes.write_concern(*options.write_concern());
    result::bulk_write res(std::move(bulk_write(writes).value()));
    optional<result::insert_many> result(result::insert_many(std::move(res), std::move(inserted_ids)));
    return result;
}

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
