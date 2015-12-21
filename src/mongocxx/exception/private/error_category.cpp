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
#include <system_error>

#include <mongocxx/exception/private/error_category.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/private/libmongoc.hpp>

namespace {

using namespace mongocxx;

///
/// An error category for libmongoc errors in the MONGOC_ERROR_BSON domain.
///
class mongoc_bson_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc bson error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_BSON_INVALID:
                return "mongoc bson_invalid error";
            default:
                return "unknown mongoc bson error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_CLIENT domain.
///
class mongoc_client_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc client error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_CLIENT_AUTHENTICATE:
                return "mongoc client_authenticate error";
            case ::MONGOC_ERROR_CLIENT_GETNONCE:
                return "mongoc client_getnonce error";
            case ::MONGOC_ERROR_CLIENT_IN_EXHAUST:
                return "mongoc client_in_exhaust error";
            case ::MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
                return "mongoc client_no_acceptable_peer error";
            case ::MONGOC_ERROR_CLIENT_NOT_READY:
                return "mongoc client_not_ready error";
            case ::MONGOC_ERROR_CLIENT_TOO_BIG:
                return "mongoc client_too_big error";
            case ::MONGOC_ERROR_CLIENT_TOO_SMALL:
                return "mongoc client_too_small error";
            default:
                return "unknown mongoc client error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_COLLECTION domain.
///
class mongoc_collection_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc collection error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_COLLECTION_DELETE_FAILED:
                return "mongoc collection_delete_failed error";
            case ::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST:
                return "mongoc collection_does_not_exist error";
            case ::MONGOC_ERROR_COLLECTION_INSERT_FAILED:
                return "mongoc collection_insert_failed error";
            case ::MONGOC_ERROR_COLLECTION_UPDATE_FAILED:
                return "mongoc collection_update_failed error";
            default:
                return "unknown mongoc collection error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_COMMAND domain.
///
class mongoc_command_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc command error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_COMMAND_INVALID_ARG:
                return "mongoc command_invalid_arg error";
            default:
                return "unknown mongoc command error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_CURSOR domain.
///
class mongoc_cursor_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc cursor error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_CURSOR_INVALID_CURSOR:
                return "mongoc cursor_invalid_cursor error";
            default:
                return "unknown mongoc cursor error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_GRIDFS domain.
///
class mongoc_gridfs_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc gridfs error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
                return "mongoc gridfs_invalid_filename error";
            default:
                return "unknown mongoc gridfs error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_INSERT domain.
///
class mongoc_insert_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc insert error";
    }

    std::string message(int code) const noexcept override {
        return "unknown mongoc insert error";
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_MATCHER domain.
///
class mongoc_matcher_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc matcher error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_MATCHER_INVALID:
                return "mongoc matcher_invalid error";
            default:
                return "unknown mongoc matcher error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_NAMESPACE domain.
///
class mongoc_namespace_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc namespace error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_NAMESPACE_INVALID:
                return "mongoc namespace_invalid error";
            case ::MONGOC_ERROR_NAMESPACE_INVALID_FILTER_TYPE:
                return "mongoc namespace_invalid_filter_type error";
            default:
                return "unknown mongoc namespace error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_PROTOCOL domain.
///
class mongoc_protocol_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc protocol error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
                return "mongoc protocol_bad_wire_version error";
            case ::MONGOC_ERROR_PROTOCOL_ERROR:
                return "mongoc protocol_error error";
            case ::MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
                return "mongoc protocol_invalid_reply error";
            default:
                return "unknown mongoc protocol error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_QUERY domain.
///
class mongoc_query_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc query error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
                return "mongoc query_command_not_found error";
            case ::MONGOC_ERROR_QUERY_FAILURE:
                return "mongoc query_failure error";
            case ::MONGOC_ERROR_QUERY_NOT_TAILABLE:
                return "mongoc query_not_tailable error";
            default:
                return "unknown mongoc query error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_SASL domain.
///
class mongoc_sasl_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc sasl error";
    }

    std::string message(int code) const noexcept override {
        return "unknown mongoc sasl error";
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_SCRAM domain.
///
class mongoc_scram_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc scram error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_SCRAM_NOT_DONE:
                return "mongoc scram_not_done error";
            case ::MONGOC_ERROR_SCRAM_PROTOCOL_ERROR:
                return "mongoc scram_protocol_error error";
            default:
                return "unknown mongoc scram error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_SERVER_SELECTION domain.
///
class mongoc_server_selection_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc server selection error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_SERVER_SELECTION_BAD_WIRE_VERSION:
                return "mongoc server_selection_bad_wire_version error";
            case ::MONGOC_ERROR_SERVER_SELECTION_FAILURE:
                return "mongoc server_selection_failure error";
            case ::MONGOC_ERROR_SERVER_SELECTION_INVALID_ID:
                return "mongoc server_selection_invalid_id error";
            default:
                return "unknown mongoc server selection error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_STREAM domain.
///
class mongoc_stream_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc stream error";
    }

    std::string message(int code) const noexcept override {
        switch (code) {
            case ::MONGOC_ERROR_STREAM_CONNECT:
                return "mongoc stream_connect error";
            case ::MONGOC_ERROR_STREAM_INVALID_STATE:
                return "mongoc stream_invalid_state error";
            case ::MONGOC_ERROR_STREAM_INVALID_TYPE:
                return "mongoc stream_invalid_type error";
            case ::MONGOC_ERROR_STREAM_NAME_RESOLUTION:
                return "mongoc stream_name_resolution error";
            case ::MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
                return "mongoc stream_not_established error";
            case ::MONGOC_ERROR_STREAM_SOCKET:
                return "mongoc stream_socket error";
            default:
                return "unknown mongoc stream error";
        }
    }
};

