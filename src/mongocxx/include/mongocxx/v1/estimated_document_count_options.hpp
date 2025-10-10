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

#include <mongocxx/v1/estimated_document_count_options-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/read_preference-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <memory>

namespace mongocxx {
namespace v1 {

///
/// Options for an "estimatedDocumentCount" operation.
///
/// Supported fields include:
/// - `comment`
/// - `max_time` ("maxTimeMS")
/// - `read_preference` ("readPreference")
///
/// @see
/// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
/// - [`count` (database command) (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/command/count/)
///
class estimated_document_count_options {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~estimated_document_count_options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() estimated_document_count_options(estimated_document_count_options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(estimated_document_count_options&) operator=(
        estimated_document_count_options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() estimated_document_count_options(estimated_document_count_options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(estimated_document_count_options&) operator=(
        estimated_document_count_options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() estimated_document_count_options();

    ///
    /// Set the "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(estimated_document_count_options&) max_time(std::chrono::milliseconds max_time);

    ///
    /// Return the current "maxTimeMS" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

    ///
    /// Set the "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(estimated_document_count_options&) comment(bsoncxx::v1::types::value comment);

    ///
    /// Return the current "comment" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

    ///
    /// Set the "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(estimated_document_count_options&) read_preference(v1::read_preference rp);

    ///
    /// Return the current "readPreference" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::read_preference>) read_preference() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::estimated_document_count_options.
///
