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

#include <string>

#include <mongocxx/exception/error_category.hpp>

#include <mongocxx/exception/mongoc_error.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace exception {

///
/// A std::error_category for codes returned by libmongoc (via bson_error_t's).
///
class mongoc_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "mongoc";
    }

    std::string message(int ev) const override {
        switch (static_cast<mongoc_error>(ev)) {
        case mongoc_error::k_bson_invalid:
            return "mongoc bson_invalid error";
        case mongoc_error::k_client_authenticate:
            return "mongoc client_authenticate error";
        case mongoc_error::k_client_getnonce:
            return "mongoc client_getnonce error";
        case mongoc_error::k_client_in_exhaust:
            return "mongoc client_in_exhaust error";
        case mongoc_error::k_client_no_acceptable_peer:
            return "mongoc client_no_acceptable_peer error";
        case mongoc_error::k_client_not_ready:
            return "mongoc client_not_ready error";
        case mongoc_error::k_client_too_big:
            return "mongoc client_too_big error";
        case mongoc_error::k_client_too_small:
            return "mongoc client_too_small error";
        case mongoc_error::k_collection_delete_failed:
            return "mongoc collection_delete_failed error";
        case mongoc_error::k_collection_does_not_exist:
            return "mongoc collection_does_not_exist error";
        case mongoc_error::k_collection_insert_failed:
            return "mongoc collection_insert_failed error";
        case mongoc_error::k_collection_update_failed:
            return "mongoc collection_update_failed error";
        case mongoc_error::k_command_invalid_arg:
            return "mongoc command_invalid_arg error";
        case mongoc_error::k_cursor_invalid_cursor:
            return "mongoc cursor_invalid_cursor error";
        case mongoc_error::k_gridfs_invalid_filename:
            return "mongoc gridfs_invalid_filename error";
        case mongoc_error::k_matcher_invalid:
            return "mongoc matcher_invalid error";
        case mongoc_error::k_namespace_invalid:
            return "mongoc namespace_invalid error";
        case mongoc_error::k_namespace_invalid_filter_type:
            return "mongoc namespace_invalid_filter_type error";
        case mongoc_error::k_protocol_bad_wire_version:
            return "mongoc protocol_bad_wire_version error";
        case mongoc_error::k_protocol_error:
            return "mongoc protocol_error error";
        case mongoc_error::k_protocol_invalid_reply:
            return "mongoc protocol_invalid_reply error";
        case mongoc_error::k_query_command_not_found:
            return "mongoc query_command_not_found error";
        case mongoc_error::k_query_failure:
            return "mongoc query_failure error";
        case mongoc_error::k_query_not_tailable:
            return "mongoc query_not_tailable error";
        case mongoc_error::k_scram_not_done:
            return "mongoc scram_not_done error";
        case mongoc_error::k_scram_protocol_error:
            return "mongoc scram_protocol_error error";
        case mongoc_error::k_server_selection_bad_wire_version:
            return "mongoc server_selection_bad_wire_version error";
        case mongoc_error::k_server_selection_failure:
            return "mongoc server_selection_failure error";
        case mongoc_error::k_server_selection_invalid_id:
            return "mongoc server_selection_invalid_id error";
        case mongoc_error::k_stream_connect:
            return "mongoc stream_connect error";
        case mongoc_error::k_stream_invalid_state:
            return "mongoc stream_invalid_state error";
        case mongoc_error::k_stream_invalid_type:
            return "mongoc stream_invalid_type error";
        case mongoc_error::k_stream_name_resolution:
            return "mongoc stream_name_resolution error";
        case mongoc_error::k_stream_not_established:
            return "mongoc stream_not_established error";
        case mongoc_error::k_stream_socket:
            return "mongoc stream_socket error";
        case mongoc_error::k_unknown:
        default:
            return "unknown mongoc error";
        }
    }
};

const std::error_category& mongoc_error_category() {
    static const mongoc_error_category_impl instance{};
    return instance;
}

std::error_code make_error_code(mongoc_error error) {
    return std::error_code(static_cast<int>(error), mongoc_error_category());
}

std::error_condition make_error_condition(mongoc_error error) {
    return std::error_condition(static_cast<int>(error), mongoc_error_category());
}

}  // namespace exception
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