///
/// An error category for libmongoc errors in the MONGOC_ERROR_WRITE_CONCERN domain.
///
class mongoc_write_concern_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc write concern error";
    }

    std::string message(int code) const noexcept override {
        return "unknown mongoc write concern error";
    }
};

///
/// An error category for libmongoc errors of unknown domain.
///
class mongoc_unknown_error_category final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "libmongoc unknown error";
    }

    std::string message(int code) const noexcept override {
        return "unknown mongoc error";
    }
};

///
/// An error_category for codes for inherent mongocxx errors.
///
class mongocxx_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "mongocxx error";
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
            default:
                return "unknown mongocxx error";
        }
    }
};
}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Get the error_category for exceptions originating from the libmongoc library.
///
/// @param domain A libmongoc error domain
///
/// @return The mongoc error_category
///
const std::error_category& mongoc_error_category(int domain) {
    switch (domain) {
        case ::MONGOC_ERROR_BSON:
            static const mongoc_bson_error_category bson_category{};
            return bson_category;
        case ::MONGOC_ERROR_CLIENT:
            static const mongoc_client_error_category client_category{};
            return client_category;
        case ::MONGOC_ERROR_COLLECTION:
            static const mongoc_collection_error_category collection_category{};
            return collection_category;
        case ::MONGOC_ERROR_COMMAND:
            static const mongoc_command_error_category command_category{};
            return command_category;
        case ::MONGOC_ERROR_CURSOR:
            static const mongoc_cursor_error_category cursor_category{};
            return cursor_category;
        case ::MONGOC_ERROR_GRIDFS:
            static const mongoc_gridfs_error_category gridfs_category{};
            return gridfs_category;
        case ::MONGOC_ERROR_INSERT:
            static const mongoc_insert_error_category insert_category{};
            return insert_category;
        case ::MONGOC_ERROR_MATCHER:
            static const mongoc_matcher_error_category matcher_category{};
            return matcher_category;
        case ::MONGOC_ERROR_NAMESPACE:
            static const mongoc_namespace_error_category namespace_category{};
            return namespace_category;
        case ::MONGOC_ERROR_PROTOCOL:
            static const mongoc_protocol_error_category protocol_category{};
            return protocol_category;
        case ::MONGOC_ERROR_QUERY:
            static const mongoc_query_error_category query_category{};
            return query_category;
        case ::MONGOC_ERROR_SASL:
            static const mongoc_sasl_error_category sasl_category{};
            return sasl_category;
        case ::MONGOC_ERROR_SCRAM:
            static const mongoc_scram_error_category scram_category{};
            return scram_category;
        case ::MONGOC_ERROR_SERVER_SELECTION:
            static const mongoc_server_selection_error_category server_selection_category{};
            return server_selection_category;
        case ::MONGOC_ERROR_STREAM:
            static const mongoc_stream_error_category stream_category{};
            return stream_category;
        case ::MONGOC_ERROR_WRITE_CONCERN:
            static const mongoc_write_concern_error_category write_concern_category{};
            return write_concern_category;
        default:
            static const mongoc_unknown_error_category unknown_category{};
            return unknown_category;
    }
}

///
/// Get the error_category for inherent mongocxx exceptions.
///
/// @return The mongocxx error_category
///
const std::error_category& mongocxx_error_category() {
    static const mongocxx_error_category_impl mongocxx_category{};
    return mongocxx_category;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
