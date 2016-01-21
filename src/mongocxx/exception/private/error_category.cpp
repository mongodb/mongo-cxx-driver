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

#include <mongocxx/exception/implementation_error.hpp>
#include <mongocxx/exception/inherent_error.hpp>
#include <mongocxx/exception/server_error.hpp>

namespace {

using namespace mongocxx;

///
/// An error_category for codes for server errors.
///
class server_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "server error";
    }

    std::string message(int code) const noexcept override {
        switch (static_cast<server_error>(code)) {
            case server_error::InternalError:
                return "InternalError server error";
            case server_error::BadValue:
                return "BadValue server error";
            case server_error::OBSOLETE_DuplicateKey:
                return "OBSOLETE_DuplicateKey server error";
            case server_error::NoSuchKey:
                return "NoSuchKey server error";
            case server_error::GraphContainsCycle:
                return "GraphContainsCycle server error";
            case server_error::HostUnreachable:
                return "HostUnreachable server error";
            case server_error::HostNotFound:
                return "HostNotFound server error";
            case server_error::UnknownError:
                return "UnknownError server error";
            case server_error::FailedToParse:
                return "FailedToParse server error";
            case server_error::CannotMutateObject:
                return "CannotMutateObject server error";
            case server_error::UserNotFound:
                return "UserNotFound server error";
            case server_error::UnsupportedFormat:
                return "UnsupportedFormat server error";
            case server_error::Unauthorized:
                return "Unauthorized server error";
            case server_error::TypeMismatch:
                return "TypeMismatch server error";
            case server_error::Overflow:
                return "Overflow server error";
            case server_error::InvalidLength:
                return "InvalidLength server error";
            case server_error::ProtocolError:
                return "ProtocolError server error";
            case server_error::AuthenticationFailed:
                return "AuthenticationFailed server error";
            case server_error::CannotReuseObject:
                return "CannotReuseObject server error";
            case server_error::IllegalOperation:
                return "IllegalOperation server error";
            case server_error::EmptyArrayOperation:
                return "EmptyArrayOperation server error";
            case server_error::InvalidBSON:
                return "InvalidBSON server error";
            case server_error::AlreadyInitialized:
                return "AlreadyInitialized server error";
            case server_error::LockTimeout:
                return "LockTimeout server error";
            case server_error::RemoteValidationError:
                return "RemoteValidationError server error";
            case server_error::NamespaceNotFound:
                return "NamespaceNotFound server error";
            case server_error::IndexNotFound:
                return "IndexNotFound server error";
            case server_error::PathNotViable:
                return "PathNotViable server error";
            case server_error::NonExistentPath:
                return "NonExistentPath server error";
            case server_error::InvalidPath:
                return "InvalidPath server error";
            case server_error::RoleNotFound:
                return "RoleNotFound server error";
            case server_error::RolesNotRelated:
                return "RolesNotRelated server error";
            case server_error::PrivilegeNotFound:
                return "PrivilegeNotFound server error";
            case server_error::CannotBackfillArray:
                return "CannotBackfillArray server error";
            case server_error::UserModificationFailed:
                return "UserModificationFailed server error";
            case server_error::RemoteChangeDetected:
                return "RemoteChangeDetected server error";
            case server_error::FileRenameFailed:
                return "FileRenameFailed server error";
            case server_error::FileNotOpen:
                return "FileNotOpen server error";
            case server_error::FileStreamFailed:
                return "FileStreamFailed server error";
            case server_error::ConflictingUpdateOperators:
                return "ConflictingUpdateOperators server error";
            case server_error::FileAlreadyOpen:
                return "FileAlreadyOpen server error";
            case server_error::LogWriteFailed:
                return "LogWriteFailed server error";
            case server_error::CursorNotFound:
                return "CursorNotFound server error";
            case server_error::UserDataInconsistent:
                return "UserDataInconsistent server error";
            case server_error::LockBusy:
                return "LockBusy server error";
            case server_error::NoMatchingDocument:
                return "NoMatchingDocument server error";
            case server_error::NamespaceExists:
                return "NamespaceExists server error";
            case server_error::InvalidRoleModification:
                return "InvalidRoleModification server error";
            case server_error::ExceededTimeLimit:
                return "ExceededTimeLimit server error";
            case server_error::ManualInterventionRequired:
                return "ManualInterventionRequired server error";
            case server_error::DollarPrefixedFieldName:
                return "DollarPrefixedFieldName server error";
            case server_error::InvalidIdField:
                return "InvalidIdField server error";
            case server_error::NotSingleValueField:
                return "NotSingleValueField server error";
            case server_error::InvalidDBRef:
                return "InvalidDBRef server error";
            case server_error::EmptyFieldName:
                return "EmptyFieldName server error";
            case server_error::DottedFieldName:
                return "DottedFieldName server error";
            case server_error::RoleModificationFailed:
                return "RoleModificationFailed server error";
            case server_error::CommandNotFound:
                return "CommandNotFound server error";
            case server_error::OBSOLETE_DatabaseNotFound:
                return "OBSOLETE_DatabaseNotFound server error";
            case server_error::ShardKeyNotFound:
                return "ShardKeyNotFound server error";
            case server_error::OplogOperationUnsupported:
                return "OplogOperationUnsupported server error";
            case server_error::StaleShardVersion:
                return "StaleShardVersion server error";
            case server_error::WriteConcernFailed:
                return "WriteConcernFailed server error";
            case server_error::MultipleErrorsOccurred:
                return "MultipleErrorsOccurred server error";
            case server_error::ImmutableField:
                return "ImmutableField server error";
            case server_error::CannotCreateIndex:
                return "CannotCreateIndex server error";
            case server_error::IndexAlreadyExists:
                return "IndexAlreadyExists server error";
            case server_error::AuthSchemaIncompatible:
                return "AuthSchemaIncompatible server error";
            case server_error::ShardNotFound:
                return "ShardNotFound server error";
            case server_error::ReplicaSetNotFound:
                return "ReplicaSetNotFound server error";
            case server_error::InvalidOptions:
                return "InvalidOptions server error";
            case server_error::InvalidNamespace:
                return "InvalidNamespace server error";
            case server_error::NodeNotFound:
                return "NodeNotFound server error";
            case server_error::WriteConcernLegacyOK:
                return "WriteConcernLegacyOK server error";
            case server_error::NoReplicationEnabled:
                return "NoReplicationEnabled server error";
            case server_error::OperationIncomplete:
                return "OperationIncomplete server error";
            case server_error::CommandResultSchemaViolation:
                return "CommandResultSchemaViolation server error";
            case server_error::UnknownReplWriteConcern:
                return "UnknownReplWriteConcern server error";
            case server_error::RoleDataInconsistent:
                return "RoleDataInconsistent server error";
            case server_error::NoMatchParseContext:
                return "NoMatchParseContext server error";
            case server_error::NoProgressMade:
                return "NoProgressMade server error";
            case server_error::RemoteResultsUnavailable:
                return "RemoteResultsUnavailable server error";
            case server_error::DuplicateKeyValue:
                return "DuplicateKeyValue server error";
            case server_error::IndexOptionsConflict:
                return "IndexOptionsConflict server error";
            case server_error::IndexKeySpecsConflict:
                return "IndexKeySpecsConflict server error";
            case server_error::CannotSplit:
                return "CannotSplit server error";
            case server_error::SplitFailed:
                return "SplitFailed server error";
            case server_error::NetworkTimeout:
                return "NetworkTimeout server error";
            case server_error::CallbackCanceled:
                return "CallbackCanceled server error";
            case server_error::ShutdownInProgress:
                return "ShutdownInProgress server error";
            case server_error::SecondaryAheadOfPrimary:
                return "SecondaryAheadOfPrimary server error";
            case server_error::InvalidReplicaSetConfig:
                return "InvalidReplicaSetConfig server error";
            case server_error::NotYetInitialized:
                return "NotYetInitialized server error";
            case server_error::NotSecondary:
                return "NotSecondary server error";
            case server_error::OperationFailed:
                return "OperationFailed server error";
            case server_error::NoProjectionFound:
                return "NoProjectionFound server error";
            case server_error::DBPathInUse:
                return "DBPathInUse server error";
            case server_error::WriteConcernNotDefined:
                return "WriteConcernNotDefined server error";
            case server_error::CannotSatisfyWriteConcern:
                return "CannotSatisfyWriteConcern server error";
            case server_error::OutdatedClient:
                return "OutdatedClient server error";
            case server_error::IncompatibleAuditMetadata:
                return "IncompatibleAuditMetadata server error";
            case server_error::NewReplicaSetConfigurationIncompatible:
                return "NewReplicaSetConfigurationIncompatible server error";
            case server_error::IncompatibleShardingMetadata:
                return "IncompatibleShardingMetadata server error";
            case server_error::DistributedClockSkewed:
                return "DistributedClockSkewed server error";
            case server_error::LockFailed:
                return "LockFailed server error";
            case server_error::InconsistentReplicaSetNames:
                return "InconsistentReplicaSetNames server error";
            case server_error::ConfigurationInProgress:
                return "ConfigurationInProgress server error";
            case server_error::CannotInitializeNodeWithData:
                return "CannotInitializeNodeWithData server error";
            case server_error::NotExactValueField:
                return "NotExactValueField server error";
            case server_error::WriteConflict:
                return "WriteConflict server error";
            case server_error::InitialSyncFailure:
                return "InitialSyncFailure server error";
            case server_error::InitialSyncOplogSourceMissing:
                return "InitialSyncOplogSourceMissing server error";
            case server_error::CommandNotSupported:
                return "CommandNotSupported server error";
            case server_error::DocTooLargeForCapped:
                return "DocTooLargeForCapped server error";
            case server_error::ConflictingOperationInProgress:
                return "ConflictingOperationInProgress server error";
            case server_error::NamespaceNotSharded:
                return "NamespaceNotSharded server error";
            case server_error::InvalidSyncSource:
                return "InvalidSyncSource server error";
            case server_error::OplogStartMissing:
                return "OplogStartMissing server error";
            case server_error::DocumentValidationFailure:
                return "DocumentValidationFailure server error";
            case server_error::OBSOLETE_ReadAfterOptimeTimeout:
                return "OBSOLETE_ReadAfterOptimeTimeout server error";
            case server_error::NotAReplicaSet:
                return "NotAReplicaSet server error";
            case server_error::IncompatibleElectionProtocol:
                return "IncompatibleElectionProtocol server error";
            case server_error::CommandFailed:
                return "CommandFailed server error";
            case server_error::RPCProtocolNegotiationFailed:
                return "RPCProtocolNegotiationFailed server error";
            case server_error::UnrecoverableRollbackError:
                return "UnrecoverableRollbackError server error";
            case server_error::LockNotFound:
                return "LockNotFound server error";
            case server_error::LockStateChangeFailed:
                return "LockStateChangeFailed server error";
            case server_error::SymbolNotFound:
                return "SymbolNotFound server error";
            case server_error::RLPInitializationFailed:
                return "RLPInitializationFailed server error";
            case server_error::ConfigServersInconsistent:
                return "ConfigServersInconsistent server error";
            case server_error::FailedToSatisfyReadPreference:
                return "FailedToSatisfyReadPreference server error";
            case server_error::ReadConcernMajorityNotAvailableYet:
                return "ReadConcernMajorityNotAvailableYet server error";
            case server_error::StaleTerm:
                return "StaleTerm server error";
            case server_error::CappedPositionLost:
                return "CappedPositionLost server error";
            case server_error::IncompatibleShardingConfigVersion:
                return "IncompatibleShardingConfigVersion server error";
            case server_error::RemoteOplogStale:
                return "RemoteOplogStale server error";
            case server_error::JSInterpreterFailure:
                return "JSInterpreterFailure server error";
            case server_error::InvalidSSLConfiguration:
                return "InvalidSSLConfiguration server error";
            case server_error::SSLHandshakeFailed:
                return "SSLHandshakeFailed server error";
            case server_error::JSUncatchableError:
                return "JSUncatchableError server error";
            case server_error::CursorInUse:
                return "CursorInUse server error";
            case server_error::IncompatibleCatalogManager:
                return "IncompatibleCatalogManager server error";
            case server_error::PooledConnectionsDropped:
                return "PooledConnectionsDropped server error";
            case server_error::ExceededMemoryLimit:
                return "ExceededMemoryLimit server error";
            case server_error::ZLibError:
                return "ZLibError server error";
            case server_error::ReadConcernMajorityNotEnabled:
                return "ReadConcernMajorityNotEnabled server error";
            case server_error::NoConfigMaster:
                return "NoConfigMaster server error";
            case server_error::StaleEpoch:
                return "StaleEpoch server error";
            case server_error::OperationCannotBeBatched:
                return "OperationCannotBeBatched server error";
            case server_error::OplogOutOfOrder:
                return "OplogOutOfOrder server error";
            case server_error::RecvStaleConfig:
                return "RecvStaleConfig server error";
            case server_error::NotMaster:
                return "NotMaster server error";
            case server_error::CannotGrowDocumentInCappedNamespace:
                return "CannotGrowDocumentInCappedNamespace server error";
            case server_error::DuplicateKey:
                return "DuplicateKey server error";
            case server_error::InterruptedAtShutdown:
                return "InterruptedAtShutdown server error";
            case server_error::Interrupted:
                return "Interrupted server error";
            case server_error::InterruptedDueToReplStateChange:
                return "InterruptedDueToReplStateChange server error";
            case server_error::OutOfDiskSpace:
                return "OutOfDiskSpace server error";
            case server_error::KeyTooLong:
                return "KeyTooLong server error";
            case server_error::BackgroundOperationInProgressForDatabase:
                return "BackgroundOperationInProgressForDatabase server error";
            case server_error::BackgroundOperationInProgressForNamespace:
                return "BackgroundOperationInProgressForNamespace server error";
            case server_error::NotMasterOrSecondary:
                return "NotMasterOrSecondary server error";
            case server_error::NotMasterNoSlaveOk:
                return "NotMasterNoSlaveOk server error";
            case server_error::ShardKeyTooBig:
                return "ShardKeyTooBig server error";
            case server_error::SendStaleConfig:
                return "SendStaleConfig server error";
            case server_error::DatabaseDifferCase:
                return "DatabaseDifferCase server error";
            case server_error::PrepareConfigsFailed:
                return "PrepareConfigsFailed server error";
            default:
                return "unknown server error";
        }
    }
};

