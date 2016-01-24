// Copyright 2014 MongoDB Inc.
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

#include <cstddef>
#include <cstdint>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

enum class type : std::uint8_t;
enum class binary_sub_type : std::uint8_t;

namespace types {
struct b_eod;
struct b_double;
struct b_utf8;
struct b_document;
struct b_array;
struct b_binary;
struct b_undefined;
struct b_oid;
struct b_bool;
struct b_date;
struct b_null;
struct b_regex;
struct b_dbpointer;
struct b_code;
struct b_symbol;
struct b_codewscope;
struct b_int32;
struct b_timestamp;
struct b_int64;
struct b_minkey;
struct b_maxkey;
class value;
}  // namespace types

namespace array {
class element;
}  // namespace array

namespace document {

///
/// @todo document this class
///
class BSONCXX_API element {
   public:

    ///
    /// @todo document this method
    ///
    element();

    ///
    /// @todo document this method
    ///
    explicit element(const std::uint8_t* raw, std::uint32_t length, std::uint32_t offset);

    ///
    /// @todo document this method
    ///
    explicit operator bool() const;

    ///
    /// @todo document this method
    ///
    const std::uint8_t* raw() const;

    ///
    /// @todo document this method
    ///
    void raw(const std::uint8_t*);

    ///
    /// @todo document this method
    ///
    std::uint32_t length() const;

    ///
    /// @todo document this method
    ///
    void length(std::uint32_t);

    ///
    /// @todo document this method
    ///
    std::uint32_t offset() const;

    ///
    /// @todo document this method
    ///
    void offset(std::uint32_t);

    ///
    /// @todo document this method
    ///
    bsoncxx::type type() const;

    ///
    /// @todo document this method
    ///
    stdx::string_view key() const;

    ///
    /// @todo document this method
    ///
    types::b_double get_double() const;

    ///
    /// @todo document this method
    ///
    types::b_utf8 get_utf8() const;

    ///
    /// @todo document this method
    ///
    types::b_document get_document() const;

    ///
    /// @todo document this method
    ///
    types::b_array get_array() const;

    ///
    /// @todo document this method
    ///
    types::b_binary get_binary() const;

    ///
    /// @todo document this method
    ///
    types::b_undefined get_undefined() const;

    ///
    /// @todo document this method
    ///
    types::b_oid get_oid() const;

    ///
    /// @todo document this method
    ///
    types::b_bool get_bool() const;

    ///
    /// @todo document this method
    ///
    types::b_date get_date() const;

    ///
    /// @todo document this method
    ///
    types::b_null get_null() const;

    ///
    /// @todo document this method
    ///
    types::b_regex get_regex() const;

    ///
    /// @todo document this method
    ///
    types::b_dbpointer get_dbpointer() const;

    ///
    /// @todo document this method
    ///
    types::b_code get_code() const;

    ///
    /// @todo document this method
    ///
    types::b_symbol get_symbol() const;

    ///
    /// @todo document this method
    ///
    types::b_codewscope get_codewscope() const;

    ///
    /// @todo document this method
    ///
    types::b_int32 get_int32() const;

    ///
    /// @todo document this method
    ///
    types::b_timestamp get_timestamp() const;

    ///
    /// @todo document this method
    ///
    types::b_int64 get_int64() const;

    ///
    /// @todo document this method
    ///
    types::b_minkey get_minkey() const;

    ///
    /// @todo document this method
    ///
    types::b_maxkey get_maxkey() const;

    ///
    /// @todo document this method
    ///
    types::value get_value() const;

    ///
    /// If this element is a document, finds the first element of the document with the provided
    /// key. If there is no such element, an invalid document::element will be returned. The
    /// runtime of operator[] is linear in the length of the document.
    ///
    /// @throws bsoncxx::exception if this element is not a document.
    ///
    /// @param key
    ///   The key to search for.
    ///
    /// @return The matching element, if found, or an invalid element.
    ///
    element operator[](stdx::string_view key) const;

    ///
    /// If this element is an array, indexes into this BSON array. If the index is out-of-bounds,
    /// an invalid array::element will be returned. As BSON represents arrays as documents, the
    /// runtime of operator[] is linear in the length of the array.
    ///
    /// @throws bsoncxx::exception if this element is not an array.
    ///
    /// @param i
    ///   The index of the element.
    ///
    /// @return The element if it exists, or an invalid element.
    ///
    array::element operator[](std::uint32_t i) const;

   private:
    const std::uint8_t* _raw;
    std::uint32_t _length;
    std::uint32_t _offset;
};

}  // namespace document

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
