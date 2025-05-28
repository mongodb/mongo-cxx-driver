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

#include <bsoncxx/v1/detail/macros.hpp>

#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <mongocxx/private/export.hh>
#include <mongocxx/private/mock.hh>

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wconversion"));
BSONCXX_PRIVATE_IF_MSVC(BSONCXX_PRIVATE_PRAGMA(warning(push, 1));)
#include <mongoc/mongoc.h>
BSONCXX_PRIVATE_WARNINGS_POP();

// See https://jira.mongodb.com/browse/CXX-1453 and
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81605 The basic issue
// is that GCC sees the visibility attributes on the mongoc functions,
// and considers them part of the type, and then emits a silly
// diagnostic stating that the attribute was ignored.
#if defined(__GNUC__) && (__GNUC__ >= 6) && !defined(__clang__)
#define SILENCE_IGNORED_ATTRIBUTES_BEGIN() \
    BSONCXX_PRIVATE_WARNINGS_PUSH();       \
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GCC("-Wignored-attributes"))
#define SILENCE_IGNORED_ATTRIBUTES_END() BSONCXX_PRIVATE_WARNINGS_POP()
#else
#define SILENCE_IGNORED_ATTRIBUTES_BEGIN()
#define SILENCE_IGNORED_ATTRIBUTES_END()
#endif

#if defined(MONGOC_ENABLE_SSL)
#define MONGOC_SYMBOLS_SSL_XMACRO(X) \
    X(client_pool_set_ssl_opts)      \
    X(client_set_ssl_opts)
#else
#define MONGOC_SYMBOLS_SSL_XMACRO(X)
#endif

