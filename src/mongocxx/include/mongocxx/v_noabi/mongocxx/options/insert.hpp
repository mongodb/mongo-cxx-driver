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

#include <mongocxx/options/insert-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/insert_many_options.hpp> // IWYU pragma: export
#include <mongocxx/v1/insert_one_options.hpp>  // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/view.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::collection.
///
class insert {
   public:
    ///
    /// Default initialization.
    ///
    insert() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() insert(v1::insert_many_options opts);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() insert(v1::insert_one_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::insert_many_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::insert_many_options ret;

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_ordered) {
            ret.ordered(*_ordered);
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(*_comment)});
        }

        return ret;
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note The `ordered` field is ignored.
    ///
    explicit operator v1::insert_one_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::insert_one_options ret;

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_bypass_document_validation) {
            ret.bypass_document_validation(*_bypass_document_validation);
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(*_comment)});
        }

        return ret;
    }

    ///
    /// Sets the bypass_document_validation option.
    /// If true, allows the write to opt-out of document level validation.
    ///
    /// @note
    ///   On servers >= 3.2, the server applies validation by default. On servers < 3.2, this option
    ///   is ignored.
    ///
    /// @param bypass_document_validation
    ///   Whether or not to bypass document validation
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    insert& bypass_document_validation(bool bypass_document_validation) {
        _bypass_document_validation = bypass_document_validation;
        return *this;
    }

    ///
    /// Gets the current value of the bypass_document_validation option.
    ///
    /// @return The optional value of the bypass_document_validation option.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& bypass_document_validation() const {
        return _bypass_document_validation;
    }

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    insert& write_concern(v_noabi::write_concern wc) {
        _write_concern = std::move(wc);
        return *this;
    }

    ///
    /// The current write_concern for this operation.
    ///
    /// @return The current write_concern.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

    ///
    /// @note: This applies only to insert_many and is ignored for insert_one.
    ///
    /// If true, when an insert fails, return without performing the remaining
    /// writes. If false, when a write fails, continue with the remaining
    /// writes, if any. Inserts can be performed in any order if this is false.
    /// Defaults to true.
    ///
    /// @param ordered
    ///   Whether or not the insert_many will be ordered.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/insert/
    ///
    insert& ordered(bool ordered) {
        _ordered = ordered;
        return *this;
    }

    ///
    /// The current ordered value for this operation.
    ///
    /// @return The current ordered value.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/insert/
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& ordered() const {
        return _ordered;
    }

    ///
    /// Sets the comment for this operation.
    ///
    /// @param comment
    ///   The new comment.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/insert/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    insert& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// The current comment for this operation.
    ///
    /// @return The current comment.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/insert/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& comment() const {
        return _comment;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> _write_concern;
    bsoncxx::v_noabi::stdx::optional<bool> _ordered;
    bsoncxx::v_noabi::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _comment;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::insert from_v1(v1::insert_many_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
/// @note The `ordered` field is initialized as unset.
///
inline v_noabi::options::insert from_v1(v1::insert_one_options v) {
    return {std::move(v)};
}

// Ambiguous whether `v_noabi::options::insert` should be converted to `v1::insert_many_options` or
// `v1::insert_one_options`. Require users to explicitly cast to the expected type instead.
//
// v1::insert_many_options to_v1(v_noabi::options::insert const& v);
// v1::insert_one_options to_v1(v_noabi::options::insert const& v);

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::insert.
///
/// @par Includes
/// - @ref mongocxx/v1/insert_many_options.hpp
/// - @ref mongocxx/v1/insert_one_options.hpp
///
