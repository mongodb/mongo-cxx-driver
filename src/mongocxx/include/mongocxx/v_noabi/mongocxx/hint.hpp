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

#include <string>

#include <mongocxx/hint-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// The index to "hint" or force a MongoDB server to use when performing a query.
///
class hint {
   public:
    ///
    /// Constructs a new hint.
    ///
    /// Note: this constructor is purposefully not explicit, to allow conversion
    /// from either document::view or document::value to view_or_value.
    ///
    /// @param index
    ///   Document view or value representing the index to be used.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() hint(bsoncxx::v_noabi::document::view_or_value index);

    ///
    /// Constructs a new hint.
    ///
    /// @param index
    ///   String representing the name of the index to be used.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() hint(bsoncxx::v_noabi::string::view_or_value index);

    ///
    /// @relates mongocxx::v_noabi::hint
    ///
    /// Convenience methods to compare for equality against an index name.
    ///
    /// Compares equal if the hint contains a matching index name. Otherwise, compares unequal.
    ///
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(hint const& index_hint, std::string index);

    ///
    /// @relates mongocxx::v_noabi::hint
    ///
    /// Convenience methods to compare for equality against an index document.
    ///
    /// Compares equal if the hint contains a matching index document. Otherwise, compares unequal.
    ///
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(hint const& index_hint, bsoncxx::v_noabi::document::view index);

    ///
    /// Returns a types::bson_value::view representing this hint.
    ///
    /// @return Hint, as a types::bson_value::view. The caller must ensure that the returned object
    /// not outlive
    /// the hint object that it was created from.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::types::bson_value::view) to_value() const;

    ///
    /// Returns a types::bson_value::view representing this hint.
    ///
    /// @return Hint, as a types::bson_value::view. The caller must ensure that the returned object
    /// not outlive
    /// the hint object that it was created from.
    ///
    operator bsoncxx::v_noabi::types::bson_value::view() const {
        return to_value();
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _index_doc;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _index_string;
};

///
/// Convenience methods to compare against an index name.
///
/// Compares equal if the hint contains a matching index name. Otherwise, compares unequal.
///
/// @{

/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(std::string index, hint const& index_hint);

/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(hint const& index_hint, std::string index);

/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(std::string index, hint const& index_index);

/// @}
///

///
/// Convenience methods to compare for equality against an index document.
///
/// Compares equal if the hint contains a matching index document. Otherwise, compares unequal.
///
/// @{

/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool)
operator==(bsoncxx::v_noabi::document::view index, hint const& index_hint);
/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool)
operator!=(hint const& index_hint, bsoncxx::v_noabi::document::view index);
/// @relatesalso mongocxx::v_noabi::hint
MONGOCXX_ABI_EXPORT_CDECL(bool)
operator!=(bsoncxx::v_noabi::document::view index, hint const& index_hint);

/// @}
///

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {

using ::mongocxx::v_noabi::operator==;
using ::mongocxx::v_noabi::operator!=;

} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::hint.
///
