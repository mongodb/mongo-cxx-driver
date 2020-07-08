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

#include <memory>

#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace types {
namespace bson_value {

///
/// A variant owning type that represents any BSON type. Owns its underlying
/// buffer. When a bson_value::value goes out of scope, its underlying
/// buffer is freed.
///
/// For accessors into this type and to extract the various BSON types out,
/// please use bson_value::view.
///
/// @relatesalso bson_value::view
///
class BSONCXX_API value {
   public:
    ~value();

    value(const value&);
    value& operator=(const value&);

    value(value&&) noexcept;
    value& operator=(value&&) noexcept;

    ///
    /// Create an owning copy of a bson_value::view.
    ///
    explicit value(const view&);

    ///
    /// Get a view over the bson_value owned by this object.
    ///
    bson_value::view view() const noexcept;

    ///
    /// Conversion operator that provides a bson_value::view given a bson_value::value.
    ///
    operator bson_value::view() const noexcept;

   private:
    friend class bsoncxx::document::element;

    value(const std::uint8_t* raw,
          std::uint32_t length,
          std::uint32_t offset,
          std::uint32_t keylen);

    // Makes a copy of 'internal_value' and owns the copy.
    value(void* internal_value);

    friend value make_owning_bson(void* internal_value);

    class BSONCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;
};

///
/// @{
///
/// Compares values for (in)-equality.
///
/// @relates bson_value::value
///
BSONCXX_INLINE bool operator==(const value& lhs, const value& rhs) {
    return (lhs.view() == rhs.view());
}

BSONCXX_INLINE bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}

///
/// @}
///

///
/// @{
///
/// Compares a value with a view for (in)-equality.
///
/// @relates bson_value::value
///
BSONCXX_INLINE bool operator==(const value& lhs, const view& rhs) {
    return (lhs.view() == rhs);
}

BSONCXX_INLINE bool operator==(const view& lhs, const value& rhs) {
    return (rhs == lhs);
}

BSONCXX_INLINE bool operator!=(const value& lhs, const view& rhs) {
    return !(lhs == rhs);
}

BSONCXX_INLINE bool operator!=(const view& lhs, const value& rhs) {
    return !(lhs == rhs);
}

///
/// @}
///

}  // namespace bson_value
}  // namespace types

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
