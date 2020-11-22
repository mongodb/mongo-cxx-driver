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

#include <sstream>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
using namespace bsoncxx::types;

///
/// A JSON-like builder for creating BSON documents.
///
class document {
    using key_type = stdx::string_view;
    using mapped_type = document;
    using value_type = std::pair<key_type, mapped_type>;
    using initializer_list_t = std::initializer_list<value_type>;

   public:
    ///
    /// Creates an empty document.
    ///
    document() : document({}){};

    ///
    /// Creates a bsoncxx::builder::document from a value of type T.
    ///
    /// @warning T must be a BSON type, i.e., implicitly convertible to a
    /// bsoncxx::types::bson_value::value.
    ///
    /// @see bsoncxx::types::bson_value::value.
    //
    /// @warning Extracting a BSON document from an object created with this constructor will result
    /// in an empty document. Use the key-value or std::initializer_list constructors instead.
    ///
    /// @note This constructor bridges the gap between a homogeneous container, a
    /// std::initializer_list, and a heterogeneous one, a BSON document. In other words, the
    /// std::initializer_list constructor implicitly uses this to convert all elements into a common
    /// type, a bsoncxx::builder::document.
    ///
    template <typename T>
    document(T v) : _value{v} {}

    ///
    /// Creates a BSON document from a single key-value pair, e.g., { "key" : 1 }
    ///
    /// @param key
    ///     the key in the key-value pair.
    /// @param value
    ///     the value in the key-value pair. It must be a BSON type, i.e., implicitly convertible to
    ///     a bsoncxx::types::bson_value::value, or another bsoncxx::builder::document.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    document(key_type key, mapped_type&& value)
        : document({{key, std::forward<mapped_type>(value)}}) {}

    ///
    /// Creates a BSON document.
    ///
    /// @param init
    ///     the initializer list used to construct the BSON document
    ///
    document(initializer_list_t init) : _is_document{true} {
        for (auto&& kvp : init) {
            _core.key_view(kvp.first);
            kvp.second._is_document ? _core.append(kvp.second._core.view_document())
                                    : _core.append(kvp.second._value);
        }
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
    document& operator+=(document rhs) {
        if (!rhs._is_document)
            throw bsoncxx::exception{error_code::k_internal_error};
        _core.concatenate(rhs.extract());
        return *this;
    }

   private:
    bool _is_document{false};
    core _core{false};
    bson_value::value _value{nullptr};
};

///
/// A JSON-like builder for creating BSON arrays.
///
class array {
    using value_type = array;
    using initializer_list_t = std::initializer_list<value_type>;

   public:
    ///
    /// Creates an empty array.
    ///
    array() : array({}){};

    ///
    /// Creates an array from a single value of type T.
    ///
    /// @warning T must be a BSON type, i.e., implicitly convertible to a
    /// bsoncxx::types::bson_value::value.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    template <typename T>
    array(T value) : _value{value} {
        _core.append(_value);
    }

    ///
    /// Creates a BSON array.
    ///
    /// @param init
    ///     the initializer list used to construct the BSON array
    ///
    /// @see bsoncxx::builder::list
    /// @see bsoncxx::builder::document
    ///
    array(initializer_list_t init) : _is_array{true} {
        for (auto&& ele : init)
            append_array_or_value(_core, ele);
    }

    operator bsoncxx::array::value() {
        return this->extract();
    }

    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    array& operator+=(value_type o) {
        append_array_or_value(_core, o);
        return *this;
    }

   private:
    void append_array_or_value(core& core, const array& ele) {
        ele._is_array ? core.append(ele._core.view_array()) : core.append(ele._value);
    }

    bool _is_array{false};
    bson_value::value _value{nullptr};
    core _core{true};
};
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
