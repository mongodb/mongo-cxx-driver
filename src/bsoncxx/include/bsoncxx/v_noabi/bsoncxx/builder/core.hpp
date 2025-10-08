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

#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <bsoncxx/builder/core-fwd.hpp>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/stdx/type_traits.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {

///
/// A low-level interface for constructing BSON documents and arrays.
///
/// @remark
///   Generally it is recommended to use the classes in builder::basic or builder::stream instead of
///   using this class directly. However, developers who wish to write their own abstractions may
///   find this class useful.
///
class core {
   public:
    class impl;

    ///
    /// Constructs an empty BSON datum.
    ///
    /// @param is_array
    ///   True if the top-level BSON datum should be an array.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() core(bool is_array);

    BSONCXX_ABI_EXPORT_CDECL() core(core&& rhs) noexcept;
    BSONCXX_ABI_EXPORT_CDECL(core&) operator=(core&& rhs) noexcept;

    BSONCXX_ABI_EXPORT_CDECL() ~core();

    core(core const&) = delete;
    core& operator=(core const&) = delete;

    ///
    /// Appends a key passed as a non-owning stdx::string_view.
    ///
    /// @remark
    ///   Use key_owned() unless you know what you are doing.
    ///
    /// @warning
    ///   The caller must ensure that the lifetime of the backing string extends until the next
    ///   value is appended.
    ///
    /// @param key
    ///   A null-terminated array of characters.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws bsoncxx::v_noabi::exception if the current BSON datum is an array or if the previous
    /// value appended to the builder was also a key.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) key_view(stdx::string_view key);

    ///
    /// Appends a key passed as an STL string.  Transfers ownership of the key to this class.
    ///
    /// @param key
    ///   A string key.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws bsoncxx::v_noabi::exception if the current BSON datum is an array or if the previous
    /// value appended to the builder was a key.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) key_owned(std::string key);

    ///
    /// Opens a sub-document within this BSON datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) open_document();

    ///
    /// Opens a sub-array within this BSON datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) open_array();

    ///
    /// Closes the current sub-document within this BSON datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws bsoncxx::v_noabi::exception if the current BSON datum is not an open sub-document.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) close_document();

    ///
    /// Closes the current sub-array within this BSON datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws bsoncxx::v_noabi::exception if the current BSON datum is not an open sub-array.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) close_array();

    ///
    /// Appends the keys from a BSON document into this BSON datum.
    ///
    /// @note
    ///   If this BSON datum is a document, the original keys from `view` are kept.  Otherwise (if
    ///   this BSON datum is an array), the original keys from `view` are discarded.
    ///
    /// @note
    ///   This can be used with an array::view as well by converting it to a document::view first.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if one of the keys fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) concatenate(bsoncxx::v_noabi::document::view const& view);

    ///
    /// Appends a BSON double.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the double fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_double const& value);

    ///
    /// Append a BSON UTF-8 string.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the string fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_string const& value);

    ///
    /// Appends a BSON document.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the document fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_document const& value);

    ///
    /// Appends a BSON array.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the array fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_array const& value);

    ///
    /// Appends a BSON binary datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the binary fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_binary const& value);

    ///
    /// Appends a BSON binary datum by allocating space that the caller must fill with content.
    ///
    /// @return
    ///   A pointer to the allocated binary data block. The caller must write to every
    ///   byte or discard the builder.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the binary fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(uint8_t*) append(binary_sub_type sub_type, uint32_t length);

    ///
    /// Appends a BSON undefined.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if undefined fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_undefined const& value);

    ///
    /// Appends a BSON ObjectId.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the ObjectId fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_oid const& value);

    ///
    /// Appends a BSON boolean.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the boolean fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_bool const& value);

    ///
    /// Appends a BSON date.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the date fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_date const& value);

    ///
    /// Appends a BSON null.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if null fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_null const& value);

    ///
    /// Appends a BSON regex.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the regex fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_regex const& value);

    ///
    /// Appends a BSON DBPointer.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the DBPointer fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_dbpointer const& value);

    ///
    /// Appends a BSON JavaScript code.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the JavaScript code fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_code const& value);

    ///
    /// Appends a BSON symbol.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the symbol fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_symbol const& value);

    ///
    /// Appends a BSON JavaScript code with scope.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the JavaScript code with scope fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_codewscope const& value);

    ///
    /// Appends a BSON 32-bit signed integer.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the 32-bit signed integer fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_int32 const& value);

    ///
    /// Appends a BSON replication timestamp.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the timestamp fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_timestamp const& value);

