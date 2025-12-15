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

#include <mongocxx/options/prefix-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::encrypt::text_opts.
///
/// Specifies index options (`RangeOpts`) for a Queryable Encryption field supporting "textPreview"
/// queries.
///
/// @note `caseSensitive`, `diacriticSensitive`, `prefix`, `suffix`, and `substring` must match the values set in the
/// encryptedFields of the destination collection.
///
class prefix {
   public:
    ///
    /// Default initialization.
    ///
    prefix() = default;

    /// @brief Sets `PrefixOpts.strMaxQueryLength`
    prefix& str_max_query_length(int32_t value) {
        _max_query_length = value;
        return *this;
    }

    /// @brief Gets `PrefixOpts.strMaxQueryLength`
    bsoncxx::v_noabi::stdx::optional<int32_t> const& str_max_query_length() const {
        return _max_query_length;
    }

    /// @brief Sets `PrefixOpts.strMinQueryLength`
    prefix& str_min_query_length(int32_t value) {
        _min_query_length = value;
        return *this;
    }

    /// @brief Gets `PrefixOpts.strMinQueryLength`
    bsoncxx::v_noabi::stdx::optional<int32_t> const& str_min_query_length() const {
        return _min_query_length;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<int32_t> _min_query_length;
    bsoncxx::v_noabi::stdx::optional<int32_t> _max_query_length;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::prefix.
///