#pragma push_macro("MONGOC_SYMBOLS_XMACRO")
#define MONGOC_SYMBOLS_XMACRO(X)                                          \
    MONGOC_SYMBOLS_SSL_XMACRO(X)                                          \
    X(apm_callbacks_destroy)                                              \
    X(apm_callbacks_new)                                                  \
    X(apm_command_failed_get_command_name)                                \
    X(apm_command_failed_get_context)                                     \
    X(apm_command_failed_get_duration)                                    \
    X(apm_command_failed_get_error)                                       \
    X(apm_command_failed_get_host)                                        \
    X(apm_command_failed_get_operation_id)                                \
    X(apm_command_failed_get_reply)                                       \
    X(apm_command_failed_get_request_id)                                  \
    X(apm_command_failed_get_server_id)                                   \
    X(apm_command_failed_get_service_id)                                  \
    X(apm_command_started_get_command_name)                               \
    X(apm_command_started_get_command)                                    \
    X(apm_command_started_get_context)                                    \
    X(apm_command_started_get_database_name)                              \
    X(apm_command_started_get_host)                                       \
    X(apm_command_started_get_operation_id)                               \
    X(apm_command_started_get_request_id)                                 \
    X(apm_command_started_get_server_id)                                  \
    X(apm_command_started_get_service_id)                                 \
    X(apm_command_succeeded_get_command_name)                             \
    X(apm_command_succeeded_get_context)                                  \
    X(apm_command_succeeded_get_duration)                                 \
    X(apm_command_succeeded_get_host)                                     \
    X(apm_command_succeeded_get_operation_id)                             \
    X(apm_command_succeeded_get_reply)                                    \
    X(apm_command_succeeded_get_request_id)                               \
    X(apm_command_succeeded_get_server_id)                                \
    X(apm_command_succeeded_get_service_id)                               \
    X(apm_server_changed_get_context)                                     \
    X(apm_server_changed_get_host)                                        \
    X(apm_server_changed_get_new_description)                             \
    X(apm_server_changed_get_previous_description)                        \
    X(apm_server_changed_get_topology_id)                                 \
    X(apm_server_closed_get_context)                                      \
    X(apm_server_closed_get_host)                                         \
    X(apm_server_closed_get_topology_id)                                  \
    X(apm_server_heartbeat_failed_get_awaited)                            \
    X(apm_server_heartbeat_failed_get_context)                            \
    X(apm_server_heartbeat_failed_get_duration)                           \
    X(apm_server_heartbeat_failed_get_error)                              \
    X(apm_server_heartbeat_failed_get_host)                               \
    X(apm_server_heartbeat_started_get_awaited)                           \
    X(apm_server_heartbeat_started_get_context)                           \
    X(apm_server_heartbeat_started_get_host)                              \
    X(apm_server_heartbeat_succeeded_get_awaited)                         \
    X(apm_server_heartbeat_succeeded_get_context)                         \
    X(apm_server_heartbeat_succeeded_get_duration)                        \
    X(apm_server_heartbeat_succeeded_get_host)                            \
    X(apm_server_heartbeat_succeeded_get_reply)                           \
    X(apm_server_opening_get_context)                                     \
    X(apm_server_opening_get_host)                                        \
    X(apm_server_opening_get_topology_id)                                 \
    X(apm_set_command_failed_cb)                                          \
    X(apm_set_command_started_cb)                                         \
    X(apm_set_command_succeeded_cb)                                       \
    X(apm_set_server_changed_cb)                                          \
    X(apm_set_server_closed_cb)                                           \
    X(apm_set_server_heartbeat_failed_cb)                                 \
    X(apm_set_server_heartbeat_started_cb)                                \
    X(apm_set_server_heartbeat_succeeded_cb)                              \
    X(apm_set_server_opening_cb)                                          \
    X(apm_set_topology_changed_cb)                                        \
    X(apm_set_topology_closed_cb)                                         \
    X(apm_set_topology_opening_cb)                                        \
    X(apm_topology_changed_get_context)                                   \
    X(apm_topology_changed_get_new_description)                           \
    X(apm_topology_changed_get_previous_description)                      \
    X(apm_topology_changed_get_topology_id)                               \
    X(apm_topology_closed_get_context)                                    \
    X(apm_topology_closed_get_topology_id)                                \
    X(apm_topology_opening_get_context)                                   \
    X(apm_topology_opening_get_topology_id)                               \
    X(auto_encryption_opts_destroy)                                       \
    X(auto_encryption_opts_new)                                           \
    X(auto_encryption_opts_set_bypass_auto_encryption)                    \
    X(auto_encryption_opts_set_bypass_query_analysis)                     \
    X(auto_encryption_opts_set_encrypted_fields_map)                      \
    X(auto_encryption_opts_set_extra)                                     \
    X(auto_encryption_opts_set_keyvault_client_pool)                      \
    X(auto_encryption_opts_set_keyvault_client)                           \
    X(auto_encryption_opts_set_keyvault_namespace)                        \
    X(auto_encryption_opts_set_kms_providers)                             \
    X(auto_encryption_opts_set_schema_map)                                \
    X(auto_encryption_opts_set_tls_opts)                                  \
    X(bulk_operation_destroy)                                             \
    X(bulk_operation_execute)                                             \
    X(bulk_operation_get_write_concern)                                   \
    X(bulk_operation_insert_with_opts)                                    \
    X(bulk_operation_new)                                                 \
    X(bulk_operation_remove_many_with_opts)                               \
    X(bulk_operation_remove_one_with_opts)                                \
    X(bulk_operation_replace_one_with_opts)                               \
    X(bulk_operation_set_bypass_document_validation)                      \
    X(bulk_operation_set_client_session)                                  \
    X(bulk_operation_set_client)                                          \
    X(bulk_operation_set_collection)                                      \
    X(bulk_operation_set_database)                                        \
    X(bulk_operation_set_write_concern)                                   \
    X(bulk_operation_update_many_with_opts)                               \
    X(bulk_operation_update_one_with_opts)                                \
    X(change_stream_destroy)                                              \
    X(change_stream_error_document)                                       \
    X(change_stream_get_resume_token)                                     \
    X(change_stream_next)                                                 \
    X(cleanup)                                                            \
    X(client_command_simple_with_server_id)                               \
    X(client_destroy)                                                     \
    X(client_enable_auto_encryption)                                      \
    X(client_encryption_add_key_alt_name)                                 \
    X(client_encryption_create_datakey)                                   \
    X(client_encryption_create_encrypted_collection)                      \
    X(client_encryption_datakey_opts_destroy)                             \
    X(client_encryption_datakey_opts_new)                                 \
    X(client_encryption_datakey_opts_set_keyaltnames)                     \
    X(client_encryption_datakey_opts_set_keymaterial)                     \
    X(client_encryption_datakey_opts_set_masterkey)                       \
    X(client_encryption_decrypt)                                          \
    X(client_encryption_delete_key)                                       \
    X(client_encryption_destroy)                                          \
    X(client_encryption_encrypt_expression)                               \
    X(client_encryption_encrypt_opts_destroy)                             \
    X(client_encryption_encrypt_opts_new)                                 \
    X(client_encryption_encrypt_opts_set_algorithm)                       \
    X(client_encryption_encrypt_opts_set_contention_factor)               \
    X(client_encryption_encrypt_opts_set_keyaltname)                      \
    X(client_encryption_encrypt_opts_set_keyid)                           \
    X(client_encryption_encrypt_opts_set_query_type)                      \
    X(client_encryption_encrypt_opts_set_range_opts)                      \
    X(client_encryption_encrypt_range_opts_destroy)                       \
    X(client_encryption_encrypt_range_opts_new)                           \
    X(client_encryption_encrypt_range_opts_set_max)                       \
    X(client_encryption_encrypt_range_opts_set_min)                       \
    X(client_encryption_encrypt_range_opts_set_precision)                 \
    X(client_encryption_encrypt_range_opts_set_sparsity)                  \
    X(client_encryption_encrypt_range_opts_set_trim_factor)               \
    X(client_encryption_encrypt)                                          \
    X(client_encryption_get_key_by_alt_name)                              \
    X(client_encryption_get_key)                                          \
    X(client_encryption_get_keys)                                         \
    X(client_encryption_new)                                              \
    X(client_encryption_opts_destroy)                                     \
    X(client_encryption_opts_new)                                         \
    X(client_encryption_opts_set_keyvault_client)                         \
    X(client_encryption_opts_set_keyvault_namespace)                      \
    X(client_encryption_opts_set_kms_providers)                           \
    X(client_encryption_opts_set_tls_opts)                                \
    X(client_encryption_remove_key_alt_name)                              \
    X(client_encryption_rewrap_many_datakey_result_destroy)               \
    X(client_encryption_rewrap_many_datakey_result_get_bulk_write_result) \
    X(client_encryption_rewrap_many_datakey_result_new)                   \
    X(client_encryption_rewrap_many_datakey)                              \
    X(client_find_databases_with_opts)                                    \
    X(client_get_collection)                                              \
    X(client_get_database_names_with_opts)                                \
    X(client_get_database)                                                \
    X(client_get_read_concern)                                            \
    X(client_get_read_prefs)                                              \
    X(client_get_uri)                                                     \
    X(client_get_write_concern)                                           \
    X(client_new_from_uri)                                                \
    X(client_pool_destroy)                                                \
    X(client_pool_enable_auto_encryption)                                 \
    X(client_pool_new_with_error)                                         \
    X(client_pool_new)                                                    \
    X(client_pool_pop)                                                    \
    X(client_pool_push)                                                   \
    X(client_pool_set_apm_callbacks)                                      \
    X(client_pool_set_server_api)                                         \
    X(client_pool_try_pop)                                                \
    X(client_reset)                                                       \
    X(client_select_server)                                               \
    X(client_session_abort_transaction)                                   \
    X(client_session_advance_cluster_time)                                \
    X(client_session_advance_operation_time)                              \
    X(client_session_append)                                              \
    X(client_session_commit_transaction)                                  \
    X(client_session_destroy)                                             \
    X(client_session_get_cluster_time)                                    \
    X(client_session_get_dirty)                                           \
    X(client_session_get_lsid)                                            \
    X(client_session_get_operation_time)                                  \
    X(client_session_get_opts)                                            \
    X(client_session_get_server_id)                                       \
    X(client_session_get_transaction_state)                               \
    X(client_session_in_transaction)                                      \
    X(client_session_start_transaction)                                   \
    X(client_session_with_transaction)                                    \
    X(client_set_apm_callbacks)                                           \
    X(client_set_read_concern)                                            \
    X(client_set_read_prefs)                                              \
    X(client_set_server_api)                                              \
    X(client_set_write_concern)                                           \
    X(client_start_session)                                               \
    X(client_watch)                                                       \
    X(collection_aggregate)                                               \
    X(collection_command_simple)                                          \
    X(collection_copy)                                                    \
    X(collection_count_documents)                                         \
    X(collection_create_bulk_operation_with_opts)                         \
    X(collection_destroy)                                                 \
    X(collection_drop_index)                                              \
    X(collection_drop_with_opts)                                          \
    X(collection_drop)                                                    \
    X(collection_estimated_document_count)                                \
    X(collection_find_and_modify_with_opts)                               \
    X(collection_find_indexes_with_opts)                                  \
    X(collection_find_with_opts)                                          \
    X(collection_get_name)                                                \
    X(collection_get_read_concern)                                        \
    X(collection_get_read_prefs)                                          \
    X(collection_get_write_concern)                                       \
    X(collection_keys_to_index_string)                                    \
    X(collection_read_command_with_opts)                                  \
    X(collection_rename_with_opts)                                        \
    X(collection_rename)                                                  \
    X(collection_set_read_concern)                                        \
    X(collection_set_read_prefs)                                          \
    X(collection_set_write_concern)                                       \
    X(collection_watch)                                                   \
    X(collection_write_command_with_opts)                                 \
    X(cursor_destroy)                                                     \
    X(cursor_error_document)                                              \
    X(cursor_error)                                                       \
    X(cursor_new_from_command_reply_with_opts)                            \
    X(cursor_next)                                                        \
    X(cursor_set_max_await_time_ms)                                       \
    X(database_aggregate)                                                 \
    X(database_command_with_opts)                                         \
    X(database_copy)                                                      \
    X(database_create_collection)                                         \
    X(database_destroy)                                                   \
    X(database_drop_with_opts)                                            \
    X(database_drop)                                                      \
    X(database_find_collections_with_opts)                                \
    X(database_get_collection_names_with_opts)                            \
    X(database_get_collection)                                            \
    X(database_get_read_concern)                                          \
    X(database_get_read_prefs)                                            \
    X(database_get_write_concern)                                         \
    X(database_has_collection)                                            \
    X(database_set_read_concern)                                          \
    X(database_set_read_prefs)                                            \
    X(database_set_write_concern)                                         \
    X(database_watch)                                                     \
    X(error_has_label)                                                    \
    X(find_and_modify_opts_append)                                        \
    X(find_and_modify_opts_destroy)                                       \
    X(find_and_modify_opts_new)                                           \
    X(find_and_modify_opts_set_bypass_document_validation)                \
    X(find_and_modify_opts_set_fields)                                    \
    X(find_and_modify_opts_set_flags)                                     \
    X(find_and_modify_opts_set_max_time_ms)                               \
    X(find_and_modify_opts_set_sort)                                      \
    X(find_and_modify_opts_set_update)                                    \
    X(handshake_data_append)                                              \
    X(init)                                                               \
    /* X(log_set_handler) CDRIVER-5678: not __cdecl. */                   \
    X(read_concern_copy)                                                  \
    X(read_concern_destroy)                                               \
    X(read_concern_get_level)                                             \
    X(read_concern_new)                                                   \
    X(read_concern_set_level)                                             \
    X(read_prefs_copy)                                                    \
    X(read_prefs_destroy)                                                 \
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN                           \
    X(read_prefs_get_hedge) /* CXX-3241 */                                \
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END                             \
    X(read_prefs_get_max_staleness_seconds)                               \
    X(read_prefs_get_mode)                                                \
    X(read_prefs_get_tags)                                                \
    X(read_prefs_new)                                                     \
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN                           \
    X(read_prefs_set_hedge) /* CXX-3241 */                                \
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END                             \
    X(read_prefs_set_max_staleness_seconds)                               \
    X(read_prefs_set_mode)                                                \
    X(read_prefs_set_tags)                                                \
    X(server_api_copy)                                                    \
    X(server_api_deprecation_errors)                                      \
    X(server_api_destroy)                                                 \
    X(server_api_get_deprecation_errors)                                  \
    X(server_api_get_strict)                                              \
    X(server_api_get_version)                                             \
    X(server_api_new)                                                     \
    X(server_api_strict)                                                  \
    X(server_api_version_from_string)                                     \
    X(server_api_version_to_string)                                       \
    X(server_description_hello_response)                                  \
    X(server_description_host)                                            \
    X(server_description_id)                                              \
    X(server_description_round_trip_time)                                 \
    X(server_description_type)                                            \
    X(server_descriptions_destroy_all)                                    \
    X(session_opts_destroy)                                               \
    X(session_opts_get_causal_consistency)                                \
    X(session_opts_get_snapshot)                                          \
    X(session_opts_new)                                                   \
    X(session_opts_set_causal_consistency)                                \
    X(session_opts_set_default_transaction_opts)                          \
    X(session_opts_set_snapshot)                                          \
    X(topology_description_get_servers)                                   \
    X(topology_description_has_readable_server)                           \
    X(topology_description_has_writable_server)                           \
    X(topology_description_type)                                          \
    X(transaction_opts_clone)                                             \
    X(transaction_opts_destroy)                                           \
    X(transaction_opts_get_max_commit_time_ms)                            \
    X(transaction_opts_get_read_concern)                                  \
    X(transaction_opts_get_read_prefs)                                    \
    X(transaction_opts_get_write_concern)                                 \
    X(transaction_opts_new)                                               \
    X(transaction_opts_set_max_commit_time_ms)                            \
    X(transaction_opts_set_read_concern)                                  \
    X(transaction_opts_set_read_prefs)                                    \
    X(transaction_opts_set_write_concern)                                 \
    X(uri_copy)                                                           \
    X(uri_destroy)                                                        \
    X(uri_get_auth_mechanism)                                             \
    X(uri_get_auth_source)                                                \
    X(uri_get_compressors)                                                \
    X(uri_get_credentials)                                                \
    X(uri_get_database)                                                   \
    X(uri_get_hosts)                                                      \
    X(uri_get_option_as_utf8)                                             \
    X(uri_get_options)                                                    \
    X(uri_get_password)                                                   \
    X(uri_get_read_concern)                                               \
    X(uri_get_read_prefs_t)                                               \
    X(uri_get_replica_set)                                                \
    X(uri_get_string)                                                     \
    X(uri_get_tls)                                                        \
    X(uri_get_username)                                                   \
    X(uri_get_write_concern)                                              \
    X(uri_new_with_error)                                                 \
    X(write_concern_copy)                                                 \
    X(write_concern_destroy)                                              \
    X(write_concern_get_journal)                                          \
    X(write_concern_get_w)                                                \
    X(write_concern_get_wmajority)                                        \
    X(write_concern_get_wtag)                                             \
    X(write_concern_get_wtimeout)                                         \
    X(write_concern_is_acknowledged)                                      \
    X(write_concern_journal_is_set)                                       \
    X(write_concern_new)                                                  \
    X(write_concern_set_journal)                                          \
    X(write_concern_set_w)                                                \
    X(write_concern_set_wmajority)                                        \
    X(write_concern_set_wtag)                                             \
    X(write_concern_set_wtimeout)

