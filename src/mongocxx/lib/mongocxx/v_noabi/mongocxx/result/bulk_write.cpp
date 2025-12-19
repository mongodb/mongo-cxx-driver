// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/result/bulk_write.hpp>

//

#include <mongocxx/v1/bulk_write.hh>

#include <cstdint>
#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

bulk_write::bulk_write(bsoncxx::v_noabi::document::value raw_response)
    : _result{v1::bulk_write::result::internal::make(bsoncxx::v_noabi::to_v1(std::move(raw_response)))} {}

std::int32_t bulk_write::inserted_count() const {
    return v1::bulk_write::result::internal::reply(_result)["nInserted"].get_int32();
}

std::int32_t bulk_write::matched_count() const {
    return v1::bulk_write::result::internal::reply(_result)["nMatched"].get_int32();
}

std::int32_t bulk_write::modified_count() const {
    return v1::bulk_write::result::internal::reply(_result)["nModified"].get_int32();
}

std::int32_t bulk_write::deleted_count() const {
    return v1::bulk_write::result::internal::reply(_result)["nRemoved"].get_int32();
}

std::int32_t bulk_write::upserted_count() const {
    return v1::bulk_write::result::internal::reply(_result)["nUpserted"].get_int32();
}

bulk_write::id_map bulk_write::upserted_ids() const {
    auto const reply = v1::bulk_write::result::internal::reply(_result);

    id_map upserted_ids;

    if (!reply["upserted"]) {
        return upserted_ids;
    }

    for (auto&& id : reply["upserted"].get_array().value) {
        upserted_ids.emplace(id["index"].get_int32(), id["_id"]);
    }
    return upserted_ids;
}

bool operator==(bulk_write const& lhs, bulk_write const& rhs) {
    auto const& l = v1::bulk_write::result::internal::reply(lhs._result);
    auto const& r = v1::bulk_write::result::internal::reply(rhs._result);

    return l == r;
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
