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

#include <cstdint>

#include <mongocxx/result/delete-fwd.hpp>

#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB delete operation.
///
class delete_result {
   public:
    // This constructor is public for testing purposes only
    explicit MONGOCXX_ABI_EXPORT_CDECL() delete_result(result::bulk_write result);

    ///
    /// Returns the bulk write result.
    ///
    /// @return The raw bulk write result.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(result::bulk_write const&) result() const;

    ///
    /// Gets the number of documents that were deleted during this operation.
    ///
    /// @return The number of documents that were deleted.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(std::int32_t) deleted_count() const;

    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(delete_result const&, delete_result const&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(delete_result const&, delete_result const&);

   private:
    result::bulk_write _result;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::delete_result.
///
