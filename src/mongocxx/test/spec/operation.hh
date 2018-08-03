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

#pragma once

#include <map>
#include <string>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace bsoncxx;
using namespace mongocxx;

pipeline build_pipeline(array::view pipeline_docs);
document::value run_aggregate_test(collection* coll, document::view operation);
document::value run_count_test(collection* coll, document::view operation);
document::value run_count_documents_test(collection* coll, document::view operation);
document::value run_estimated_document_count_test(collection* coll, document::view operation);
document::value run_distinct_test(collection* coll, document::view operation);
document::value run_find_test(collection* coll, document::view operation);
document::value run_delete_many_test(collection* coll, document::view operation);
document::value run_delete_one_test(collection* coll, document::view operation);
document::value run_find_one_and_delete_test(collection* coll, document::view operation);
document::value run_find_one_and_replace_test(collection* coll, document::view operation);
document::value run_find_one_and_update_test(collection* coll, document::view operation);
document::value run_insert_many_test(collection* coll, document::view operation);
document::value run_insert_one_test(collection* coll, document::view operation);
document::value run_replace_one_test(collection* coll, document::view operation);
document::value run_update_many_test(collection* coll, document::view operation);
document::value run_update_one_test(collection* coll, document::view operation);
document::value run_bulk_write_test(collection* coll, document::view operation);
using test_runners =
    std::map<std::string, std::function<document::value(collection*, document::view)>>;
test_runners get_test_runners();

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
#include <mongocxx/config/private/postlude.hh>