///
/// An error_category for codes for mongoc errors.
///
class implementation_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "implementation error";
    }

    std::string message(int code) const noexcept override {
        switch (static_cast<implementation_error>(code)) {
            case implementation_error::MONGOC_ERROR_STREAM_INVALID_TYPE:
                return "stream invalid type implementation error";
            case implementation_error::MONGOC_ERROR_STREAM_INVALID_STATE:
                return "stream invalid state implementation error";
            case implementation_error::MONGOC_ERROR_STREAM_NAME_RESOLUTION:
                return "stream name resolution implementation error";
            case implementation_error::MONGOC_ERROR_STREAM_SOCKET:
                return "stream socket implementation error";
            case implementation_error::MONGOC_ERROR_STREAM_CONNECT:
                return "stream connect implementation error";
            case implementation_error::MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
                return "stream not established implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_NOT_READY:
                return "client not ready implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_TOO_BIG:
                return "client too big implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_TOO_SMALL:
                return "client too small implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_GETNONCE:
                return "client getnonce implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_AUTHENTICATE:
                return "client authenticate implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_NO_ACCEPTABLE_PEER:
                return "client no acceptable peer implementation error";
            case implementation_error::MONGOC_ERROR_CLIENT_IN_EXHAUST:
                return "client in exhaust implementation error";
            case implementation_error::MONGOC_ERROR_PROTOCOL_INVALID_REPLY:
                return "protocol invalid reply implementation error";
            case implementation_error::MONGOC_ERROR_PROTOCOL_BAD_WIRE_VERSION:
                return "protocol bad wire version implementation error";
            case implementation_error::MONGOC_ERROR_CURSOR_INVALID_CURSOR:
                return "invalid cursor implementation error";
            case implementation_error::MONGOC_ERROR_QUERY_FAILURE:
                return "query failure implementation error";
            case implementation_error::MONGOC_ERROR_BSON_INVALID:
                return "bson invalid implementation error";
            case implementation_error::MONGOC_ERROR_MATCHER_INVALID:
                return "matcher invalid implementation error";
            case implementation_error::MONGOC_ERROR_NAMESPACE_INVALID:
                return "namespace invalid implementation error";
            case implementation_error::MONGOC_ERROR_NAMESPACE_INVALID_FILTER_TYPE:
                return "namespace invalid filter type implementation error";
            case implementation_error::MONGOC_ERROR_COMMAND_INVALID_ARG:
                return "command invalid arg implementation error";
            case implementation_error::MONGOC_ERROR_COLLECTION_INSERT_FAILED:
                return "collection insert failed implementation error";
            case implementation_error::MONGOC_ERROR_COLLECTION_UPDATE_FAILED:
                return "collection update failed implementation error";
            case implementation_error::MONGOC_ERROR_COLLECTION_DELETE_FAILED:
                return "collection delete failed implementation error";
            case implementation_error::MONGOC_ERROR_COLLECTION_DOES_NOT_EXIST:
                return "collection does not exist implementation error";
            case implementation_error::MONGOC_ERROR_GRIDFS_INVALID_FILENAME:
                return "gridfs invalid filename implementation error";
            case implementation_error::MONGOC_ERROR_SCRAM_NOT_DONE:
                return "scram not done implementation error";
            case implementation_error::MONGOC_ERROR_SCRAM_PROTOCOL_ERROR:
                return "scram protocol implementation error";
            case implementation_error::MONGOC_ERROR_QUERY_COMMAND_NOT_FOUND:
                return "query command not found implementation error";
            case implementation_error::MONGOC_ERROR_QUERY_NOT_TAILABLE:
                return "query not tailable implementation error";
            case implementation_error::MONGOC_ERROR_SERVER_SELECTION_BAD_WIRE_VERSION:
                return "server selection bad wire version implementation error";
            case implementation_error::MONGOC_ERROR_SERVER_SELECTION_FAILURE:
                return "server selection failure implementation error";
            case implementation_error::MONGOC_ERROR_SERVER_SELECTION_INVALID_ID:
                return "server selection invalid ID implementation error";
            case implementation_error::MONGOC_ERROR_GRIDFS_CHUNK_MISSING:
                return "gridfs chunk missing implementation error";
            case implementation_error::MONGOC_ERROR_WRITE_CONCERN_ERROR:
                return "write concern implementation error";
            default:
                return "unknown implementation error";
        }
    }
};

