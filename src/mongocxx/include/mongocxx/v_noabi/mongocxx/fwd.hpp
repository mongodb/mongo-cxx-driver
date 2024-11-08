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

#pragma once

#include <mongocxx/bulk_write-fwd.hpp>
#include <mongocxx/change_stream-fwd.hpp>
#include <mongocxx/client-fwd.hpp>
#include <mongocxx/client_encryption-fwd.hpp>
#include <mongocxx/client_session-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/cursor-fwd.hpp>
#include <mongocxx/database-fwd.hpp>
#include <mongocxx/events/command_failed_event-fwd.hpp>
#include <mongocxx/events/command_started_event-fwd.hpp>
#include <mongocxx/events/command_succeeded_event-fwd.hpp>
#include <mongocxx/events/heartbeat_failed_event-fwd.hpp>
#include <mongocxx/events/heartbeat_started_event-fwd.hpp>
#include <mongocxx/events/heartbeat_succeeded_event-fwd.hpp>
#include <mongocxx/events/server_changed_event-fwd.hpp>
#include <mongocxx/events/server_closed_event-fwd.hpp>
#include <mongocxx/events/server_description-fwd.hpp>
#include <mongocxx/events/server_opening_event-fwd.hpp>
#include <mongocxx/events/topology_changed_event-fwd.hpp>
#include <mongocxx/events/topology_closed_event-fwd.hpp>
#include <mongocxx/events/topology_description-fwd.hpp>
#include <mongocxx/events/topology_opening_event-fwd.hpp>
#include <mongocxx/exception/authentication_exception-fwd.hpp>
#include <mongocxx/exception/bulk_write_exception-fwd.hpp>
#include <mongocxx/exception/error_code-fwd.hpp>
#include <mongocxx/exception/exception-fwd.hpp>
#include <mongocxx/exception/gridfs_exception-fwd.hpp>
#include <mongocxx/exception/logic_error-fwd.hpp>
#include <mongocxx/exception/operation_exception-fwd.hpp>
#include <mongocxx/exception/query_exception-fwd.hpp>
#include <mongocxx/exception/server_error_code-fwd.hpp>
#include <mongocxx/exception/write_exception-fwd.hpp>
#include <mongocxx/gridfs/bucket-fwd.hpp>
#include <mongocxx/gridfs/downloader-fwd.hpp>
#include <mongocxx/gridfs/uploader-fwd.hpp>
#include <mongocxx/hint-fwd.hpp>
#include <mongocxx/index_model-fwd.hpp>
#include <mongocxx/index_view-fwd.hpp>
#include <mongocxx/instance-fwd.hpp>
#include <mongocxx/logger-fwd.hpp>
#include <mongocxx/model/delete_many-fwd.hpp>
#include <mongocxx/model/delete_one-fwd.hpp>
#include <mongocxx/model/insert_one-fwd.hpp>
#include <mongocxx/model/replace_one-fwd.hpp>
#include <mongocxx/model/update_many-fwd.hpp>
#include <mongocxx/model/update_one-fwd.hpp>
#include <mongocxx/model/write-fwd.hpp>
#include <mongocxx/options/aggregate-fwd.hpp>
#include <mongocxx/options/apm-fwd.hpp>
#include <mongocxx/options/auto_encryption-fwd.hpp>
#include <mongocxx/options/bulk_write-fwd.hpp>
#include <mongocxx/options/change_stream-fwd.hpp>
#include <mongocxx/options/client-fwd.hpp>
#include <mongocxx/options/client_encryption-fwd.hpp>
#include <mongocxx/options/client_session-fwd.hpp>
#include <mongocxx/options/count-fwd.hpp>
#include <mongocxx/options/data_key-fwd.hpp>
#include <mongocxx/options/delete-fwd.hpp>
#include <mongocxx/options/distinct-fwd.hpp>
#include <mongocxx/options/encrypt-fwd.hpp>
#include <mongocxx/options/estimated_document_count-fwd.hpp>
#include <mongocxx/options/find-fwd.hpp>
#include <mongocxx/options/find_one_and_delete-fwd.hpp>
#include <mongocxx/options/find_one_and_replace-fwd.hpp>
#include <mongocxx/options/find_one_and_update-fwd.hpp>
#include <mongocxx/options/find_one_common_options-fwd.hpp>
#include <mongocxx/options/gridfs/bucket-fwd.hpp>
#include <mongocxx/options/gridfs/upload-fwd.hpp>
#include <mongocxx/options/index-fwd.hpp>
#include <mongocxx/options/index_view-fwd.hpp>
#include <mongocxx/options/insert-fwd.hpp>
#include <mongocxx/options/pool-fwd.hpp>
#include <mongocxx/options/range-fwd.hpp>
#include <mongocxx/options/replace-fwd.hpp>
#include <mongocxx/options/rewrap_many_datakey-fwd.hpp>
#include <mongocxx/options/server_api-fwd.hpp>
#include <mongocxx/options/tls-fwd.hpp>
#include <mongocxx/options/transaction-fwd.hpp>
#include <mongocxx/options/update-fwd.hpp>
#include <mongocxx/pipeline-fwd.hpp>
#include <mongocxx/pool-fwd.hpp>
#include <mongocxx/read_concern-fwd.hpp>
#include <mongocxx/read_preference-fwd.hpp>
#include <mongocxx/result/bulk_write-fwd.hpp>
#include <mongocxx/result/delete-fwd.hpp>
#include <mongocxx/result/gridfs/upload-fwd.hpp>
#include <mongocxx/result/insert_many-fwd.hpp>
#include <mongocxx/result/insert_one-fwd.hpp>
#include <mongocxx/result/replace_one-fwd.hpp>
#include <mongocxx/result/rewrap_many_datakey-fwd.hpp>
#include <mongocxx/result/update-fwd.hpp>
#include <mongocxx/search_index_model-fwd.hpp>
#include <mongocxx/search_index_view-fwd.hpp>
#include <mongocxx/uri-fwd.hpp>
#include <mongocxx/validation_criteria-fwd.hpp>
#include <mongocxx/write_concern-fwd.hpp>
#include <mongocxx/write_type-fwd.hpp>

