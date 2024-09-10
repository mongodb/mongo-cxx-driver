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

#include <string>

#include <mongocxx/exception/error_code.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

//
// An error_category for codes generated by the client.
//
class error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "mongocxx";
    }

    std::string message(int code) const noexcept override {
        switch (static_cast<error_code>(code)) {
            case error_code::k_invalid_client_object:
                return "invalid use of default constructed or moved-from mongocxx::client object";
            case error_code::k_invalid_collection_object:
                return "invalid use of default constructed or moved-from mongocxx::collection "
                       "object";
            case error_code::k_invalid_database_object:
                return "invalid use of default constructed or moved-from mongocxx::database object";
            case error_code::k_invalid_parameter:
                return "an invalid or out-of-bounds parameter was provided";
            case error_code::k_ssl_not_supported:
                return "SSL support not available";
            case error_code::k_unknown_read_concern:
                return "invalid attempt to set an unknown read concern level";
            case error_code::k_unknown_write_concern:
                return "invalid attempt to set an unknown write concern level";
            case error_code::k_cannot_recreate_instance:
                return "cannot create a mongocxx::instance object if one has already been created";
            case error_code::k_server_response_malformed:
                return "the response from the server was malformed";
            case error_code::k_invalid_uri:
                return "an invalid MongoDB URI was provided";
            case error_code::k_invalid_gridfs_bucket_object:
                return "invalid use of default constructed or moved-from mongocxx::gridfs::bucket "
                       "object";
            case error_code::k_invalid_gridfs_uploader_object:
                return "invalid use of default constructed or moved-from "
                       "mongocxx::gridfs::uploader object";
            case error_code::k_invalid_gridfs_downloader_object:
                return "invalid use of default constructed or moved-from "
                       "mongocxx::gridfs::downloader object";
            case error_code::k_gridfs_stream_not_open:
                return "a mongocxx::gridfs::uploader object was not open for writing, or a "
                       "mongocxx::gridfs::downloader object was not open for "
                       "reading";
            case error_code::k_gridfs_upload_requires_too_many_chunks:
                return "a mongocxx::gridfs::uploader object has exceeded the maximum number "
                       "of allowable GridFS chunks when attempting to upload the requested "
                       "file";
            case error_code::k_gridfs_file_not_found:
                return "the requested GridFS file was not found";
            case error_code::k_gridfs_file_corrupted:
                return "a GridFS file being operated on was discovered to be corrupted";
            case error_code::k_instance_destroyed:
                return "the mongocxx instance has been destroyed";
            case error_code::k_cannot_create_session:
                return "failed to create a client session";
            case error_code::k_invalid_session:
                return "an invalid client session was provided";
            case error_code::k_invalid_transaction_options_object:
                return "an invalid transactions options object was provided";
            case error_code::k_create_resource_fail:
                return "could not create resource";
            case error_code::k_invalid_search_index_model:
                return "invalid use of default constructed or moved-from "
                       "mongocxx::search_index_model object";
            case error_code::k_invalid_search_index_view:
                return "invalid use of default constructed or moved-from "
                       "mongocxx::search_index_view object";
            case error_code::k_invalid_client_acquired:
                return "invalid mongocxx::v_noabi::client object was acquired, "
                       "possibly due to curi-parameter 'waitQueueTimeoutMS' limits";
            default:
                return "unknown mongocxx error";
        }
    }
};

}  // namespace

const std::error_category& MONGOCXX_CALL error_category() {
    static const class error_category category {};
    return category;
}

}  // namespace v_noabi
}  // namespace mongocxx
