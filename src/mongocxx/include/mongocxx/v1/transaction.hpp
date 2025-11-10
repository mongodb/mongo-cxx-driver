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

#include <mongocxx/v1/transaction-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/read_concern-fwd.hpp>
#include <mongocxx/v1/read_preference-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>

namespace mongocxx {
namespace v1 {

///
/// Options related to a distributed transaction.
///
/// Supported fields include:
/// - `max_commit_time_ms` ("maxCommitTimeMS")
/// - `read_concern` ("readConcern")
/// - `read_preference` ("readPreference")
/// - `write_concern` ("writeConcern")
///
/// @see
/// - [Transactions (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/transactions/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class transaction {
   private:
    void* _impl; // mongoc_transaction_opt_t

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~transaction();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() transaction(transaction&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) operator=(transaction&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() transaction(transaction const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) operator=(transaction const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() transaction();

    ///
    /// Set the "maxCommitTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) max_commit_time_ms(std::chrono::milliseconds v);

    ///
    /// Return the current "maxCommitTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_commit_time_ms() const;

    ///
    /// Set the "readConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) read_concern(v1::read_concern const& v);

    ///
    /// Return the current "readConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_concern>) read_concern() const;

    ///
    /// Set the "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) read_preference(v1::read_preference const& v);

    ///
    /// Return the current "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_preference>) read_preference() const;

    ///
    /// Set the "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) write_concern(v1::write_concern const& v);

    ///
    /// Return the current "writeConcern" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::transaction.
///