///
/// @file
/// Aggregate of all forward headers declaring entities in @ref mongocxx::v_noabi.
///
/// @par Includes
/// - All header files under @ref src/mongocxx/include/mongocxx/v_noabi/mongocxx "mongocxx/v_noabi/mongocxx" whose filename ends with `-fwd.hpp`.
///

///
/// @dir mongocxx/v_noabi
/// The directory relative to which headers declaring entities in @ref mongocxx::v_noabi
/// are typically included.
///
/// For backward compatibility, unstable ABI headers may be included using the syntax
/// `<mongocxx/foo.hpp>`, which is equivalent to `<mongocxx/v_noabi/mongocxx/foo.hpp>`.
///

///
/// @dir mongocxx/v_noabi/mongocxx
/// Provides headers declaring entities in @ref mongocxx::v_noabi.
///

///
/// @dir mongocxx/v_noabi/mongocxx/config
/// Provides headers related to mongocxx library configuration.
///

#if defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @defgroup mongocxx-v_noabi-mongocxx-config-config-hpp mongocxx/v_noabi/mongocxx/config/config.hpp
/// This page documents macros provided by the generated header
/// `mongocxx/v_noabi/mongocxx/config/config.hpp`.
///
/// @name mongocxx/v_noabi/mongocxx/config/config.hpp
///
/// These macros are provided by @ref mongocxx-v_noabi-mongocxx-config-config-hpp "config.hpp",
/// **NOT** by @ref mongocxx/v_noabi/mongocxx/fwd.hpp "fwd.hpp" (where this is documented)!
///
/// @{
///

///
/// @} <!-- mongocxx/v_noabi/mongocxx/config/config.hpp -->
///

///
/// @defgroup mongocxx-v_noabi-mongocxx-config-export-hpp mongocxx/v_noabi/mongocxx/config/export.hpp
/// This page documents macros provided by the generated header
/// `mongocxx/v_noabi/mongocxx/config/export.hpp`.
///
/// @name mongocxx/v_noabi/mongocxx/config/export.hpp
///
/// These macros are provided by @ref mongocxx-v_noabi-mongocxx-config-export-hpp "export.hpp",
/// **NOT** by @ref mongocxx/v_noabi/mongocxx/fwd.hpp "fwd.hpp" (where this is documented)!
///
/// @{
///

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-export-hpp
/// Exports the associated entity as part of the ABI.
///
/// @warning For internal use only!
///
#define MONGOCXX_ABI_EXPORT

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-export-hpp
/// Excludes the associated entity from being part of the ABI.
///
/// @warning For internal use only!
///
#define MONGOCXX_ABI_NO_EXPORT

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-export-hpp
/// Expands to `__cdecl` when built with MSVC on Windows.
///
/// @warning For internal use only!
///
#define MONGOCXX_ABI_CDECL

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-export-hpp
/// Equivalent to @ref MONGOCXX_ABI_EXPORT with @ref MONGOCXX_ABI_CDECL.
///
/// @warning For internal use only!
///
#define MONGOCXX_ABI_EXPORT_CDECL(...) MONGOCXX_ABI_EXPORT __VA_ARGS__ MONGOCXX_ABI_CDECL

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-export-hpp
/// Declares the associated entity as deprecated.
///
/// @warning For internal use only!
///
#define MONGOCXX_DEPRECATED

