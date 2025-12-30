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

#include <mongocxx/result/insert_one-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/insert_one_result.hpp> // IWYU pragma: export

#include <utility>

#include <bsoncxx/array/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types.hpp>       // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/value.hpp>
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
    ~insert_one() = default;

    insert_one(insert_one&& other) noexcept = default;
    insert_one& operator=(insert_one&& other) noexcept = default;

    insert_one(insert_one const& other)
        : _result{other._result}, _inserted_id_owned{other._inserted_id_owned}, _inserted_id{_inserted_id_owned} {}

    insert_one& operator=(insert_one const& other) {
        if (this != &other) {
            _result = other._result;
            _inserted_id_owned = other._inserted_id_owned;
            _inserted_id = _inserted_id_owned;
        }

        return *this;
    }

    ///
    /// @deprecated For internal use only.
    ///
    insert_one(v_noabi::result::bulk_write result, bsoncxx::v_noabi::types::view inserted_id)
        : _result{std::move(result)}, _inserted_id_owned{inserted_id}, _inserted_id{_inserted_id_owned} {}

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() insert_one(v1::insert_one_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::insert_one_result() const;

    ///
    /// Returns the bulk write result for this insert operation.
    ///
    /// @return The raw bulk write result.
    ///
    v_noabi::result::bulk_write const& result() const {
        return _result;
    }

    ///
    /// Gets the _id of the inserted document.
    ///
    /// @return The value of the _id field for the inserted document.
    ///
    bsoncxx::v_noabi::types::bson_value::view const& inserted_id() const {
        return _inserted_id;
    }

    friend bool operator==(insert_one const& lhs, insert_one const& rhs) {
        return lhs._result == rhs._result && lhs._inserted_id == rhs._inserted_id;
    }

    friend bool operator!=(insert_one const& lhs, insert_one const& rhs) {
        return !(lhs == rhs);
    }

   private:
    v_noabi::result::bulk_write _result;
    bsoncxx::v_noabi::types::value _inserted_id_owned;
    bsoncxx::v_noabi::types::view _inserted_id;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::insert_one from_v1(v1::insert_one_result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::insert_one_result to_v1(v_noabi::result::insert_one const& v) {
    return v1::insert_one_result{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::insert_one.
///
/// @par Includes
/// - @ref mongocxx/v1/insert_one_result.hpp
///
