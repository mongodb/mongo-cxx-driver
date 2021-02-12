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

#include <mongocxx/config/private/prelude.hh>

#include <map>
#include <string>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/client_session.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libmongoc.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace bsoncxx;
using namespace mongocxx;

pipeline build_pipeline(array::view pipeline_docs);
bsoncxx::stdx::optional<read_concern> lookup_read_concern(document::view doc);
bsoncxx::stdx::optional<write_concern> lookup_write_concern(document::view doc);
bsoncxx::stdx::optional<read_preference> lookup_read_preference(document::view doc);

class operation_runner {
   public:
    explicit operation_runner() noexcept = default;

    document::value run(document::view operation);

    operation_runner& set_collection(std::shared_ptr<collection> coll);

    operation_runner& set_database(std::shared_ptr<database> db);

    operation_runner& set_session0(std::shared_ptr<client_session> session0);

    operation_runner& set_session1(std::shared_ptr<client_session> session1);

    operation_runner& set_client(std::shared_ptr<client> client);

   private:
    std::shared_ptr<collection> _coll;
    std::shared_ptr<database> _db;
    std::shared_ptr<client_session> _session0;
    std::shared_ptr<client_session> _session1;
    std::shared_ptr<client> _client;

    std::shared_ptr<client_session> _lookup_session(document::view doc);
    std::shared_ptr<client_session> _lookup_session(stdx::string_view key);
    document::value _run_aggregate(document::view operation);
    document::value _run_count(document::view operation);
    document::value _run_distinct(document::view operation);
    document::value _run_find(document::view operation);
    document::value _run_delete_many(document::view operation);
    document::value _run_delete_one(document::view operation);
    document::value _run_find_one_and_delete(document::view operation);
    document::value _run_find_one_and_replace(document::view operation);
    document::value _run_find_one_and_update(document::view operation);
    document::value _run_insert_many(document::view operation);
    document::value _run_insert_one(document::view operation);
    document::value _run_replace_one(document::view operation);
    document::value _run_update_many(document::view operation);
    document::value _run_update_one(document::view operation);
    document::value _run_bulk_write(document::view operation);
    document::value _run_count_documents(document::view operation);
    document::value _run_estimated_document_count(document::view);
    document::value _run_start_transaction(document::view operation);
    document::value _run_commit_transaction(document::view operation);
    document::value _run_abort_transaction(document::view operation);
    document::value _run_run_command(document::view operation);
    document::value _run_configure_fail_point(bsoncxx::document::view operation);
    document::value _run_find_one(document::view operation);
    document::value _create_index(const document::view& operation);
    void _set_collection_options(document::view operation);
};

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
#include <mongocxx/config/private/postlude.hh>
