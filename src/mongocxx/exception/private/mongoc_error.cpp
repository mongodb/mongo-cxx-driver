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

#include <mongocxx/exception/private/mongoc_error.hpp>

#include <mongocxx/exception/error_category.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

mongoc_error convert_mongoc_error(int code, int domain) {
    switch (domain) {
    case ::MONGOC_ERROR_BSON:
        switch (code) {
        case ::MONGOC_ERROR_BSON_INVALID:
            return mongoc_error::k_bson_invalid;
        }
    case ::MONGOC_ERROR_CLIENT:
        switch (code) {
        case ::MONGOC_ERROR_CLIENT_AUTHENTICATE:
            return mongoc_error::k_client_authenticate;
        case ::MONGOC_ERROR_CLIENT_GETNONCE:
            return mongoc_error::k_client_getnonce;
        case ::MONGOC_ERROR_CLIENT_IN_EXHAUST:
            return mongoc_error::k_client_in_exhaust;
        case ::MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
            return mongoc_error::k_client_no_acceptable_peer;
        case ::MONGOC_ERROR_CLIENT_NOT_READY:
            return mongoc_error::k_client_not_ready;
        case ::MONGOC_ERROR_CLIENT_TOO_BIG:
            return mongoc_error::k_client_too_big;
        case ::MONGOC_ERROR_CLIENT_TOO_SMALL:
            return mongoc_error::k_client_too_small;
        }
    case ::MONGOC_ERROR_COLLECTION:
        switch (code) {
        case ::MONGOC_ERROR_COLLECTION_DELETE_FAILED:
            return mongoc_error::k_collection_delete_failed;
        case ::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST:
            return mongoc_error::k_collection_does_not_exist;
        case ::MONGOC_ERROR_COLLECTION_INSERT_FAILED:
            return mongoc_error::k_collection_insert_failed;
        case ::MONGOC_ERROR_COLLECTION_UPDATE_FAILED:
            return mongoc_error::k_collection_update_failed;
        }
    case ::MONGOC_ERROR_COMMAND:
        switch (code) {
        case ::MONGOC_ERROR_COMMAND_INVALID_ARG:
            return mongoc_error::k_command_invalid_arg;
        }
    case ::MONGOC_ERROR_CURSOR:
        switch (code) {
        case ::MONGOC_ERROR_CURSOR_INVALID_CURSOR:
            return mongoc_error::k_cursor_invalid_cursor;
        }
    case ::MONGOC_ERROR_GRIDFS:
        switch (code) {
        case ::MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
            return mongoc_error::k_gridfs_invalid_filename;
        }
    case ::MONGOC_ERROR_INSERT:
        switch (code) {
            // unused in libmongoc
        }
    case ::MONGOC_ERROR_MATCHER:
        switch (code) {
        case ::MONGOC_ERROR_MATCHER_INVALID:
            return mongoc_error::k_matcher_invalid;
        }
    case ::MONGOC_ERROR_NAMESPACE:
        switch (code) {
        case ::MONGOC_ERROR_NAMESPACE_INVALID:
            return mongoc_error::k_namespace_invalid;
        case ::MONGOC_ERROR_NAMESPACE_INVALID_FILTER_TYPE:
            return mongoc_error::k_namespace_invalid_filter_type;
        }
    case ::MONGOC_ERROR_PROTOCOL:
        switch (code) {
        case ::MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
            return mongoc_error::k_protocol_bad_wire_version;
        case ::MONGOC_ERROR_PROTOCOL_ERROR:
            return mongoc_error::k_protocol_error;
        case ::MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
            return mongoc_error::k_protocol_invalid_reply;
        }
    case ::MONGOC_ERROR_QUERY:
        switch (code) {
        case ::MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
            return mongoc_error::k_query_command_not_found;
        case ::MONGOC_ERROR_QUERY_FAILURE:
            return mongoc_error::k_query_failure;
        case ::MONGOC_ERROR_QUERY_NOT_TAILABLE:
            return mongoc_error::k_query_not_tailable;
        }
    case ::MONGOC_ERROR_SASL:
        switch (code) {
            // unused in libmongoc
        }
    case ::MONGOC_ERROR_SCRAM:
        switch (code) {
        case ::MONGOC_ERROR_SCRAM_NOT_DONE:
            return mongoc_error::k_scram_not_done;
        case ::MONGOC_ERROR_SCRAM_PROTOCOL_ERROR:
            return mongoc_error::k_scram_protocol_error;
        }
    case ::MONGOC_ERROR_SERVER_SELECTION:
        switch (code) {
        case ::MONGOC_ERROR_SERVER_SELECTION_BAD_WIRE_VERSION:
            return mongoc_error::k_server_selection_bad_wire_version;
        case ::MONGOC_ERROR_SERVER_SELECTION_FAILURE:
            return mongoc_error::k_server_selection_failure;
        case ::MONGOC_ERROR_SERVER_SELECTION_INVALID_ID:
            return mongoc_error::k_server_selection_invalid_id;
        }
    case ::MONGOC_ERROR_STREAM:
        switch (code) {
        case ::MONGOC_ERROR_STREAM_CONNECT:
            return mongoc_error::k_stream_connect;
        case ::MONGOC_ERROR_STREAM_INVALID_STATE:
            return mongoc_error::k_stream_invalid_state;
        case ::MONGOC_ERROR_STREAM_INVALID_TYPE:
            return mongoc_error::k_stream_invalid_type;
        case ::MONGOC_ERROR_STREAM_NAME_RESOLUTION:
            return mongoc_error::k_stream_name_resolution;
        case ::MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
            return mongoc_error::k_stream_not_established;
        case ::MONGOC_ERROR_STREAM_SOCKET:
            return mongoc_error::k_stream_socket;
        }
    case ::MONGOC_ERROR_WRITE_CONCERN:
        switch (code) {
            // unused in libmongoc
        }
    }
    return mongoc_error::k_unknown;
}

std::error_code error_code_from_bson_error_t(::bson_error_t error) {
    return make_error_code(convert_mongoc_error(error.code, error.domain));
}

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
