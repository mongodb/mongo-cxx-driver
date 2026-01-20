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

#include <mongocxx/result/rewrap_many_datakey-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/rewrap_many_datakey_result.hpp> // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/bulk_write.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB rewrapManyDatakey operation.
///
class rewrap_many_datakey {
   public:
    ///
    /// @deprecated For internal use only.
    ///
    rewrap_many_datakey() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey(v1::rewrap_many_datakey_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::rewrap_many_datakey_result() const;

    ///
    /// @deprecated For internal use only.
    ///
    explicit rewrap_many_datakey(v_noabi::result::bulk_write bulk_write_result_doc)
        : _result{std::move(bulk_write_result_doc)} {}

    ///
    /// Returns the bulk write result for this rewrap_many_datakey operation.
    ///
    /// @return The raw bulk write result.
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> const& result() {
        return _result;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<v_noabi::result::bulk_write> _result;
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::rewrap_many_datakey from_v1(v1::rewrap_many_datakey_result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::rewrap_many_datakey_result to_v1(v_noabi::result::rewrap_many_datakey const& v) {
    return v1::rewrap_many_datakey_result{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::rewrap_many_datakey.
///
/// @par Includes
/// - @ref mongocxx/v1/rewrap_many_datakey_result.hpp
///
