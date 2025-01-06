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

#include <mongocxx/options/range-fwd.hpp>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

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
    /// @brief Sets `RangeOpts.min`.
    /// @note Required if @ref precision is set.
    MONGOCXX_ABI_EXPORT_CDECL(range&) min(bsoncxx::v_noabi::types::bson_value::view_or_value value);

    /// @brief Gets `RangeOpts.min`.
    /// @note Required if @ref precision is set.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<
                              bsoncxx::v_noabi::types::bson_value::view_or_value> const&)
    min() const;

    /// @brief Sets `RangeOpts.max`.
    /// @note Required if @ref precision is set.
    MONGOCXX_ABI_EXPORT_CDECL(range&) max(bsoncxx::v_noabi::types::bson_value::view_or_value value);

    /// @brief Gets `RangeOpts.max`.
    /// @note Required if @ref precision is set.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<
                              bsoncxx::v_noabi::types::bson_value::view_or_value> const&)
    max() const;

    /// @brief Sets `RangeOpts.sparsity`.
    MONGOCXX_ABI_EXPORT_CDECL(range&) sparsity(std::int64_t value);

    /// @brief Gets `RangeOpts.sparsity`.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int64_t> const&)
    sparsity() const;

    /// @brief Sets `RangeOpts.trimFactor`.
    MONGOCXX_ABI_EXPORT_CDECL(range&) trim_factor(std::int32_t value);

    /// @brief Gets `RangeOpts.trimFactor`.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t> const&)
    trim_factor() const;

    /// @brief Sets `RangeOpts.precision`.
    /// @note May only be set for `double` or `decimal128`.
    MONGOCXX_ABI_EXPORT_CDECL(range&) precision(std::int32_t value);

    /// @brief Gets `RangeOpts.precision`.
    /// @note May only be set for `double` or `decimal128`.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::int32_t> const&)
    precision() const;

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

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::range.
///
