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

#include <mongocxx/result/delete-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/delete_many_result.hpp> // IWYU pragma: export
#include <mongocxx/v1/delete_one_result.hpp>  // IWYU pragma: export

#include <cstdint>
#include <utility>

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
    ///
    /// @deprecated For internal use only.
    ///
    explicit delete_result(v_noabi::result::bulk_write result) : _result{std::move(result)} {}

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() delete_result(v1::delete_many_result opts);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() delete_result(v1::delete_one_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::delete_many_result() const;

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::delete_one_result() const;

    ///
    /// Returns the bulk write result.
    ///
    /// @return The raw bulk write result.
    ///
    v_noabi::result::bulk_write const& result() const {
        return _result;
    }

    ///
    /// Gets the number of documents that were deleted during this operation.
    ///
    /// @return The number of documents that were deleted.
    ///
    std::int32_t deleted_count() const {
        return _result.deleted_count();
    }

    friend bool operator==(delete_result const& lhs, delete_result const& rhs) {
        return lhs._result == rhs._result;
    }

    friend bool operator!=(delete_result const& lhs, delete_result const& rhs) {
        return !(lhs == rhs);
    }

   private:
    v_noabi::result::bulk_write _result;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::delete_result from_v1(v1::delete_many_result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::delete_result from_v1(v1::delete_one_result v) {
    return {std::move(v)};
}

// Ambiguous whether `v_noabi::result::delete_result` should be converted to `v1::delete_many_result` or
// `v1::delete_one_result`. Require users to explicitly cast to the expected type instead.
//
// v1::delete_many_result to_v1(v_noabi::result::delete_result const& v);
// v1::delete_one_result to_v1(v_noabi::result::delete_result const& v);

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::delete_result.
///
/// @par Includes
/// - @ref mongocxx/v1/delete_many_result.hpp
/// - @ref mongocxx/v1/delete_one_result.hpp
///