///
/// @} <!-- mongocxx/v_noabi/mongocxx/config/export.hpp -->
///

///
/// @defgroup mongocxx-v_noabi-mongocxx-config-version-hpp mongocxx/v_noabi/mongocxx/config/version.hpp
/// This page documents macros provided by the generated header
/// `mongocxx/v_noabi/mongocxx/config/version.hpp`.
///
/// @name mongocxx/v_noabi/mongocxx/config/version.hpp
///
/// These macros are provided by @ref mongocxx-v_noabi-mongocxx-config-version-hpp "version.hpp",
/// **NOT** by @ref mongocxx/v_noabi/mongocxx/fwd.hpp "fwd.hpp" (where this is documented)!
///
/// @{
///

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-version-hpp
/// Defined as a string literal encoding this mongocxx library's API version number.
///
/// This value includes the major version, minor version, patch version, and suffix if any.
///
/// Example: `"1.2.3-extra"`
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define MONGOCXX_VERSION_STRING

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-version-hpp
/// Defined as this mongocxx library's API major version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define MONGOCXX_VERSION_MAJOR

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-version-hpp
/// Defined as this mongocxx library's API minor version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define MONGOCXX_VERSION_MINOR

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-version-hpp
/// Defined as this mongocxx library's API patch version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define MONGOCXX_VERSION_PATCH

///
/// @ingroup mongocxx-v_noabi-mongocxx-config-version-hpp
/// Defined as this mongocxx library's API version number suffix if any.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define MONGOCXX_VERSION_EXTRA

///
/// @} <!-- mongocxx/v_noabi/mongocxx/config/version.hpp -->
///

#endif  // defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @dir mongocxx/v_noabi/mongocxx/events
/// Provides headers declaring entities in @ref mongocxx::v_noabi::events.
///

///
/// @dir mongocxx/v_noabi/mongocxx/exception
/// Provides headers declaring entities in @ref mongocxx::v_noabi::exception.
///

///
/// @dir mongocxx/v_noabi/mongocxx/gridfs
/// Provides headers declaring entities in @ref mongocxx::v_noabi::gridfs.
///

///
/// @dir mongocxx/v_noabi/mongocxx/model
/// Provides headers declaring entities in @ref mongocxx::v_noabi::model.
///

///
/// @dir mongocxx/v_noabi/mongocxx/options
/// Provides headers declaring entities in @ref mongocxx::v_noabi::options.
///

///
/// @dir mongocxx/v_noabi/mongocxx/options/gridfs
/// Provides headers declaring entities in @ref mongocxx::v_noabi::options::gridfs.
///

///
/// @dir mongocxx/v_noabi/mongocxx/result
/// Provides headers declaring entities in @ref mongocxx::v_noabi::result.
///

///
/// @dir mongocxx/v_noabi/mongocxx/result/gridfs
/// Provides headers declaring entities in @ref mongocxx::v_noabi::result::gridfs.
///

///
/// @namespace mongocxx::v_noabi
/// Declares entities whose ABI stability is NOT guaranteed.
///

///
/// @namespace mongocxx::v_noabi::events
/// @copydoc mongocxx::events
///

///
/// @namespace mongocxx::v_noabi::gridfs
/// @copydoc mongocxx::gridfs
///

///
/// @namespace mongocxx::v_noabi::model
/// @copydoc mongocxx::model
///
/// @see
/// - @ref mongocxx::v_noabi::bulk_write
///

///
/// @namespace mongocxx::v_noabi::options
/// @copydoc mongocxx::options
///

///
/// @namespace mongocxx::v_noabi::options::gridfs
/// @copydoc mongocxx::options::gridfs
///

///
/// @namespace mongocxx::v_noabi::result
/// @copydoc mongocxx::result
///

///
/// @namespace mongocxx::v_noabi::result::gridfs
/// @copydoc mongocxx::result::gridfs
///
