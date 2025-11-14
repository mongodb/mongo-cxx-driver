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

#include <mongocxx/bulk_write-fwd.hpp>                         // IWYU pragma: export
#include <mongocxx/change_stream-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/client-fwd.hpp>                             // IWYU pragma: export
#include <mongocxx/client_encryption-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/client_session-fwd.hpp>                     // IWYU pragma: export
#include <mongocxx/collection-fwd.hpp>                         // IWYU pragma: export
#include <mongocxx/cursor-fwd.hpp>                             // IWYU pragma: export
#include <mongocxx/database-fwd.hpp>                           // IWYU pragma: export
#include <mongocxx/events/command_failed_event-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/events/command_started_event-fwd.hpp>       // IWYU pragma: export
#include <mongocxx/events/command_succeeded_event-fwd.hpp>     // IWYU pragma: export
#include <mongocxx/events/heartbeat_failed_event-fwd.hpp>      // IWYU pragma: export
#include <mongocxx/events/heartbeat_started_event-fwd.hpp>     // IWYU pragma: export
#include <mongocxx/events/heartbeat_succeeded_event-fwd.hpp>   // IWYU pragma: export
#include <mongocxx/events/server_changed_event-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/events/server_closed_event-fwd.hpp>         // IWYU pragma: export
#include <mongocxx/events/server_description-fwd.hpp>          // IWYU pragma: export
#include <mongocxx/events/server_opening_event-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/events/topology_changed_event-fwd.hpp>      // IWYU pragma: export
#include <mongocxx/events/topology_closed_event-fwd.hpp>       // IWYU pragma: export
#include <mongocxx/events/topology_description-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/events/topology_opening_event-fwd.hpp>      // IWYU pragma: export
#include <mongocxx/exception/authentication_exception-fwd.hpp> // IWYU pragma: export
#include <mongocxx/exception/bulk_write_exception-fwd.hpp>     // IWYU pragma: export
#include <mongocxx/exception/error_code-fwd.hpp>               // IWYU pragma: export
#include <mongocxx/exception/exception-fwd.hpp>                // IWYU pragma: export
#include <mongocxx/exception/gridfs_exception-fwd.hpp>         // IWYU pragma: export
#include <mongocxx/exception/logic_error-fwd.hpp>              // IWYU pragma: export
#include <mongocxx/exception/operation_exception-fwd.hpp>      // IWYU pragma: export
#include <mongocxx/exception/query_exception-fwd.hpp>          // IWYU pragma: export
#include <mongocxx/exception/server_error_code-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/exception/write_exception-fwd.hpp>          // IWYU pragma: export
#include <mongocxx/gridfs/bucket-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/gridfs/downloader-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/gridfs/uploader-fwd.hpp>                    // IWYU pragma: export
#include <mongocxx/hint-fwd.hpp>                               // IWYU pragma: export
#include <mongocxx/index_model-fwd.hpp>                        // IWYU pragma: export
#include <mongocxx/index_view-fwd.hpp>                         // IWYU pragma: export
#include <mongocxx/instance-fwd.hpp>                           // IWYU pragma: export
#include <mongocxx/logger-fwd.hpp>                             // IWYU pragma: export
#include <mongocxx/model/delete_many-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/model/delete_one-fwd.hpp>                   // IWYU pragma: export
#include <mongocxx/model/insert_one-fwd.hpp>                   // IWYU pragma: export
#include <mongocxx/model/replace_one-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/model/update_many-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/model/update_one-fwd.hpp>                   // IWYU pragma: export
#include <mongocxx/model/write-fwd.hpp>                        // IWYU pragma: export
#include <mongocxx/options/aggregate-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/options/apm-fwd.hpp>                        // IWYU pragma: export
#include <mongocxx/options/auto_encryption-fwd.hpp>            // IWYU pragma: export
#include <mongocxx/options/bulk_write-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/options/change_stream-fwd.hpp>              // IWYU pragma: export
#include <mongocxx/options/client-fwd.hpp>                     // IWYU pragma: export
#include <mongocxx/options/client_encryption-fwd.hpp>          // IWYU pragma: export
#include <mongocxx/options/client_session-fwd.hpp>             // IWYU pragma: export
#include <mongocxx/options/count-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/options/data_key-fwd.hpp>                   // IWYU pragma: export
#include <mongocxx/options/delete-fwd.hpp>                     // IWYU pragma: export
#include <mongocxx/options/distinct-fwd.hpp>                   // IWYU pragma: export
#include <mongocxx/options/encrypt-fwd.hpp>                    // IWYU pragma: export
#include <mongocxx/options/estimated_document_count-fwd.hpp>   // IWYU pragma: export
#include <mongocxx/options/find-fwd.hpp>                       // IWYU pragma: export
#include <mongocxx/options/find_one_and_delete-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/options/find_one_and_replace-fwd.hpp>       // IWYU pragma: export
#include <mongocxx/options/find_one_and_update-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/options/find_one_common_options-fwd.hpp>    // IWYU pragma: export
#include <mongocxx/options/gridfs/bucket-fwd.hpp>              // IWYU pragma: export
#include <mongocxx/options/gridfs/upload-fwd.hpp>              // IWYU pragma: export
#include <mongocxx/options/index-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/options/index_view-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/options/insert-fwd.hpp>                     // IWYU pragma: export
#include <mongocxx/options/pool-fwd.hpp>                       // IWYU pragma: export
#include <mongocxx/options/range-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/options/replace-fwd.hpp>                    // IWYU pragma: export
#include <mongocxx/options/rewrap_many_datakey-fwd.hpp>        // IWYU pragma: export
#include <mongocxx/options/server_api-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/options/tls-fwd.hpp>                        // IWYU pragma: export
#include <mongocxx/options/transaction-fwd.hpp>                // IWYU pragma: export
#include <mongocxx/options/update-fwd.hpp>                     // IWYU pragma: export
#include <mongocxx/pipeline-fwd.hpp>                           // IWYU pragma: export
#include <mongocxx/pool-fwd.hpp>                               // IWYU pragma: export
#include <mongocxx/read_concern-fwd.hpp>                       // IWYU pragma: export
#include <mongocxx/read_preference-fwd.hpp>                    // IWYU pragma: export
#include <mongocxx/result/bulk_write-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/result/delete-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/result/gridfs/upload-fwd.hpp>               // IWYU pragma: export
#include <mongocxx/result/insert_many-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/result/insert_one-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/result/replace_one-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/result/rewrap_many_datakey-fwd.hpp>         // IWYU pragma: export
#include <mongocxx/result/update-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/search_index_model-fwd.hpp>                 // IWYU pragma: export
#include <mongocxx/search_index_view-fwd.hpp>                  // IWYU pragma: export
#include <mongocxx/uri-fwd.hpp>                                // IWYU pragma: export
#include <mongocxx/validation_criteria-fwd.hpp>                // IWYU pragma: export
#include <mongocxx/write_concern-fwd.hpp>                      // IWYU pragma: export
#include <mongocxx/write_type-fwd.hpp>                         // IWYU pragma: export

///
/// @file
/// Aggregate of all forward headers declaring entities in @ref mongocxx::v_noabi.
///
/// @par Includes
/// - All header files under `mongocxx/v_noabi/mongocxx` whose filename ends with `-fwd.hpp`.
///
