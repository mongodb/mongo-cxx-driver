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

#include <memory>
#include <stdexcept>
#include <type_traits>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {

///
/// A low-level interface for constructing BSON documents and arrays.
///
/// @remark
///   Generally it is recommended to use the classes in builder::basic or
///   builder::stream instead of using this class directly. However, developers
///   who wish to write their own abstractions may find this class useful.
///
class BSONCXX_API core {
   public:
    class BSONCXX_PRIVATE impl;

    ///
    /// Constructs an empty BSON datum.
    ///
    /// @param is_array
    ///   true if the top-level BSON datum should be an array.
    ///
    explicit core(bool is_array);

    core(core&& rhs) noexcept;
    core& operator=(core&& rhs) noexcept;

    ~core();

    ///
    /// Appends a key passed as a non-owning stdx::string_view.
    ///
    /// @remark
    ///   Use key_owned() unless you know what you are doing.
    ///
    /// @warning
    ///   The caller must ensure that the lifetime of the backing
    ///   string extends until the next value is appended.
    ///
    /// @param key
    ///   A null-terminated array of characters.
    ///
    void key_view(stdx::string_view key);

    ///
    /// Appends a key passed as a STL string.
    /// Transfers ownership of the key to this class.
    ///
    /// @param key
    ///   A string key.
    ///
    void key_owned(std::string key);

    ///
    /// Opens a sub-document within this BSON datum.
    ///
    void open_document();

    ///
    /// Opens a sub-array within this BSON datum.
    ///
    void open_array();

    ///
    /// Closes the current sub-document within this BSON datum.
    ///
    void close_document();

    ///
    /// Closes the current sub-array within this BSON datum.
    ///
    void close_array();

    ///
    /// Appends the keys from a BSON document into this BSON datum.
    ///
    /// @note
    ///   This can be used with an array::view as well by converting
    ///   it to a document::view first.
    ///
    void concatenate(const document::view& view);

    ///
    /// Append a BSON double.
    ///
    void append(const types::b_double& value);

    ///
    /// Append a BSON UTF-8 string.
    ///
    void append(const types::b_utf8& value);

    ///
    /// Append a BSON document.
    ///
    void append(const types::b_document& value);

    ///
    /// Append a BSON array.
    ///
    void append(const types::b_array& value);

    ///
    /// Append a BSON binary datum.
    ///
    void append(const types::b_binary& value);

    ///
    /// Append a BSON undefined.
    ///
    void append(const types::b_undefined& value);

    ///
    /// Append a BSON ObjectId.
    ///
    void append(const types::b_oid& value);

    ///
    /// Append a BSON boolean.
    ///
    void append(const types::b_bool& value);

    ///
    /// Append a BSON date.
    ///
    void append(const types::b_date& value);

    ///
    /// Append a BSON null.
    ///
    void append(const types::b_null& value);

    ///
    /// Append a BSON regex.
    ///
    void append(const types::b_regex& value);

    ///
    /// Append a BSON DBPointer.
    ///
    void append(const types::b_dbpointer& value);

    ///
    /// Append a BSON JavaScript code.
    ///
    void append(const types::b_code& value);

    ///
    /// Append a BSON symbol.
    ///
    void append(const types::b_symbol& value);

    ///
    /// Append a BSON JavaScript code with scope.
    ///
    void append(const types::b_codewscope& value);

    ///
    /// Append a BSON 32-bit signed integer.
    ///
    void append(const types::b_int32& value);

    ///
    /// Append a BSON replication timestamp.
    ///
    void append(const types::b_timestamp& value);

    ///
    /// Append a BSON 64-bit signed integer.
    ///
    void append(const types::b_int64& value);

    ///
    /// Append a BSON min-key.
    ///
    void append(const types::b_minkey& value);

    ///
    /// Append a BSON max-key.
    ///
    void append(const types::b_maxkey& value);

    ///
    /// Append a BSON variant value.
    ///
    void append(const types::value& value);

    ///
    /// Append a STL string as a BSON UTF-8 string.
    ///
    void append(std::string str);

    ///
    /// Append a string view as a BSON UTF-8 string.
    ///
    void append(stdx::string_view str);

    ///
    /// Append a char* or const char*
    ///
    /// We disable all other pointer types to prevent the surprising implicit
    /// conversion to bool.
    ///
    template <typename T>
    BSONCXX_INLINE void append(T* v) {
        static_assert(std::is_same<typename std::remove_const<T>::type, char>::value,
                      "append is disabled for non-char pointer types");
        append(types::b_utf8{v});
    }

    ///
    /// Append a native boolean as a BSON boolean.
    ///
    void append(bool value);

    ///
    /// Append a native double as a BSON double.
    ///
    void append(double value);

    ///
    /// Append a native int32_t as a BSON 32-bit signed integer.
    ///
    void append(std::int32_t value);

    ///
    /// Append a native int64_t as a BSON 64-bit signed integer.
    ///
    void append(std::int64_t value);

    ///
    /// Append an oid as a BSON ObjectId.
    ///
    void append(const oid& value);

    ///
    /// Append the given document view.
    ///
    void append(document::view view);

    ///
    /// Append the given array view.
    ///
    void append(array::view view);

    ///
    /// Get a view over the document.
    ///
    /// @return A document::view of the internal BSON.
    ///
    document::view view_document() const;

    ///
    /// Get a view over the array.
    ///
    /// @return An array::view of the internal BSON.
    ///
    /// @warning
    ///   It is undefined behavior to call this method if the underlying BSON
    ///   datum is not a BSON array.
    ///
    array::view view_array() const;

    ///
    /// Transfer ownership of the underlying document to the caller.
    ///
    /// @return A document::value with ownership of the document.
    ///
    /// @warning
    ///  After calling extract_document() it is illegal to call any methods
    ///  on this class, unless it is subsequenly moved into.
    ///
    document::value extract_document();

    ///
    /// Transfer ownership of the underlying document to the caller.
    ///
    /// @return A document::value with ownership of the document.
    ///
    /// @warning
    ///   It is undefined behavior to call this method if the underlying BSON
    ///   datum is not a BSON array.
    ///
    /// @warning
    ///  After calling extract_array() it is illegal to call any methods
    ///  on this class, unless it is subsequenly moved into.
    ///
    array::value extract_array();

    ///
    /// Delete the contents of the underlying BSON datum. After calling clear the
    /// state of this class will be the same as it was immediately after construction.
    ///
    void clear();

   private:
    std::unique_ptr<impl> _impl;
};

}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
