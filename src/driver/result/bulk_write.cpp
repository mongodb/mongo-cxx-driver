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

#include "driver/result/bulk_write.hpp"

namespace mongo {
namespace driver {
namespace result {

bulk_write::bulk_write(bson::document::value raw_response)
    : _response(std::move(raw_response))
{}

std::int64_t bulk_write::inserted_count() const { return view()["nInserted"].get_int64(); }

std::int64_t bulk_write::matched_count() const { return view()["nMatched"].get_int64(); }

std::int64_t bulk_write::modified_count() const { return view()["nModified"].get_int64(); };

std::int64_t bulk_write::deleted_count() const { return view()["nRemoved"].get_int64(); }

std::int64_t bulk_write::upserted_count() const { return view()["nUpserted"].get_int64(); }

bson::document::element bulk_write::inserted_ids() const {
    return view()["inserted_ids"];
}

bson::document::element bulk_write::upserted_ids() const {
    return view()["upserted_ids"];
}

bson::document::view bulk_write::view() const {
    return _response.view();
}

}  // namespace result
}  // namespace driver
}  // namespace mongo
