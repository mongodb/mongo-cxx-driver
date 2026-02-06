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

#include <mongocxx/search_index_model-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/search_indexes.hpp> // IWYU pragma: export

#include <memory>
#include <string> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Used by @ref mongocxx::v_noabi::search_index_view.
///
class search_index_model {
   public:
    ///
    /// Initializes a new search_index_model over a mongocxx::v_noabi::collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL()
    search_index_model(bsoncxx::v_noabi::document::view_or_value definition);

    MONGOCXX_ABI_EXPORT_CDECL()
    search_index_model(
        bsoncxx::v_noabi::string::view_or_value name,
        bsoncxx::v_noabi::document::view_or_value definition);

    search_index_model() = delete;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() search_index_model(v1::search_indexes::model opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::search_indexes::model() const;

    ///
    /// Move constructs a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() search_index_model(search_index_model&& other) noexcept;

    ///
    /// Move assigns a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(search_index_model&) operator=(search_index_model&& other) noexcept;

    ///
    /// Copy constructs a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() search_index_model(search_index_model const& other);

    ///
    /// Copy assigns a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(search_index_model&) operator=(search_index_model const& other);

    ///
    /// Destroys a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~search_index_model();

    ///
    /// Retrieves name of a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>)
    name() const;

    ///
    /// Retrieves definition of a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) definition() const;

    ///
    /// Retrieves type of a search_index_model.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>)
    type() const;

    ///
    /// Sets type of a search_index_model.
    ///
    /// @param type The type for this index. Can be either "search" or "vectorSearch".
    ///
    /// @return A reference to this object to facilitate method chaining.
    MONGOCXX_ABI_EXPORT_CDECL(search_index_model&)
    type(bsoncxx::v_noabi::string::view_or_value type);

    class internal;

   private:
    class impl;
    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::search_index_model from_v1(v1::search_indexes::model v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::search_indexes::model to_v1(v_noabi::search_index_model v) {
    return v1::search_indexes::model{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::search_index_model.
///
/// @par Includes
/// - @ref mongocxx/v1/search_indexes.hpp
///
