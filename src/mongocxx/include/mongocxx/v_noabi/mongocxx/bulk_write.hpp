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

#include <mongocxx/bulk_write-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <utility>

#include <mongocxx/collection-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/client_session.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/model/write.hpp>
#include <mongocxx/options/bulk_write.hpp>
#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A batch of write operations that can be sent to the server as a group.
///
/// If you have a lot of write operations to execute, it can be more efficient to send them as
/// part of a bulk_write in order to avoid unnecessary network-level round trips between the driver
/// and the server.
///
/// Bulk writes affect a single collection only and are executed via the bulk_write::execute()
/// method. Options that you would typically specify for individual write operations (such as write
/// concern) are instead specified for the aggregate operation.
///
/// @see
/// - https://www.mongodb.com/docs/manual/core/crud/
/// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
///
class bulk_write {
   private:
    v1::bulk_write _bulk;

   public:
    ///
    /// Move constructs a bulk write operation.
    ///
    bulk_write(bulk_write&&) noexcept = default;

    ///
    /// Move assigns a bulk write operation.
    ///
    bulk_write& operator=(bulk_write&&) noexcept = default;

    ///
    /// This class is not copyable.
    ///
    bulk_write(bulk_write const& other) = delete;

    ///
    /// This class is not copyable.
    ///
    bulk_write& operator=(bulk_write const& other) = delete;

    ///
    /// Destroys a bulk write operation.
    ///
    ~bulk_write() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ bulk_write(v1::bulk_write bulk) : _bulk{std::move(bulk)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    explicit operator v1::bulk_write() && {
        return std::move(_bulk);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::bulk_write() const& = delete;

    ///
    /// Checks if a bulk write operation is empty.
    ///
    /// @return A boolean indicating if the bulk write operation is empty.
    ///
    bool empty() const noexcept {
        return _bulk.empty();
    }

    ///
    /// Appends a single write to the bulk write operation. The write operation's contents are
    /// copied into the bulk operation completely, so there is no dependency between the life of an
    /// appended write operation and the bulk operation itself.
    ///
    /// @param operation
    ///   The write operation to append (an instance of model::write)
    ///
    ///   A model::write can be implicitly constructed from any of the following MongoDB models:
    ///
    ///     - model::insert_one
    ///     - model::delete_one
    ///     - model::replace_one
    ///     - model::update_many
    ///     - model::update_one
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @throws mongocxx::v_noabi::logic_error if the given operation is invalid.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bulk_write&) append(v_noabi::model::write const& operation);

    ///
    /// Executes a bulk write.
    ///
    /// @throws mongocxx::v_noabi::bulk_write_exception when there are errors processing the writes.
    ///
    /// @return The optional result of the bulk operation execution, a result::bulk_write.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/bulk-write-operations/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<result::bulk_write>) execute() const;

    class internal;
};

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::bulk_write from_v1(v1::bulk_write v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write to_v1(v_noabi::bulk_write v) {
    return v1::bulk_write{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::bulk_write.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