///
/// An error_category for codes for inherent mongocxx errors.
///
class inherent_error_category_impl final : public std::error_category {
   public:
    const char* name() const noexcept override {
        return "inherent error";
    }

    std::string message(int code) const noexcept override {
        switch (static_cast<inherent_error>(code)) {
            case inherent_error::k_invalid_client_object:
                return "invalid use of default constructed or moved-from mongocxx::client object";
            case inherent_error::k_invalid_collection_object:
                return "invalid use of default constructed or moved-from mongocxx::collection "
                       "object";
            case inherent_error::k_invalid_database_object:
                return "invalid use of default constructed or moved-from mongocxx::database object";
            case inherent_error::k_invalid_parameter:
                return "an invalid or out-of-bounds parameter was provided";
            case inherent_error::k_ssl_not_supported:
                return "SSL support not available";
            case inherent_error::k_unknown_read_concern:
                return "invalid attempt to set an unknown read concern level";
            case inherent_error::k_unknown_write_concern:
                return "invalid attempt to set an unknown write concern level";
            default:
                return "unknown inherent error";
        }
    }
};
}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Get the error_category for exceptions originating from the server.
///
/// @return The server error_category
///
const std::error_category& server_error_category() {
    static const server_error_category_impl server_category{};
    return server_category;
}

///
/// Get the error_category for exceptions originating from the libmongoc library.
///
/// @return The implementation error_category
///
const std::error_category& implementation_error_category() {
    static const implementation_error_category_impl implementation_category{};
    return implementation_category;
}

///
/// Get the error_category for inherent mongocxx exceptions.
///
/// @return The inherent error_category
///
const std::error_category& inherent_error_category() {
    static const inherent_error_category_impl inherent_category{};
    return inherent_category;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
