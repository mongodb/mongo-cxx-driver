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

#include <cstdlib>
#include <memory>

#include <bsoncxx/array/value-fwd.hpp>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

///
/// A read-only BSON array that owns its underlying buffer.
///
/// When a array::value goes out of scope, the underlying buffer is freed. Generally this class
/// should be used sparingly; array::view should be used instead wherever possible.
///
class value {
   public:
    using deleter_type = void(BSONCXX_ABI_CDECL*)(std::uint8_t*);
    using unique_ptr_type = std::unique_ptr<uint8_t[], deleter_type>;

    ///
    /// Constructs a value from a buffer.
    /// This constructor transfers ownership of the buffer to the resulting
    /// value. A user-provided deleter is used to destroy the buffer.
    ///
    /// @param data
    ///   A pointer to a buffer containing a valid BSON array.
    /// @param length
    ///   The length of the document.
    /// @param dtor
    ///   A user provided deleter.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(std::uint8_t* data, std::size_t length, deleter_type dtor);

    ///
    /// Constructs a value from a std::unique_ptr to a buffer. The ownership
    /// of the buffer is transferred to the resulting value.
    ///
    /// @param ptr
    ///   A pointer to a buffer containing a valid BSON array.
    /// @param length
    ///   The length of the document.
    ///
    BSONCXX_ABI_EXPORT_CDECL() value(unique_ptr_type ptr, std::size_t length);

    ///
    /// Constructs a value from a view of an array. The data referenced
    /// by the array::view will be copied into a new buffer managed by the
    /// constructed value.
    ///
    /// @param view
    ///   A view of another array to copy.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() value(array::view view);

    BSONCXX_ABI_EXPORT_CDECL() value(value const&);
    BSONCXX_ABI_EXPORT_CDECL(value&) operator=(value const&);

    value(value&&) = default;
    value& operator=(value&&) = default;

    ///
    /// Get a view over the document owned by this value.
    ///
    array::view view() const noexcept {
        return array::view{static_cast<uint8_t*>(_data.get()), _length};
    }

    ///
    /// Conversion operator that provides a view given a value.
    ///
    /// @return A view over the value.
    ///
    operator array::view() const noexcept {
        return view();
    }

    ///
    /// Transfer ownership of the underlying buffer to the caller.
    ///
    /// @warning
    ///   After calling release() it is illegal to call any methods
    ///   on this class, unless it is subsequently moved into.
    ///
    /// @return A std::unique_ptr with ownership of the buffer.
    ///
    BSONCXX_ABI_EXPORT_CDECL(unique_ptr_type) release();

   private:
    unique_ptr_type _data;
    std::size_t _length{0};
};

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::array::value.
///
