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
    /// Default constructor. Creates an empty document.
    ///
    document() = default;

    ///
    /// Creates a document from a key-value pair. Initializes the key with 'key' and the value with
    /// 'value'. The value may be another document.
    ///
    /// @param key
    ///     the key in the key-value pair.
    /// @param value
    ///     the value in the key-value pair.
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from 'value'.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    template <typename T>
    document(stdx::string_view key, T&& value) {
        _core.key_view(key);
        _core.append(bson_value::value{value});
    }

    ///
    /// Creates a document from a key-value pair. The value may be another document.
    ///
    /// @param key
    ///     the key in the key-value pair.
    /// @param value
    ///     the value in the key-value pair.
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from 'value'.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    template <typename T>
    document(stdx::string_view key, const T& value) {
        _core.key_view(key);
        _core.append(bson_value::value{value});
    }

    ///
    /// Constructs the document with the concatenated contents of the initializer list 'init'.
    ///
    document(std::initializer_list<document> init) {
        for (auto doc : init) {
            bson_value::value val{doc};
            _core.concatenate(val.view().get_document());
        }
    }

    ///
    /// Move constructor. Constructs the document with the contents of 'other' using move semantics.
    ///
    document(document&& other) noexcept = default;

    ///
    /// Replaces the contents with those of 'other' using move semantics.
    ///
    document& operator=(document&& other) noexcept = default;

    ///
    /// Creates a document identical to the given document. The given document will not be modified.
    ///
    document(const document& other) {
        this->append(other);
    }

    ///
    /// Creates a document identical to the given document. The given document will not be modified.
    ///
    document& operator=(const document& other) {
        if (this != &other)
            *this = document(other);
        return *this;
    }

    operator bsoncxx::document::value() {
        return this->extract();
    }

    bsoncxx::document::value extract() {
        return _core.extract_document();
    }

    ///
    /// Appends a bsoncxx::builder::document.
    ///
    /// @exception
    ///    Throws a bsoncxx::exception if the document is malformed.
    ///
    document& operator+=(const document& rhs) {
        this->append(rhs);
        return *this;
    }

    document& operator+=(document&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    ///
    /// Appends a bsoncxx::builder::document.
    ///
    /// @exception
    ///    Throws a bsoncxx::exception if the document is malformed.
    ///
    document& append(const document& rhs) {
        _core.concatenate(rhs._core.view_document());
        return *this;
    }

    document& append(document&& rhs) {
        _core.concatenate(std::move(rhs.extract()));
        return *this;
    }

   private:
    core _core{false};
};
}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
