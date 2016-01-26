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

#include <bsoncxx/document/element.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace array {

///
/// A variant view type that accesses values in serialized BSON arrays.
///
/// Element functions as a variant type, where the kind of the element can be
/// interrogated by calling type() and a specific value can be extracted through
/// get_X() accessors.
///
class BSONCXX_API element : private document::element {
   public:
    element();

    explicit element(const std::uint8_t* raw, std::uint32_t length, std::uint32_t offset);

    using document::element::operator bool;

    using document::element::type;

    using document::element::get_double;
    using document::element::get_utf8;
    using document::element::get_document;
    using document::element::get_array;
    using document::element::get_binary;
    using document::element::get_undefined;
    using document::element::get_oid;
    using document::element::get_bool;
    using document::element::get_date;
    using document::element::get_null;
    using document::element::get_regex;
    using document::element::get_dbpointer;
    using document::element::get_code;
    using document::element::get_symbol;
    using document::element::get_codewscope;
    using document::element::get_int32;
    using document::element::get_timestamp;
    using document::element::get_int64;
    using document::element::get_minkey;
    using document::element::get_maxkey;

    using document::element::get_value;

    using document::element::operator[];

    using document::element::raw;
    using document::element::length;
    using document::element::offset;
};

}  // namespace array

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
