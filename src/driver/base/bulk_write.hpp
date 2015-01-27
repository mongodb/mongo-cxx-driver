// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include "driver/base/write_concern.hpp"
#include "driver/model/write.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace base {

class collection;

///
/// Class representing a batch of write operations that can be sent to the server as a group.
///
/// If you have a lot of write operations to execute, it can be more efficient to send them as
/// part of a bulk_write in order to avoid unnecessary network-level round trips between the driver
/// and the server.
///
/// Bulk writes affect a single collection only and are executed via the collection::bulk_write()
/// method. Options that you would typically specify for individual write operations (such as write
/// concern) are instead specified for the aggregate operation.
///
/// Example usage:
/// @include examples/bulk_write.cpp
///
/// @todo Uncomment and implement when c-driver provides this functionality
///     class write_concern write_concern() const;
///
/// @see http://docs.mongodb.org/manual/core/write-operations/
/// @see http://docs.mongodb.org/manual/core/bulk-write-operations/
///
class LIBMONGOCXX_EXPORT bulk_write {

   public:

    ///
    /// Initializes a new bulk operation to be executed against a mongo::driver::collection.
    ///
    /// @param ordered
    ///   If @c true all write operations will be executed serially in the order they were appended
    ///   and the entire bulk operation will abort on the first error. If @c false operations will
    ///   be executed in an arbitrary order (possibly in parallel on the server) and any errors will
    ///   be reported after attempting all operations. Unordered bulk writes may be more efficient
    ///   than ordered bulk writes.
    ///
    explicit bulk_write(bool ordered);

    ///
    /// Move constructs a bulk write operation.
    ///
    bulk_write(bulk_write&&) noexcept;

    ///
    /// Move assigns a bulk write operation.
    ///
    bulk_write& operator=(bulk_write&&) noexcept;

    ///
    /// Destroys a bulk write operation.
    ///
    ~bulk_write();

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
    void append(const model::write& operation);

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The write_concern to set
    ///
    void write_concern(class write_concern wc);

   private:
    friend class collection;

    class impl;
    std::unique_ptr<impl> _impl;

};

}  // namespace base
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
