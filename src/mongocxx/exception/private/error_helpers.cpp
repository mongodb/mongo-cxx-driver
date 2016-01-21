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

#include <mongocxx/exception/private/error_helpers.hpp>

#include <mongocxx/exception/implementation_error.hpp>
#include <mongocxx/exception/private/error_category.hpp>
#include <mongocxx/exception/server_error.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Translate an error code and domain from libmongoc into a std::error_code.
/// Determine whether to use the implementation_error or server_error category.
///
/// @param code A libmongoc error code
/// @param domain A libmongoc error domain
///
/// @return A std::error_code
///
std::error_code make_error_code(int code, int domain) {
    // If the code is not in the server_error enum, use the implementation_error category.
    switch (static_cast<server_error>(code)) {
        case server_error::InternalError:
            break;
        case server_error::BadValue:
            break;
        case server_error::OBSOLETE_DuplicateKey:
            break;
        case server_error::NoSuchKey:
            break;
        case server_error::GraphContainsCycle:
            break;
        case server_error::HostUnreachable:
            break;
        case server_error::HostNotFound:
            break;
        case server_error::UnknownError:
            break;
        case server_error::FailedToParse:
            break;
        case server_error::CannotMutateObject:
            break;
        case server_error::UserNotFound:
            break;
        case server_error::UnsupportedFormat:
            break;
        case server_error::Unauthorized:
            break;
        case server_error::TypeMismatch:
            break;
        case server_error::Overflow:
            break;
        case server_error::InvalidLength:
            break;
        case server_error::ProtocolError:
            break;
        case server_error::AuthenticationFailed:
            break;
        case server_error::CannotReuseObject:
            break;
        case server_error::IllegalOperation:
            break;
        case server_error::EmptyArrayOperation:
            break;
        case server_error::InvalidBSON:
            break;
        case server_error::AlreadyInitialized:
            break;
        case server_error::LockTimeout:
            break;
        case server_error::RemoteValidationError:
            break;
        case server_error::NamespaceNotFound:
            break;
        case server_error::IndexNotFound:
            break;
        case server_error::PathNotViable:
            break;
        case server_error::NonExistentPath:
            break;
        case server_error::InvalidPath:
            break;
        case server_error::RoleNotFound:
            break;
        case server_error::RolesNotRelated:
            break;
        case server_error::PrivilegeNotFound:
            break;
        case server_error::CannotBackfillArray:
            break;
        case server_error::UserModificationFailed:
            break;
        case server_error::RemoteChangeDetected:
            break;
        case server_error::FileRenameFailed:
            break;
        case server_error::FileNotOpen:
            break;
        case server_error::FileStreamFailed:
            break;
        case server_error::ConflictingUpdateOperators:
            break;
        case server_error::FileAlreadyOpen:
            break;
        case server_error::LogWriteFailed:
            break;
        case server_error::CursorNotFound:
            break;
        case server_error::UserDataInconsistent:
            break;
        case server_error::LockBusy:
            break;
        case server_error::NoMatchingDocument:
            break;
        case server_error::NamespaceExists:
            break;
        case server_error::InvalidRoleModification:
            break;
        case server_error::ExceededTimeLimit:
            break;
        case server_error::ManualInterventionRequired:
            break;
        case server_error::DollarPrefixedFieldName:
            break;
        case server_error::InvalidIdField:
            break;
        case server_error::NotSingleValueField:
            break;
        case server_error::InvalidDBRef:
            break;
        case server_error::EmptyFieldName:
            break;
        case server_error::DottedFieldName:
            break;
        case server_error::RoleModificationFailed:
            break;
        case server_error::CommandNotFound:
            break;
        case server_error::OBSOLETE_DatabaseNotFound:
            break;
        case server_error::ShardKeyNotFound:
            break;
        case server_error::OplogOperationUnsupported:
            break;
        case server_error::StaleShardVersion:
            break;
        case server_error::WriteConcernFailed:
            break;
        case server_error::MultipleErrorsOccurred:
            break;
        case server_error::ImmutableField:
            break;
        case server_error::CannotCreateIndex:
            break;
        case server_error::IndexAlreadyExists:
            break;
        case server_error::AuthSchemaIncompatible:
            break;
        case server_error::ShardNotFound:
            break;
        case server_error::ReplicaSetNotFound:
            break;
        case server_error::InvalidOptions:
            break;
        case server_error::InvalidNamespace:
            break;
        case server_error::NodeNotFound:
            break;
        case server_error::WriteConcernLegacyOK:
            break;
        case server_error::NoReplicationEnabled:
            break;
        case server_error::OperationIncomplete:
            break;
        case server_error::CommandResultSchemaViolation:
            break;
        case server_error::UnknownReplWriteConcern:
            break;
        case server_error::RoleDataInconsistent:
            break;
        case server_error::NoMatchParseContext:
            break;
        case server_error::NoProgressMade:
            break;
        case server_error::RemoteResultsUnavailable:
            break;
        case server_error::DuplicateKeyValue:
            break;
        case server_error::IndexOptionsConflict:
            break;
        case server_error::IndexKeySpecsConflict:
            break;
        case server_error::CannotSplit:
            break;
        case server_error::SplitFailed:
            break;
        case server_error::NetworkTimeout:
            break;
        case server_error::CallbackCanceled:
            break;
        case server_error::ShutdownInProgress:
            break;
        case server_error::SecondaryAheadOfPrimary:
            break;
        case server_error::InvalidReplicaSetConfig:
            break;
        case server_error::NotYetInitialized:
            break;
        case server_error::NotSecondary:
            break;
        case server_error::OperationFailed:
            break;
        case server_error::NoProjectionFound:
            break;
        case server_error::DBPathInUse:
            break;
        case server_error::WriteConcernNotDefined:
            break;
        case server_error::CannotSatisfyWriteConcern:
            break;
        case server_error::OutdatedClient:
            break;
        case server_error::IncompatibleAuditMetadata:
            break;
        case server_error::NewReplicaSetConfigurationIncompatible:
            break;
        case server_error::IncompatibleShardingMetadata:
            break;
        case server_error::DistributedClockSkewed:
            break;
        case server_error::LockFailed:
            break;
        case server_error::InconsistentReplicaSetNames:
            break;
        case server_error::ConfigurationInProgress:
            break;
        case server_error::CannotInitializeNodeWithData:
            break;
        case server_error::NotExactValueField:
            break;
        case server_error::WriteConflict:
            break;
        case server_error::InitialSyncFailure:
            break;
        case server_error::InitialSyncOplogSourceMissing:
            break;
        case server_error::CommandNotSupported:
            break;
        case server_error::DocTooLargeForCapped:
            break;
        case server_error::ConflictingOperationInProgress:
            break;
        case server_error::NamespaceNotSharded:
            break;
        case server_error::InvalidSyncSource:
            break;
        case server_error::OplogStartMissing:
            break;
        case server_error::DocumentValidationFailure:
            break;
        case server_error::OBSOLETE_ReadAfterOptimeTimeout:
            break;
        case server_error::NotAReplicaSet:
            break;
        case server_error::IncompatibleElectionProtocol:
            break;
        case server_error::CommandFailed:
            break;
        case server_error::RPCProtocolNegotiationFailed:
            break;
        case server_error::UnrecoverableRollbackError:
            break;
        case server_error::LockNotFound:
            break;
        case server_error::LockStateChangeFailed:
            break;
        case server_error::SymbolNotFound:
            break;
        case server_error::RLPInitializationFailed:
            break;
        case server_error::ConfigServersInconsistent:
            break;
        case server_error::FailedToSatisfyReadPreference:
            break;
        case server_error::ReadConcernMajorityNotAvailableYet:
            break;
        case server_error::StaleTerm:
            break;
        case server_error::CappedPositionLost:
            break;
        case server_error::IncompatibleShardingConfigVersion:
            break;
        case server_error::RemoteOplogStale:
            break;
        case server_error::JSInterpreterFailure:
            break;
        case server_error::InvalidSSLConfiguration:
            break;
        case server_error::SSLHandshakeFailed:
            break;
        case server_error::JSUncatchableError:
            break;
        case server_error::CursorInUse:
            break;
        case server_error::IncompatibleCatalogManager:
            break;
        case server_error::PooledConnectionsDropped:
            break;
        case server_error::ExceededMemoryLimit:
            break;
        case server_error::ZLibError:
            break;
        case server_error::ReadConcernMajorityNotEnabled:
            break;
        case server_error::NoConfigMaster:
            break;
        case server_error::StaleEpoch:
            break;
        case server_error::OperationCannotBeBatched:
            break;
        case server_error::OplogOutOfOrder:
            break;
        case server_error::RecvStaleConfig:
            break;
            break;
        case server_error::CannotGrowDocumentInCappedNamespace:
            break;
        case server_error::DuplicateKey:
            break;
        case server_error::InterruptedAtShutdown:
            break;
        case server_error::Interrupted:
            break;
        case server_error::InterruptedDueToReplStateChange:
            break;
        case server_error::OutOfDiskSpace:
            break;
        case server_error::KeyTooLong:
            break;
        case server_error::BackgroundOperationInProgressForDatabase:
            break;
        case server_error::BackgroundOperationInProgressForNamespace:
            break;
        case server_error::NotMasterOrSecondary:
            break;
        case server_error::NotMasterNoSlaveOk:
            break;
        case server_error::ShardKeyTooBig:
            break;
        case server_error::SendStaleConfig:
            break;
        case server_error::DatabaseDifferCase:
            break;
        case server_error::PrepareConfigsFailed:
            break;
        default:
            return {code, implementation_error_category()};
    }

    // If the code is not in the implementation_error enum, use the server_error category.
    switch (static_cast<implementation_error>(code)) {
        case implementation_error::MONGOC_ERROR_STREAM_INVALID_TYPE:
            break;
        case implementation_error::MONGOC_ERROR_STREAM_INVALID_STATE:
            break;
        case implementation_error::MONGOC_ERROR_STREAM_NAME_RESOLUTION:
            break;
        case implementation_error::MONGOC_ERROR_STREAM_SOCKET:
            break;
        case implementation_error::MONGOC_ERROR_STREAM_CONNECT:
            break;
        case implementation_error::MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_NOT_READY:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_TOO_BIG:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_TOO_SMALL:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_GETNONCE:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_AUTHENTICATE:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
            break;
        case implementation_error::MONGOC_ERROR_CLIENT_IN_EXHAUST:
            break;
        case implementation_error::MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
            break;
        case implementation_error::MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
            break;
        case implementation_error::MONGOC_ERROR_CURSOR_INVALID_CURSOR:
            break;
        case implementation_error::MONGOC_ERROR_QUERY_FAILURE:
            break;
        case implementation_error::MONGOC_ERROR_BSON_INVALID:
            break;
        case implementation_error::MONGOC_ERROR_MATCHER_INVALID:
            break;
        case implementation_error::MONGOC_ERROR_NAMESPACE_INVALID:
            break;
        case implementation_error::MONGOC_ERROR_NAMESPACE_INVALID_FILTER_TYPE:
            break;
        case implementation_error::MONGOC_ERROR_COMMAND_INVALID_ARG:
            break;
        case implementation_error::MONGOC_ERROR_COLLECTION_INSERT_FAILED:
            break;
        case implementation_error::MONGOC_ERROR_COLLECTION_UPDATE_FAILED:
            break;
        case implementation_error::MONGOC_ERROR_COLLECTION_DELETE_FAILED:
            break;
        case implementation_error::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST:
            break;
        case implementation_error::MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
            break;
        case implementation_error::MONGOC_ERROR_SCRAM_NOT_DONE:
            break;
        case implementation_error::MONGOC_ERROR_SCRAM_PROTOCOL_ERROR:
            break;
        case implementation_error::MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
            break;
        case implementation_error::MONGOC_ERROR_QUERY_NOT_TAILABLE:
            break;
        case implementation_error::MONGOC_ERROR_SERVER_SELECTION_BAD_WIRE_VERSION:
            break;
        case implementation_error::MONGOC_ERROR_SERVER_SELECTION_FAILURE:
            break;
        case implementation_error::MONGOC_ERROR_SERVER_SELECTION_INVALID_ID:
            break;
        case implementation_error::MONGOC_ERROR_GRIDFS_CHUNK_MISSING:
            break;
        case implementation_error::MONGOC_ERROR_WRITE_CONCERN_ERROR:
            break;
        default:
            return {code, server_error_category()};
    }

    // If the code is in both enums, use the server_error category iff the domain is
    // MONGOC_ERROR_COMMAND or MONGOC_ERROR_QUERY.
    std::cout << "Domain" << domain << std::endl;
    switch (domain) {
        case ::MONGOC_ERROR_COMMAND:
            return {code, server_error_category()};
        case ::MONGOC_ERROR_QUERY:
            return {code, server_error_category()};
        default:
            return {code, implementation_error_category()};
    }
}

///
/// Translate a bson_error_t from libmongoc into a std::error_code.
/// Determine whether to use the implementation_error or server_error category.
///
/// @param error A libmongoc bson_error_t
///
/// @return A std::error_code
///
std::error_code make_error_code(::bson_error_t error) {
    return make_error_code(error.code, error.domain);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
