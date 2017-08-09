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

///
/// Enum representing the various errors types that can be returned from the server. As this list
/// changes over time, this is just a placeholder for an Int32 error code value from the server.
///
/// @see https://github.com/mongodb/mongo/blob/master/src/mongo/base/error_codes.err
///
enum class server_error_code : std::int32_t {
    k_ok = 0,
    k_internal_error = 1,
    k_bad_value = 2,
    k_OBSOLETE_duplicate_key = 3,
    k_no_such_key = 4,
    k_graph_contains_cycle = 5,
    k_host_unreachable = 6,
    k_host_not_found = 7,
    k_unknown_error = 8,
    k_failed_to_parse = 9,
    k_cannot_mutate_object = 10,
    k_user_not_found = 11,
    k_unsupported_format = 12,
    k_unauthorized = 13,
    k_type_mismatch = 14,
    k_overflow = 15,
    k_invalid_length = 16,
    k_protocol_error = 17,
    k_authentication_failed = 18,
    k_cannot_reuse_object = 19,
    k_illegal_operation = 20,
    k_empty_array_operation = 21,
    k_invalid_bson = 22,
    k_already_initialized = 23,
    k_lock_timeout = 24,
    k_remote_validation_error = 25,
    k_namespace_not_found = 26,
    k_index_not_found = 27,
    k_path_not_viable = 28,
    k_non_existent_path = 29,
    k_invalid_path = 30,
    k_role_not_found = 31,
    k_roles_not_related = 32,
    k_privilege_not_found = 33,
    k_cannot_backfill_array = 34,
    k_user_modification_failed = 35,
    k_remote_change_detected = 36,
    k_file_rename_failed = 37,
    k_file_not_open = 38,
    k_file_stream_failed = 39,
    k_conflicting_update_operators = 40,
    k_file_already_open = 41,
    k_log_write_failed = 42,
    k_cursor_not_found = 43,
    k_user_data_inconsistent = 45,
    k_lock_busy = 46,
    k_no_matching_document = 47,
    k_namespace_exists = 48,
    k_invalid_role_modification = 49,
    k_exceeded_time_limit = 50,
    k_manual_intervention_required = 51,
    k_dollar_prefixed_field_name = 52,
    k_invalid_id_field = 53,
    k_not_single_value_field = 54,
    k_invalid_d_b_ref = 55,
    k_empty_field_name = 56,
    k_dotted_field_name = 57,
    k_role_modification_failed = 58,
    k_command_not_found = 59,
    k_OBSOLETE_database_not_found = 60,
    k_shard_key_not_found = 61,
    k_oplog_operation_unsupported = 62,
    k_stale_shard_version = 63,
    k_write_concern_failed = 64,
    k_multiple_errors_occurred = 65,
    k_immutable_field = 66,
    k_cannot_create_index = 67,
    k_index_already_exists = 68,
    k_auth_schema_incompatible = 69,
    k_shard_not_found = 70,
    k_replica_set_not_found = 71,
    k_invalid_options = 72,
    k_invalid_namespace = 73,
    k_node_not_found = 74,
    k_write_concern_legacy_o_k = 75,
    k_no_replication_enabled = 76,
    k_operation_incomplete = 77,
    k_command_result_schema_violation = 78,
    k_unknown_repl_write_concern = 79,
    k_role_data_inconsistent = 80,
    k_no_match_parse_context = 81,
    k_no_progress_made = 82,
    k_remote_results_unavailable = 83,
    k_duplicate_key_value = 84,
    k_index_options_conflict = 85,
    k_index_key_specs_conflict = 86,
    k_cannot_split = 87,
    k_split_failed_OBSOLETE = 88,
    k_network_timeout = 89,
    k_callback_canceled = 90,
    k_shutdown_in_progress = 91,
    k_secondary_ahead_of_primary = 92,
    k_invalid_replica_set_config = 93,
    k_not_yet_initialized = 94,
    k_not_secondary = 95,
    k_operation_failed = 96,
    k_no_projection_found = 97,
    k_db_path_in_use = 98,
    k_cannot_satisfy_write_concern = 100,
    k_outdated_client = 101,
    k_incompatible_audit_metadata = 102,
    k_new_replica_set_configuration_incompatible = 103,
    k_node_not_electable = 104,
    k_incompatible_sharding_metadata = 105,
    k_distributed_clock_skewed = 106,
    k_lock_failed = 107,
    k_inconsistent_replica_set_names = 108,
    k_configuration_in_progress = 109,
    k_cannot_initialize_node_with_data = 110,
    k_not_exact_value_field = 111,
    k_write_conflict = 112,
    k_initial_sync_failure = 113,
    k_initial_sync_oplog_source_missing = 114,
    k_command_not_supported = 115,
    k_doc_too_large_for_capped = 116,
    k_conflicting_operation_in_progress = 117,
    k_namespace_not_sharded = 118,
    k_invalid_sync_source = 119,
    k_oplog_start_missing = 120,
    k_document_validation_failure = 121,
    k_OBSOLETE_read_after_optime_timeout = 122,
    k_not_a_replica_set = 123,
    k_incompatible_election_protocol = 124,
    k_command_failed = 125,
    k_rpc_protocol_negotiation_failed = 126,
    k_unrecoverable_rollback_error = 127,
    k_lock_not_found = 128,
    k_lock_state_change_failed = 129,
    k_symbol_not_found = 130,
    k_rlp_initialization_failed = 131,
    k_OBSOLETE_config_servers_inconsistent = 132,
    k_failed_to_satisfy_read_preference = 133,
    k_read_concern_majority_not_available_yet = 134,
    k_stale_term = 135,
    k_capped_position_lost = 136,
    k_incompatible_sharding_config_version = 137,
    k_remote_oplog_stale = 138,
    k_js_interpreter_failure = 139,
    k_invalid_s_s_l_configuration = 140,
    k_ssl_handshake_failed = 141,
    k_js_uncatchable_error = 142,
    k_cursor_in_use = 143,
    k_incompatible_catalog_manager = 144,
    k_pooled_connections_dropped = 145,
    k_exceeded_memory_limit = 146,
    k_z_lib_error = 147,
    k_read_concern_majority_not_enabled = 148,
    k_no_config_master = 149,
    k_stale_epoch = 150,
    k_operation_cannot_be_batched = 151,
    k_oplog_out_of_order = 152,
    k_chunk_too_big = 153,
    k_inconsistent_shard_identity = 154,
    k_cannot_apply_oplog_while_primary = 155,
    k_needs_document_move = 156,
    k_can_repair_to_downgrade = 157,
    k_must_upgrade = 158,
    k_duration_overflow = 159,
    k_max_staleness_out_of_range = 160,
    k_incompatible_collation_version = 161,
    k_collection_is_empty = 162,
    k_zone_still_in_use = 163,
    k_initial_sync_active = 164,
    k_view_depth_limit_exceeded = 165,
    k_command_not_supported_on_view = 166,
    k_option_not_supported_on_view = 167,
    k_invalid_pipeline_operator = 168,
    k_command_on_sharded_view_not_supported_on_mongod = 169,
    k_too_many_matching_documents = 170,
    k_cannot_index_parallel_arrays = 171,
    k_transport_session_closed = 172,
    k_transport_session_not_found = 173,
    k_transport_session_unknown = 174,
    k_query_plan_killed = 175,
    k_file_open_failed = 176,
    k_zone_not_found = 177,
    k_range_overlap_conflict = 178,
    k_windows_pdh_error = 179,
    k_bad_perf_counter_path = 180,
    k_ambiguous_index_key_pattern = 181,
    k_invalid_view_definition = 182,
    k_client_metadata_missing_field = 183,
    k_client_metadata_app_name_too_large = 184,
    k_client_metadata_document_too_large = 185,
    k_client_metadata_cannot_be_mutated = 186,
    k_linearizable_read_concern_error = 187,
    k_incompatible_server_version = 188,
    k_primary_stepped_down = 189,
    k_master_slave_connection_failure = 190,
    k_OBSOLETE_balancer_lost_distributed_lock = 191,
    k_fail_point_enabled = 192,
    k_no_sharding_enabled = 193,
    k_balancer_interrupted = 194,
    k_view_pipeline_max_size_exceeded = 195,
    k_invalid_index_specification_option = 197,
    k_OBSOLETE_received_op_reply_message = 198,
    k_replica_set_monitor_removed = 199,
    k_chunk_range_cleanup_pending = 200,
    k_cannot_build_index_keys = 201,
    k_network_interface_exceeded_time_limit = 202,
    k_sharding_state_not_initialized = 203,
    k_time_proof_mismatch = 204,
    k_cluster_time_fails_rate_limiter = 205,
    k_no_such_session = 206,
    k_invalid_uuid = 207,
    k_too_many_locks = 208,
    k_stale_cluster_time = 209,
    k_cannot_verify_and_sign_logical_time = 210,
    k_key_not_found = 211,
    k_incompatible_rollback_algorithm = 212,
    k_duplicate_session = 213,
    k_authentication_restriction_unmet = 214,
    k_database_drop_pending = 215,
    k_election_in_progress = 216,
    k_incomplete_transaction_history = 217,
    k_update_operation_failed = 218,
    k_ftdc_path_not_set = 219,
    k_ftdc_path_already_set = 220,
    k_index_modified = 221,
    k_close_change_stream = 222,
    k_socket_exception = 9001,
    k_recv_stale_config = 9996,
    k_not_master = 10107,
    k_cannot_grow_document_in_capped_namespace = 10003,
    k_duplicate_key = 11000,
    k_interrupted_at_shutdown = 11600,
    k_interrupted = 11601,
    k_interrupted_due_to_repl_state_change = 11602,
    k_out_of_disk_space = 14031,
    k_key_too_long = 17280,
    k_background_operation_in_progress_for_database = 12586,
    k_background_operation_in_progress_for_namespace = 12587,
    k_not_master_or_secondary = 13436,
    k_not_master_no_slave_ok = 13435,
    k_shard_key_too_big = 13334,
    k_send_stale_config = 13388,
    k_database_differ_case = 13297,
    k_OBSOLETE_prepare_configs_failed = 13104
};

///
/// Get the error_category for mongocxx library exceptions.
///
/// @return The mongocxx error_category
///
MONGOCXX_API const std::error_category& MONGOCXX_CALL server_error_category();

///
/// Translate a mongocxx::server_error_code into a std::error_code.
///
/// @param error A mongocxx::error_code
///
/// @return A std::error_code
///
MONGOCXX_INLINE std::error_code make_error_code(server_error_code error) {
    return {static_cast<int>(error), server_error_category()};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

namespace std {
// Specialize is_error_code_enum so we get simpler std::error_code construction
template <>
struct is_error_code_enum<mongocxx::server_error_code> : public true_type {};
}  // namespace std
