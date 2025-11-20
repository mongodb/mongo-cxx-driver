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

#include <mongocxx/hint-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/hint.hpp> // IWYU pragma: export

#include <string>
#include <utility>

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// The index to "hint" or force a MongoDB server to use when performing a query.
///
class hint {
   public:
    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() hint(v1::hint hint);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::hint() const {
        if (_index_doc) {
            return v1::hint{bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_index_doc->view())}};
        }

        return v1::hint{std::string{_index_string->view()}};
    }

    ///
    /// Constructs a new hint.
    ///
    /// Note: this constructor is purposefully not explicit, to allow conversion
    /// from either document::view or document::value to view_or_value.
    ///
    /// @param index
    ///   Document view or value representing the index to be used.
    ///
    hint(bsoncxx::v_noabi::document::view_or_value index) : _index_doc{std::move(index)} {}

    ///
    /// Constructs a new hint.
    ///
    /// @param index
    ///   String representing the name of the index to be used.
    ///
    explicit hint(bsoncxx::v_noabi::string::view_or_value index) : _index_string(std::move(index)) {}

    ///
    /// @relates mongocxx::v_noabi::hint
    ///
    /// Convenience methods to compare for equality against an index name.
    ///
    /// Compares equal if the hint contains a matching index name. Otherwise, compares unequal.
    ///
    friend bool operator==(hint const& index_hint, std::string index) {
        return index_hint._index_string == index;
    }

    ///
    /// @relates mongocxx::v_noabi::hint
    ///
    /// Convenience methods to compare for equality against an index document.
    ///
    /// Compares equal if the hint contains a matching index document. Otherwise, compares unequal.
    ///
    friend bool operator==(hint const& index_hint, bsoncxx::v_noabi::document::view index) {
        return index_hint._index_doc == index;
    }

    ///
    /// Returns a @ref bsoncxx::v_noabi::types::view representing this hint.
    ///
    /// @return Hint, as a @ref bsoncxx::v_noabi::types::view. The caller must ensure that the returned object not
    /// outlive the hint object that it was created from.
    ///
    bsoncxx::v_noabi::types::view to_value() const {
        if (_index_doc) {
            return bsoncxx::v_noabi::types::view{bsoncxx::v_noabi::types::b_document{_index_doc->view()}};
        }

        return bsoncxx::v_noabi::types::view{bsoncxx::v_noabi::types::b_string{_index_string->view()}};
    }

    ///
    /// Returns a @ref bsoncxx::v_noabi::types::view representing this hint.
    ///
    /// @return Hint, as a @ref bsoncxx::v_noabi::types::view. The caller must ensure that the returned object not
    /// outlive the hint object that it was created from.
    ///
    operator bsoncxx::v_noabi::types::view() const {
        return this->to_value();
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
inline bool operator==(std::string str, hint const& hint) {
    return hint == str;
}

/// @relatesalso mongocxx::v_noabi::hint
inline bool operator!=(hint const& hint, std::string str) {
    return !(hint == str);
}

/// @relatesalso mongocxx::v_noabi::hint
inline bool operator!=(std::string str, hint const& hint) {
    return !(hint == str);
}

/// @}
///

///
/// Convenience methods to compare for equality against an index document.
///
/// Compares equal if the hint contains a matching index document. Otherwise, compares unequal.
///
/// @{

/// @relatesalso mongocxx::v_noabi::hint
inline bool operator==(bsoncxx::v_noabi::document::view doc, hint const& hint) {
    return hint == doc;
}

/// @relatesalso mongocxx::v_noabi::hint
inline bool operator!=(hint const& hint, bsoncxx::v_noabi::document::view doc) {
    return !(hint == doc);
}

/// @relatesalso mongocxx::v_noabi::hint
inline bool operator!=(bsoncxx::v_noabi::document::view doc, hint const& hint) {
    return !(hint == doc);
}

/// @}
///

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::hint from_v1(v1::hint v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::hint to_v1(v_noabi::hint const& v) {
    return v1::hint{v};
}

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {

using v_noabi::operator==;
using v_noabi::operator!=;

} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::hint.
///
/// @par Includes
/// - @ref mongocxx/v1/hint.hpp
///
