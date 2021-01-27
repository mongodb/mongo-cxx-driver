// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace list {
using namespace bsoncxx::types;

///
/// A JSON-like builder for creating BSON documents.
///
class document {
   public:
    ///
    /// Default constructor. Creates an empty document builder.
    ///
    document() = default;

    ///
    /// Creates a document builder from a key-value pair. Initializes the key with 'key' and the
    /// value with 'value'. The value may be another document builder.
    ///
    /// @param key
    ///     the value to initialize the key in the key-value pair.
    /// @param value
    ///     the value to initialize the value in the key-value pair.
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from 'value'.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    template <typename T>
    document(stdx::string_view key, T&& value) {
        _core.key_view(key);
        _core.append(bson_value::value{std::forward<T>(value)});
    }

    ///
    /// Constructs the document builder with the contents of the initializer list 'init'. All
    /// document builders in 'init' are concatenated.
    ///
    /// @param init initializer list to initialize the key-value pairs of the document builder with
    ///
    document(std::initializer_list<document> init) {
        for (auto&& doc : init) {
            bson_value::value val{doc._core.view_document()};
            _core.concatenate(val.view().get_document());
        }
    }

    ///
    /// Move constructor. Constructs the document builder with the contents of other using move
    /// semantics.
    ///
    /// @param other another document builder to use as source to initialize this document builder
    /// with
    ///
    document(document&& other) noexcept = default;

    ///
    /// Replaces the contents with those of other using move semantics.
    ///
    /// @param other another document builder to use as source to initialize this document builder
    /// with
    ///
    document& operator=(document&& other) noexcept = default;

    ///
    /// Returns an owning bsoncxx::document::value.
    ///
    /// @return An owning document::value representing the entire contents of the document builder.
    ///
    /// @see bsoncxx::document::value
    ///
    /// @warning
    ///   After calling this method it is illegal to call any methods on this class, unless
    ///   it is subsequenly moved into.
    ///
    bsoncxx::document::value extract() {
        return _core.extract_document();
    }

    ///
    /// Concatenates document builder rhs.
    ///
    /// @param rhs document builder to concatenate
    ///
    /// @return *this
    ///
    document& operator+=(document&& rhs) {
        this->concatenate(std::move(rhs));
        return *this;
    }

    ///
    /// Concatenates document builder rhs.
    ///
    /// @param rhs document builder to concatenate
    ///
    /// @return *this
    ///
    document& concatenate(document&& rhs) {
        _core.concatenate(rhs.extract().view());
        return *this;
    }

   private:
    core _core{false};
};
}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
