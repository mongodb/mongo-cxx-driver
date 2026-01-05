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

#include <mongocxx/result/insert_many-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/insert_many_result.hpp> // IWYU pragma: export

#include <cstddef>
#include <cstdint>
#include <map>
#include <utility>

#include <mongocxx/collection-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/value.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/types.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// The result of a MongoDB insert many operation.
///
class insert_many {
   public:
    using id_map = std::map<std::size_t, bsoncxx::v_noabi::document::element>;

    ~insert_many() = default;

    insert_many(insert_many&& other) = default;

    insert_many& operator=(insert_many&& other) = default;

    insert_many(insert_many const& other) : _result{other._result}, _inserted_ids{other._inserted_ids} {
        this->sync_id_map();
    }

    insert_many& operator=(insert_many const& other) {
        if (this != &other) {
            _result = other._result;
            _inserted_ids = other._inserted_ids;
            this->sync_id_map();
        }

        return *this;
    }

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() insert_many(v1::insert_many_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::insert_many_result() const;

    ///
    /// @deprecated For internal use only.
    ///
    insert_many(result::bulk_write result, bsoncxx::v_noabi::array::value inserted_ids)
        : _result{std::move(result)}, _inserted_ids{std::move(inserted_ids)} {}

    ///
    /// Returns the bulk write result for this insert many operation.
    ///
    /// @return The raw bulk write result.
    ///
    v_noabi::result::bulk_write const& result() const {
        return _result;
    }

    ///
    /// Gets the number of documents that were inserted during this operation.
    ///
    /// @return The number of documents that were inserted.
    ///
    std::int32_t inserted_count() const {
        return _result.inserted_count();
    }

    ///
    /// Gets the _ids of the inserted documents.
    ///
    /// @note The returned id_map must not be accessed after the result::insert_many object is
    /// destroyed.
    /// @return Map of the index of the operation to the _id of the inserted document.
    ///
    id_map inserted_ids() const {
        return _id_map;
    }

    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(insert_many const& lhs, insert_many const& rhs);

    friend bool operator!=(insert_many const& lhs, insert_many const& rhs) {
        return !(lhs == rhs);
    }

   private:
    v_noabi::result::bulk_write _result;
    bsoncxx::v_noabi::array::value _inserted_ids;
    id_map _id_map;

    void sync_id_map() {
        std::size_t index = 0;
        for (auto const& ele : _inserted_ids) {
            // v_noabi::array::element -> v1::element::view -> v_noabi::document::element
            _id_map.emplace(index++, bsoncxx::v_noabi::to_v1(ele));
        }
    }
};

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::result::insert_many from_v1(v1::insert_many_result v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::insert_many_result to_v1(v_noabi::result::insert_many const& v) {
    return v1::insert_many_result{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::insert_many.
///
/// @par Includes
/// - @ref mongocxx/v1/insert_many_result.hpp
///
