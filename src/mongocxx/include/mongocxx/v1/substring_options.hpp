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

#include <mongocxx/options/substring-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::encrypt::text_opts.
///
/// Specifies index options (`SubstringOpts`) for a Queryable Encryption field supporting "substringPreview"
/// queries.
///
/// @note `strMaxQueryLength`, `strMinQueryLength`, and `strMaxLength` must match the values set in the
/// encryptedFields of the destination collection.
///
class substring {
   public:
    ///
    /// Default initialization.
    ///
    substring() = default;

    /// @brief Sets `SubstringOpts.strMaxQueryLength`
    substring& str_max_query_length(int32_t value) {
        _max_query_length = value;
        return *this;
    }

    /// @brief Gets `SubstringOpts.strMaxQueryLength`
    bsoncxx::v_noabi::stdx::optional<int32_t> const& str_max_query_length() const {
        return _max_query_length;
    }

    /// @brief Sets `SubstringOpts.strMinQueryLength`
    substring& str_min_query_length(int32_t value) {
        _min_query_length = value;
        return *this;
    }

    /// @brief Gets `SubstringOpts.strMinQueryLength`
    bsoncxx::v_noabi::stdx::optional<int32_t> const& str_min_query_length() const {
        return _min_query_length;
    }

    /// @brief Sets `SubstringOpts.strMaxLength`
    substring& str_max_length(int32_t value) {
        _max_length = value;
        return *this;
    }

    /// @brief Gets `substringOpts.strMaxLength`
    bsoncxx::v_noabi::stdx::optional<int32_t> const& str_max_length() const {
        return _max_length;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<int32_t> _min_query_length;
    bsoncxx::v_noabi::stdx::optional<int32_t> _max_query_length;
    bsoncxx::v_noabi::stdx::optional<int32_t> _max_length;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::substring.
///
