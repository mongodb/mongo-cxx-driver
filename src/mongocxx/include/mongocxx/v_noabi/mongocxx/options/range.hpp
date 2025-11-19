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

#include <mongocxx/options/range-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/range_options.hpp> // IWYU pragma: export

#include <cstdint>
#include <utility>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/view_or_value.hpp>
#include <bsoncxx/types/view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::encrypt::range_opts.
///
/// Specifies index options (`RangeOpts`) for a Queryable Encryption field supporting "range"
/// queries.
///
/// @note `min`, `max`, `trimFactor`, `sparsity`, and `precision` must match the values set in the
/// encryptedFields of the destination collection.
///
/// @note For double and decimal128, `min`, `max`, and `precision` must all be set, or all be
/// unset.
///
class range {
   public:
    ///
    /// Default initialization.
    ///
    range() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() range(v1::range_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated iterators and views.
    ///
    explicit operator v1::range_options() const {
        v1::range_options ret;

        if (_min) {
            ret.min(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(_min->view())});
        }

        if (_max) {
            ret.max(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(_max->view())});
        }

        if (_sparsity) {
            ret.sparsity(*_sparsity);
        }

        if (_trim_factor) {
            ret.trim_factor(*_trim_factor);
        }

        if (_precision) {
            ret.precision(*_precision);
        }

        return ret;
    }

    /// @brief Sets `RangeOpts.min`.
    /// @note Required if @ref precision is set.
    range& min(bsoncxx::v_noabi::types::bson_value::view_or_value value) {
        _min = value.view();
        return *this;
    }

    /// @brief Gets `RangeOpts.min`.
    /// @note Required if @ref precision is set.
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& min() const {
        return _min;
    }

    /// @brief Sets `RangeOpts.max`.
    /// @note Required if @ref precision is set.
    range& max(bsoncxx::v_noabi::types::bson_value::view_or_value value) {
        _max = value.view();
        return *this;
    }

    /// @brief Gets `RangeOpts.max`.
    /// @note Required if @ref precision is set.
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& max() const {
        return _max;
    }

    /// @brief Sets `RangeOpts.sparsity`.
    range& sparsity(std::int64_t value) {
        _sparsity = value;
        return *this;
    }

    /// @brief Gets `RangeOpts.sparsity`.
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& sparsity() const {
        return _sparsity;
    }

    /// @brief Sets `RangeOpts.trimFactor`.
    range& trim_factor(std::int32_t value) {
        _trim_factor = value;
        return *this;
    }

    /// @brief Gets `RangeOpts.trimFactor`.
    bsoncxx::v_noabi::stdx::optional<std::int32_t> const& trim_factor() const {
        return _trim_factor;
    }

    /// @brief Sets `RangeOpts.precision`.
    /// @note May only be set for `double` or `decimal128`.
    range& precision(std::int32_t value) {
        _precision = value;
        return *this;
    }

    /// @brief Gets `RangeOpts.precision`.
    /// @note May only be set for `double` or `decimal128`.
    bsoncxx::v_noabi::stdx::optional<std::int32_t> const& precision() const {
        return _precision;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _min;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _max;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _sparsity;
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _trim_factor;
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _precision;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::range from_v1(v1::range_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::range_options to_v1(v_noabi::options::range const& v) {
    return v1::range_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::range.
///
/// @par Includes
/// - @ref mongocxx/v1/range_options.hpp
///
