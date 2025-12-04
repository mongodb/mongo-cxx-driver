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

#include <mongocxx/options/delete-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/delete_many_options.hpp> // IWYU pragma: export
#include <mongocxx/v1/delete_one_options.hpp>  // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/hint.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by MongoDB delete operations.
///
class delete_options {
   public:
    ///
    /// Default initialization.
    ///
    delete_options() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() delete_options(v1::delete_many_options opts);

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() delete_options(v1::delete_one_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::delete_many_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::delete_many_options ret;

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        return ret;
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::delete_one_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::delete_one_options ret;

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(_collation->view())});
        }

        if (_write_concern) {
            ret.write_concern(to_v1(*_write_concern));
        }

        if (_hint) {
            ret.hint(to_v1(*_hint));
        }

        if (_let) {
            ret.let(bsoncxx::v1::document::value{to_v1(_let->view())});
        }

        if (_comment) {
            ret.comment(bsoncxx::v1::types::value{to_v1(_comment->view())});
        }

        return ret;
    }

    ///
    /// Sets the collation for this operation.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    delete_options& collation(bsoncxx::v_noabi::document::view_or_value collation) {
        _collation = std::move(collation);
        return *this;
    }

    ///
    /// Retrieves the current collation for this operation.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& collation() const {
        return _collation;
    }

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    delete_options& write_concern(write_concern wc) {
        _write_concern = std::move(wc);
        return *this;
    }

    ///
    /// The current write_concern for this operation.
    ///
    /// @return
    ///   The current write_concern.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/write-concern/
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> const& write_concern() const {
        return _write_concern;
    }

    ///
    /// Sets the index to use for this operation.
    ///
    /// @note if the server already has a cached shape for this query, it may
    /// ignore a hint.
    ///
    /// @param index_hint
    ///   Object representing the index to use.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    delete_options& hint(v_noabi::hint index_hint) {
        _hint = std::move(index_hint);
        return *this;
    }

    ///
    /// Gets the current hint.
    ///
    /// @return The current hint, if one is set.
    ///
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> const& hint() const {
        return _hint;
    }

    ///
    /// Set the value of the let option.
    ///
    /// @param let
    ///   The new let option.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    delete_options& let(bsoncxx::v_noabi::document::view_or_value let) {
        _let = std::move(let);
        return *this;
    }

    ///
    /// Gets the current value of the let option.
    ///
    /// @return
    ///  The current let option.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const let() const {
        return _let;
    }

    ///
    /// Set the value of the comment option.
    ///
    /// @param comment
    ///   The new comment option.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    delete_options& comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
        _comment = std::move(comment);
        return *this;
    }

    ///
    /// Gets the current value of the comment option.
    ///
    /// @return
    ///  The current comment option.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const comment() const {
        return _comment;
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> _write_concern;
    bsoncxx::v_noabi::stdx::optional<v_noabi::hint> _hint;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _let;
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
inline v_noabi::options::delete_options from_v1(v1::delete_many_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::delete_options from_v1(v1::delete_one_options v) {
    return {std::move(v)};
}

// Ambiguous whether `v_noabi::options::delete_options` should be converted to `v1::delete_many_options` or
// `v1::delete_one_options`. Require users to explicitly cast to the expected type instead.
//
// v1::delete_many_options to_v1(v_noabi::options::delete_options const& v);
// v1::delete_one_options to_v1(v_noabi::options::delete_options const& v);

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::delete_options.
///
/// @par Includes
/// - @ref mongocxx/v1/delete_many_options.hpp
/// - @ref mongocxx/v1/delete_one_options.hpp
///
