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

#include <mongocxx/result/rewrap_many_datakey-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB rewrapManyDatakey operation.
///
class rewrap_many_datakey {
   public:
    rewrap_many_datakey() = default;

    explicit MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey(
        mongocxx::v_noabi::result::bulk_write bulk_write_result_doc);

    ///
    /// Returns the bulk write result for this rewrap_many_datakey operation.
    ///
    /// @return The raw bulk write result.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::result::bulk_write> const&)
    result();

   private:
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::result::bulk_write> _result;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::rewrap_many_datakey.
///
