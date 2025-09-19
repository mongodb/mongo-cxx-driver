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

#include <mongocxx/v1/bulk_write-fwd.hpp>

namespace mongocxx {
namespace v1 {

///
/// A list of bulk write operations.
///
/// @see
/// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
/// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class bulk_write {
   public:
    ///
    /// Enumeration identifying the type of a bulk write operation.
    ///
    enum class type {};

    ///
    /// A single "Insert One" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class insert_one {};

    ///
    /// A single "Update One" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class update_one {};

    ///
    /// A single "Update Many" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class update_many {};

    ///
    /// A single "Replace One" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class replace_one {};

    ///
    /// A single "Delete One" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class delete_one {};

    ///
    /// A single "Delete Many" bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class delete_many {};

    ///
    /// A single bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    /// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class single {};

    ///
    /// Options for a bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    /// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class options {};

    ///
    /// The result of a bulk write operation.
    ///
    /// @see
    /// - [Bulk Write Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/bulk-write-operations/)
    /// - [Update Methods (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/update-methods/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class result {};
};

} // namespace v1
} // namespace mongocxx

///
/// @file
/// Provides entities related to bulk write operations.
///
