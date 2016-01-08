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

#include <mongocxx/exception/error_code.hpp>

#include <mongocxx/exception/private/error_code.hpp>
#include <mongocxx/exception/private/mongoc_error.hpp>
#include <mongocxx/private/libmongoc.hpp>

namespace mongocxx {

const std::error_code& bson_invalid_error() {
    static const std::error_code bson_invalid_error = make_error_code(::MONGOC_ERROR_BSON_INVALID, ::MONGOC_ERROR_BSON);
    return bson_invalid_error;
}

const std::error_code& client_authenticate_error() {
    static const std::error_code client_authenticate_error = make_error_code(::MONGOC_ERROR_CLIENT_AUTHENTICATE, ::MONGOC_ERROR_CLIENT);
    return client_authenticate_error;
}

const std::error_code& client_getnonce_error() {
    static const std::error_code client_getnonce_error = make_error_code(::MONGOC_ERROR_CLIENT_GETNONCE, ::MONGOC_ERROR_CLIENT);
    return client_getnonce_error;
}

const std::error_code& client_in_exhaust_error() {
    static const std::error_code client_in_exhaust_error = make_error_code(::MONGOC_ERROR_CLIENT_IN_EXHAUST, ::MONGOC_ERROR_CLIENT);
    return client_in_exhaust_error;
}

const std::error_code& client_no_acceptable_peer_error() {
    static const std::error_code client_no_acceptable_peer_error = make_error_code(::MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER, ::MONGOC_ERROR_CLIENT);
    return client_no_acceptable_peer_error;
}

const std::error_code& client_not_ready_error() {
    static const std::error_code client_not_ready_error = make_error_code(::MONGOC_ERROR_CLIENT_NOT_READY, ::MONGOC_ERROR_CLIENT);
    return client_not_ready_error;
}

const std::error_code& client_too_big_error() {
    static const std::error_code client_too_big_error = make_error_code(::MONGOC_ERROR_CLIENT_TOO_BIG, ::MONGOC_ERROR_CLIENT);
    return client_too_big_error;
}

const std::error_code& client_too_small_error() {
    static const std::error_code client_too_small_error = make_error_code(::MONGOC_ERROR_CLIENT_TOO_SMALL, ::MONGOC_ERROR_CLIENT);
    return client_too_small_error;
}

const std::error_code& collection_delete_failed_error() {
    static const std::error_code collection_delete_failed_error = make_error_code(::MONGOC_ERROR_COLLECTION_DELETE_FAILED, ::MONGOC_ERROR_COLLECTION);
    return collection_delete_failed_error;
}

const std::error_code& collection_does_not_exist_error() {
    static const std::error_code collection_does_not_exist_error = make_error_code(::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST, ::MONGOC_ERROR_COLLECTION);
    return collection_does_not_exist_error;
}

const std::error_code& collection_delete_insert_error() {
    static const std::error_code collection_insert_failed_error = make_error_code(::MONGOC_ERROR_COLLECTION_INSERT_FAILED, ::MONGOC_ERROR_COLLECTION);
    return collection_insert_failed_error;
}

const std::error_code& collection_update_failed_error() {
    static const std::error_code collection_update_failed_error = make_error_code(::MONGOC_ERROR_COLLECTION_UPDATE_FAILED, ::MONGOC_ERROR_COLLECTION);
    return collection_update_failed_error;
}

const std::error_code& command_invalid_arg_error() {
    static const std::error_code command_invalid_arg_error = make_error_code(::MONGOC_ERROR_COMMAND_INVALID_ARG, ::MONGOC_ERROR_COMMAND);
    return command_invalid_arg_error;
}

const std::error_code& invalid_cursor_error() {
    static const std::error_code invalid_cursor_error = make_error_code(::MONGOC_ERROR_CURSOR_INVALID_CURSOR, ::MONGOC_ERROR_CURSOR);
    return invalid_cursor_error;
}

const std::error_code& gridfs_invalid_filename_error() {
    static const std::error_code gridfs_invalid_filename_error = make_error_code(::MONGOC_ERROR_GRIDFS_INVALID_FILENAME, ::MONGOC_ERROR_GRIDFS);
    return gridfs_invalid_filename_error;
}

const std::error_code& matcher_invalid_error() {
    static const std::error_code matcher_invalid_error = make_error_code(::MONGOC_ERROR_MATCHER_INVALID, ::MONGOC_ERROR_MATCHER);
    return matcher_invalid_error;
}

const std::error_code& namespace_invalid_error() {
    static const std::error_code namespace_invalid_error = make_error_code(::MONGOC_ERROR_NAMESPACE_INVALID, ::MONGOC_ERROR_NAMESPACE);
    return namespace_invalid_error;
}

const std::error_code& namespace_invalid_filter_type_error() {
    static const std::error_code namespace_invalid_filter_type_error = make_error_code(::MONGOC_ERROR_NAMESPACE_INVALID_FILTER_TYPE, ::MONGOC_ERROR_NAMESPACE);
    return namespace_invalid_filter_type_error;
}

const std::error_code& protocol_bad_wire_version_error() {
    static const std::error_code protocol_bad_wire_version_error = make_error_code(::MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION, ::MONGOC_ERROR_PROTOCOL);
    return protocol_bad_wire_version_error;
}

const std::error_code& protocol_error() {
    static const std::error_code protocol_error = make_error_code(::MONGOC_ERROR_PROTOCOL_ERROR, ::MONGOC_ERROR_PROTOCOL);
    return protocol_error;
}

const std::error_code& protocol_invalid_reply_error() {
    static const std::error_code protocol_invalid_reply_error = make_error_code(::MONGOC_ERROR_PROTOCOL_INVALID_REPLY, ::MONGOC_ERROR_PROTOCOL);
    return protocol_invalid_reply_error;
}

const std::error_code& query_command_not_found_error() {
    static const std::error_code query_command_not_found_error = make_error_code(::MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND, ::MONGOC_ERROR_QUERY);
    return query_command_not_found_error;
}

const std::error_code& query_failure_error() {
    static const std::error_code query_failure_error = make_error_code(::MONGOC_ERROR_QUERY_FAILURE, ::MONGOC_ERROR_QUERY);
    return query_failure_error;
}

const std::error_code& query_not_tailable_error() {
    static const std::error_code query_not_tailable_error = make_error_code(::MONGOC_ERROR_QUERY_NOT_TAILABLE, ::MONGOC_ERROR_QUERY);
    return query_not_tailable_error;
}

const std::error_code& collection_does_not_exist_query_error() {
    static const std::error_code collection_does_not_exist_query_error = make_error_code(::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST, ::MONGOC_ERROR_QUERY);
    return collection_does_not_exist_query_error;
}

const std::error_code& scram_not_done_error() {
    static const std::error_code scram_not_done_error = make_error_code(::MONGOC_ERROR_SCRAM_NOT_DONE, ::MONGOC_ERROR_SCRAM);
    return scram_not_done_error;
}

const std::error_code& scram_protocol_error() {
    static const std::error_code scram_protocol_error = make_error_code(::MONGOC_ERROR_SCRAM_PROTOCOL_ERROR, ::MONGOC_ERROR_SCRAM);
    return scram_protocol_error;
}

const std::error_code& server_selection_bad_wire_version_error() {
    static const std::error_code server_selection_bad_wire_version_error = make_error_code(::MONGOC_ERROR_SERVER_SELECTION_BAD_WIRE_VERSION, ::MONGOC_ERROR_SERVER_SELECTION);
    return server_selection_bad_wire_version_error;
}

const std::error_code& server_selection_failure_error() {
    static const std::error_code server_selection_failure_error = make_error_code(::MONGOC_ERROR_SERVER_SELECTION_FAILURE, ::MONGOC_ERROR_SERVER_SELECTION);
    return server_selection_failure_error;
}

const std::error_code& server_selection_invalid_id_error() {
    static const std::error_code server_selection_invalid_id_error = make_error_code(::MONGOC_ERROR_SERVER_SELECTION_INVALID_ID, ::MONGOC_ERROR_SERVER_SELECTION);
    return server_selection_invalid_id_error;
}

const std::error_code& stream_connect_error() {
    static const std::error_code stream_connect_error = make_error_code(::MONGOC_ERROR_STREAM_CONNECT, ::MONGOC_ERROR_STREAM);
    return stream_connect_error;
}

const std::error_code& stream_invalid_state_error() {
    static const std::error_code stream_invalid_state_error = make_error_code(::MONGOC_ERROR_STREAM_INVALID_STATE, ::MONGOC_ERROR_STREAM);
    return stream_invalid_state_error;
}

const std::error_code& stream_invalid_type_error() {
    static const std::error_code stream_invalid_type_error = make_error_code(::MONGOC_ERROR_STREAM_INVALID_TYPE, ::MONGOC_ERROR_STREAM);
    return stream_invalid_type_error;
}

const std::error_code& stream_name_resolution_error() {
    static const std::error_code stream_name_resolution_error = make_error_code(::MONGOC_ERROR_STREAM_NAME_RESOLUTION, ::MONGOC_ERROR_STREAM);
    return stream_name_resolution_error;
}

const std::error_code& stream_not_established_error() {
    static const std::error_code stream_not_established_error = make_error_code(::MONGOC_ERROR_STREAM_NOT_ESTABLISHED, ::MONGOC_ERROR_STREAM);
    return stream_not_established_error;
}

const std::error_code& stream_socket_error() {
    static const std::error_code stream_socket_error = make_error_code(::MONGOC_ERROR_STREAM_SOCKET, ::MONGOC_ERROR_STREAM);
    return stream_socket_error;
}

const std::error_code& invalid_client_object_error() {
    static const std::error_code invalid_client_object_error = make_error_code(error_code::k_invalid_client_object);
    return invalid_client_object_error;
}

const std::error_code& invalid_collection_object_error() {
    static const std::error_code invalid_collection_object_error = make_error_code(error_code::k_invalid_collection_object);
    return invalid_collection_object_error;
}

const std::error_code& invalid_database_object_error() {
    static const std::error_code invalid_database_object_error = make_error_code(error_code::k_invalid_database_object);
    return invalid_database_object_error;
}

const std::error_code& invalid_parameter_error() {
    static const std::error_code invalid_parameter_error = make_error_code(error_code::k_invalid_parameter);
    return invalid_parameter_error;
}

const std::error_code& ssl_not_supported_error() {
    static const std::error_code ssl_not_supported_error = make_error_code(error_code::k_ssl_not_supported);
    return ssl_not_supported_error;
}

const std::error_code& unknown_read_concern_error() {
    static const std::error_code unknown_read_concern_error = make_error_code(error_code::k_unknown_read_concern);
    return unknown_read_concern_error;
}

const std::error_code& unknown_write_concern_error() {
    static const std::error_code unknown_write_concern_error = make_error_code(error_code::k_unknown_write_concern);
    return unknown_write_concern_error;
}

}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