namespace mongocxx {
namespace libmongoc {

#if defined(MONGOCXX_TESTING)

SILENCE_IGNORED_ATTRIBUTES_BEGIN();

#pragma push_macro("X")
#undef X
#define X(name) extern MONGOCXX_ABI_EXPORT_TESTING mongocxx::test_util::mock<decltype(&mongoc_##name)>& name;
MONGOC_SYMBOLS_XMACRO(X)
#pragma pop_macro("X")

// CDRIVER-5678
using log_func_cdecl_t = void(
    MONGOCXX_ABI_CDECL*)(mongoc_log_level_t log_level, char const* log_domain, char const* message, void* user_data);
using log_set_handler_cdecl_t = void(MONGOCXX_ABI_CDECL*)(log_func_cdecl_t log_func, void* user_data);

extern MONGOCXX_ABI_EXPORT_TESTING mongocxx::test_util::mock<log_set_handler_cdecl_t>& log_set_handler;

SILENCE_IGNORED_ATTRIBUTES_END();

#else // defined(MONGOCXX_TESTING) ^|v !defined(MONGOCXX_TESTING)

#pragma push_macro("X")
#undef X
#define X(name) constexpr auto name = mongoc_##name;
MONGOC_SYMBOLS_XMACRO(X)
#pragma pop_macro("X")

// CDRIVER-5678
constexpr auto log_set_handler = mongoc_log_set_handler;

#endif // !defined(MONGOCXX_TESTING)

} // namespace libmongoc
} // namespace mongocxx
