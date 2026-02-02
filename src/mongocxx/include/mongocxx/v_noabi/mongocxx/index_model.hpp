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

#include <mongocxx/index_model-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/indexes.hpp> // IWYU pragma: export

#include <utility>

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <mongocxx/options/index.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Used by @ref mongocxx::v_noabi::index_model.
///
class index_model {
   private:
    v1::indexes::model _index;

   public:
    ///
    /// Initializes a new index_model over a mongocxx::v_noabi::collection.
    ///
    index_model(
        bsoncxx::v_noabi::document::view_or_value const& keys,
        bsoncxx::v_noabi::document::view_or_value const& options = {})
        : _index{
              bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(keys.view())},
              bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(options.view())}} {}

    index_model() = delete;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ index_model(v1::indexes::model index) : _index{std::move(index)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::indexes::model() && {
        return std::move(_index);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::indexes::model() const& {
        return _index;
    }

    ///
    /// Retrieves keys of an index_model.
    ///
    bsoncxx::v_noabi::document::view keys() const {
        return _index.keys();
    }

    ///
    /// Retrieves options of an index_model.
    ///
    bsoncxx::v_noabi::document::view options() const {
        return _index.options();
    }
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::index_model from_v1(v1::indexes::model v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::indexes::model to_v1(v_noabi::index_model v) {
    return v1::indexes::model{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::index_model.
///
/// @par Includes
/// - @ref mongocxx/v1/indexes.hpp
///
