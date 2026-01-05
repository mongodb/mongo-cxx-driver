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

#include <mongocxx/result/replace_one-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/replace_one_result.hpp> // IWYU pragma: export

#include <cstdint>
#include <utility>

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB replaceOne operation.
///
class replace_one {
   public:
    ///
    /// @deprecated For internal use only.
    ///
    explicit replace_one(v_noabi::result::bulk_write result) : _result{std::move(result)} {}

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() replace_one(v1::replace_one_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::replace_one_result() const;

    ///
    /// Returns the bulk write result for this replace_one operation.
    ///
    /// @return The raw bulk write result.
    ///
    v_noabi::result::bulk_write const& result() const {
        return _result;
    }

    ///
    /// Gets the number of documents that were matched during this operation.
    ///
    /// @return The number of documents that were matched.
    ///
    std::int32_t matched_count() const {
        return _result.matched_count();
    }

    ///
    /// Gets the number of documents that were modified during this operation.
    ///
    /// @return The number of documents that were modified.
    ///
    std::int32_t modified_count() const {
        return _result.modified_count();
    }

    ///
    /// Gets the id of the upserted document.
    ///
    /// @return The value of the _id field for upserted document.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::element> upserted_id() const {
        auto const ids = _result.upserted_ids();
        auto const iter = ids.find(0);

        if (iter != ids.end()) {
            return iter->second;
        }

        return {};
    }

    friend bool operator==(replace_one const& lhs, replace_one const& rhs) {
        return lhs._result == rhs._result;
    }

    friend bool operator!=(replace_one const& lhs, replace_one const& rhs) {
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
inline v_noabi::result::replace_one from_v1(v1::replace_one_result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::replace_one_result to_v1(v_noabi::result::replace_one const& v) {
    return v1::replace_one_result{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::replace_one.
///
/// @par Includes
/// - @ref mongocxx/v1/replace_one_result.hpp
///
