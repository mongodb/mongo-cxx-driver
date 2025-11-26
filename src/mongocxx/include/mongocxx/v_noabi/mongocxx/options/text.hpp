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

#include <mongocxx/options/text-fwd.hpp> // IWYU pragma: export

#include <mongocxx/options/prefix.hpp>
#include <mongocxx/options/substring.hpp>
#include <mongocxx/options/suffix.hpp>

//

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::options::encrypt::text_opts.
///
/// Specifies index options (`TextOpts`) for a Queryable Encryption field supporting "textPreview"
/// queries.
///
/// @note `caseSensitive`, `diacriticSensitive`, `prefix`, `suffix`, and `substring` must match the values set in the
/// encryptedFields of the destination collection.
///
class text {
   public:
    ///
    /// Default initialization.
    ///
    text() = default;

    /// @brief Sets `TextOpts.caseSensitive`
    text& case_sensitive(bool value) {
        _case_sensitive = value;
        return *this;
    }

    /// @brief Gets `TextOpts.caseSensitive`
    bsoncxx::v_noabi::stdx::optional<bool> const& case_sensitive() const {
        return _case_sensitive;
    }

    /// @brief Sets `TextOpts.diacriticSensitive`
    text& diacritic_sensitive(bool value) {
        _diacritic_sensitive = value;
        return *this;
    }

    /// @brief Gets `TextOpts.diacriticSensitive`
    bsoncxx::v_noabi::stdx::optional<bool> const& diacritic_sensitive() const {
        return _diacritic_sensitive;
    }

    /// @brief Sets `TextOpts.PrefixOpts`
    text& prefix(options::prefix opts) {
        _prefix = opts;
        return *this;
    }

    /// @brief Gets `TextOpts.PrefixOpts`
    bsoncxx::v_noabi::stdx::optional<options::prefix> const& prefix() const {
        return _prefix;
    }

    /// @brief Sets `TextOpts.SuffixOpts`
    text& suffix(options::suffix opts) {
        _suffix = opts;
        return *this;
    }

    /// @brief Gets `TextOpts.SuffixOpts`
    bsoncxx::v_noabi::stdx::optional<options::suffix> const& suffix() const {
        return _suffix;
    }

    /// @brief Sets `TextOpts.SubstringOpts`
    text& substring(options::substring opts) {
        _substring = opts;
        return *this;
    }

    /// @brief Gets `TextOpts.SubstringOpts`
    bsoncxx::v_noabi::stdx::optional<options::substring> const& substring() const {
        return _substring;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _case_sensitive;
    bsoncxx::v_noabi::stdx::optional<bool> _diacritic_sensitive;
    bsoncxx::v_noabi::stdx::optional<options::prefix> _prefix;
    bsoncxx::v_noabi::stdx::optional<options::suffix> _suffix;
    bsoncxx::v_noabi::stdx::optional<options::substring> _substring;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::text.
///
