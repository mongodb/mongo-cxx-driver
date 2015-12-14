// Copyright 2015 MongoDB Inc.
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

#include <mongocxx/config/prelude.hpp>

#include <mongocxx/insert_many_builder.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/collection.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

insert_many_builder::insert_many_builder(const options::insert& options)
    : _writes{options.ordered().value_or(true)}, _inserted_ids{}, _index{0} {
    if (options.write_concern()) {
        _writes.write_concern(*options.write_concern());
    }
    if (options.bypass_document_validation()) {
        _writes.bypass_document_validation(*options.bypass_document_validation());
    }
};

void insert_many_builder::operator()(const bsoncxx::document::view& doc) {
    if (!doc["_id"]) {
        bsoncxx::builder::stream::document new_document;
        new_document << "_id" << bsoncxx::oid(bsoncxx::oid::init_tag)
                     << bsoncxx::builder::stream::concatenate(doc);

        _writes.append(model::insert_one{new_document.view()});
        _inserted_ids.emplace(_index++, new_document.view()["_id"]);
    } else {
        _writes.append(model::insert_one{doc});
        _inserted_ids.emplace(_index++, doc["_id"]);
    }
};

stdx::optional<result::insert_many> insert_many_builder::insert(collection* col) const {
    auto val = col->bulk_write(_writes).value();
    result::bulk_write res{std::move(val)};
    stdx::optional<result::insert_many> result{{std::move(res), std::move(_inserted_ids)}};
    return result;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