    ///
    /// Appends a BSON 64-bit signed integer.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the 64-bit signed integer fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_int64 const& value);

    ///
    /// Appends a BSON Decimal128.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the Decimal128 fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_decimal128 const& value);

    ///
    /// Appends a BSON min-key.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the min-key fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_minkey const& value);

    ///
    /// Appends a BSON max-key.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///   bsoncxx::v_noabi::exception if the max-key fails to append.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::b_maxkey const& value);

    ///
    /// Appends a BSON variant value.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(types::bson_value::view const& value);

    ///
    /// Appends an STL string as a BSON UTF-8 string.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(std::string str);

    ///
    /// Appends a string view as a BSON UTF-8 string.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(stdx::string_view str);

    ///
    /// Appends a char* or char const*.
    ///
    /// We disable all other pointer types to prevent the surprising implicit conversion to bool.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    template <typename T>
    core& append(T* v) {
        static_assert(detail::is_alike<T, char>::value, "append is disabled for non-char pointer types");
        append(types::b_string{v});

        return *this;
    }

    ///
    /// Appends a native boolean as a BSON boolean.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(bool value);

    ///
    /// Appends a native double as a BSON double.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(double value);

    ///
    /// Appends a native int32_t as a BSON 32-bit signed integer.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(std::int32_t value);

    ///
    /// Appends a native int64_t as a BSON 64-bit signed integer.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(std::int64_t value);

    ///
    /// Appends an oid as a BSON ObjectId.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(oid const& value);

    ///
    /// Appends a decimal128 object as a BSON Decimal128.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(decimal128 value);

    ///
    /// Appends the given document view.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(bsoncxx::v_noabi::document::view view);

    ///
    /// Appends the given array view.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws
    ///   bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting for a
    ///   key to be appended to start a new key/value pair.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) append(bsoncxx::v_noabi::array::view view);

    ///
    /// Gets a view over the document.
    ///
    /// @return A document::view of the internal BSON.
    ///
    /// @pre
    ///    The top-level BSON datum should be a document that is not waiting for a key to be
    ///    appended to start a new key/value pair, and does contain any open sub-documents or open
    ///    sub-arrays.
    ///
    /// @throws bsoncxx::v_noabi::exception if the precondition is violated.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view) view_document() const;

    ///
    /// Gets a view over the array.
    ///
    /// @return An array::view of the internal BSON.
    ///
    /// @pre
    ///    The top-level BSON datum should be an array that does not contain any open sub-documents
    ///    or open sub-arrays.
    ///
    /// @throws bsoncxx::v_noabi::exception if the precondition is violated.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::array::view) view_array() const;

    ///
    /// Transfers ownership of the underlying document to the caller.
    ///
    /// @return A document::value with ownership of the document.
    ///
    /// @pre
    ///    The top-level BSON datum should be a document that is not waiting for a key to be
    ///    appended to start a new key/value pair, and does not contain any open sub-documents or
    ///    open sub-arrays.
    ///
    /// @throws bsoncxx::v_noabi::exception if the precondition is violated.
    ///
    /// @warning
    ///   After calling extract_document() it is illegal to call any methods on this class, unless
    ///   it is subsequenly moved into.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value) extract_document();

    ///
    /// Transfers ownership of the underlying document to the caller.
    ///
    /// @return A document::value with ownership of the document.
    ///
    /// @pre
    ///    The top-level BSON datum should be an array that does not contain any open sub-documents
    ///    or open sub-arrays.
    ///
    /// @throws bsoncxx::v_noabi::exception if the precondition is violated.
    ///
    /// @warning
    ///   After calling extract_array() it is illegal to call any methods on this class, unless it
    ///   is subsequenly moved into.
    ///
    BSONCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::array::value) extract_array();

    ///
    /// Deletes the contents of the underlying BSON datum. After calling clear(), the state of this
    /// class will be the same as it was immediately after construction.
    ///
    BSONCXX_ABI_EXPORT_CDECL(void) clear();

    ///
    /// A sub-binary must be opened by invoking @ref bsoncxx::v_noabi::builder::basic::sub_binary::allocate()
    ///
    core& open_binary() = delete;

    ///
    /// Closes the current sub-binary within this BSON datum.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @throws bsoncxx::v_noabi::exception if the binary contents were never allocated.
    ///
    BSONCXX_ABI_EXPORT_CDECL(core&) close_binary();

   private:
    std::unique_ptr<impl> _impl;
};

} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::core.
///
