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

#include <system_error>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

enum class error_code : std::int32_t {
    k_invalid_client_object = 1,
    k_invalid_collection_object,
    k_invalid_database_object,
    k_invalid_parameter,
    k_ssl_not_supported,
    k_unknown_read_concern,
    k_unknown_write_concern,
};

MONGOCXX_API const std::error_code& bson_invalid_error();

MONGOCXX_API const std::error_code& client_authenticate_error();

MONGOCXX_API const std::error_code& client_getnonce_error();

MONGOCXX_API const std::error_code& client_in_exhaust_error();

MONGOCXX_API const std::error_code& client_no_acceptable_peer_error();

MONGOCXX_API const std::error_code& client_not_ready_error();

MONGOCXX_API const std::error_code& client_too_big_error();

MONGOCXX_API const std::error_code& client_too_small_error();

MONGOCXX_API const std::error_code& collection_delete_failed_error();

MONGOCXX_API const std::error_code& collection_does_not_exist_error();

MONGOCXX_API const std::error_code& collection_delete_insert_error();

MONGOCXX_API const std::error_code& collection_update_failed_error();

MONGOCXX_API const std::error_code& command_invalid_arg_error();

MONGOCXX_API const std::error_code& invalid_cursor_error();

MONGOCXX_API const std::error_code& gridfs_invalid_filename_error();

MONGOCXX_API const std::error_code& matcher_invalid_error();

MONGOCXX_API const std::error_code& namespace_invalid_error();

MONGOCXX_API const std::error_code& namespace_invalid_filter_type_error();

MONGOCXX_API const std::error_code& protocol_bad_wire_version_error();

MONGOCXX_API const std::error_code& protocol_error();

MONGOCXX_API const std::error_code& protocol_invalid_reply_error();

MONGOCXX_API const std::error_code& query_command_not_found_error();

MONGOCXX_API const std::error_code& query_failure_error();

MONGOCXX_API const std::error_code& query_not_tailable_error();

MONGOCXX_API const std::error_code& collection_does_not_exist_query_error();

MONGOCXX_API const std::error_code& scram_not_done_error();

MONGOCXX_API const std::error_code& scram_protocol_error();

MONGOCXX_API const std::error_code& server_selection_bad_wire_version_error();

MONGOCXX_API const std::error_code& server_selection_failure_error();

MONGOCXX_API const std::error_code& server_selection_invalid_id_error();

MONGOCXX_API const std::error_code& stream_connect_error();

MONGOCXX_API const std::error_code& stream_invalid_state_error();

MONGOCXX_API const std::error_code& stream_invalid_type_error();

MONGOCXX_API const std::error_code& stream_name_resolution_error();

MONGOCXX_API const std::error_code& stream_not_established_error();

MONGOCXX_API const std::error_code& stream_socket_error();

MONGOCXX_API const std::error_code& invalid_client_object_error();

MONGOCXX_API const std::error_code& invalid_collection_object_error();

MONGOCXX_API const std::error_code& invalid_database_object_error();

MONGOCXX_API const std::error_code& invalid_parameter_error();

MONGOCXX_API const std::error_code& ssl_not_supported_error();

MONGOCXX_API const std::error_code& unknown_read_concern_error();

MONGOCXX_API const std::error_code& unknown_write_concern_error();

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
