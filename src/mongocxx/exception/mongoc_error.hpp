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

#pragma once

#include <cstdint>
#include <limits>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

enum class mongoc_error : std::int32_t {
    k_bson_invalid = 1,
    k_client_authenticate,
    k_client_getnonce,
    k_client_in_exhaust,
    k_client_no_acceptable_peer,
    k_client_not_ready,
    k_client_too_big,
    k_client_too_small,
    k_collection_delete_failed,
    k_collection_does_not_exist,
    k_collection_insert_failed,
    k_collection_update_failed,
    k_command_invalid_arg,
    k_cursor_invalid_cursor,
    k_gridfs_invalid_filename,
    k_matcher_invalid,
    k_namespace_invalid,
    k_namespace_invalid_filter_type,
    k_protocol_bad_wire_version,
    k_protocol_error,
    k_protocol_invalid_reply,
    k_query_command_not_found,
    k_query_failure,
    k_query_not_tailable,
    k_scram_not_done,
    k_scram_protocol_error,
    k_server_selection_bad_wire_version,
    k_server_selection_failure,
    k_server_selection_invalid_id,
    k_stream_connect,
    k_stream_invalid_state,
    k_stream_invalid_type,
    k_stream_name_resolution,
    k_stream_not_established,
    k_stream_socket,
    k_raw_bson,
    k_unknown = std::numeric_limits<std::int32_t>::max(),
};

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
