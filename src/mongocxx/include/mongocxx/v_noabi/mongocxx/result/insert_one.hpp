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

#include <mongocxx/result/insert_one-fwd.hpp>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB insert operation.
///
class insert_one {
   public:
    // This constructor is public for testing purposes only
    MONGOCXX_ABI_EXPORT_CDECL()
    insert_one(result::bulk_write result, bsoncxx::v_noabi::types::bson_value::view inserted_id);

    ///
    /// Returns the bulk write result for this insert operation.
    ///
    /// @return The raw bulk write result.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(result::bulk_write const&) result() const;

    ///
    /// Gets the _id of the inserted document.
    ///
    /// @return The value of the _id field for the inserted document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::types::bson_value::view const&) inserted_id() const;

    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(insert_one const&, insert_one const&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(insert_one const&, insert_one const&);

   private:
    result::bulk_write _result;

    // Array with a single element, containing the value of the _id field for the inserted document.
    bsoncxx::v_noabi::array::value _inserted_id_owned;

    // Points into _inserted_id_owned.
    bsoncxx::v_noabi::types::bson_value::view _inserted_id;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::insert_one.
///
