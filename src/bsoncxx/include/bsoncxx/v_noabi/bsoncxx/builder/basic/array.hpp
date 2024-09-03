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

#include <bsoncxx/builder/basic/array-fwd.hpp>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/impl.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/builder/core.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace basic {

///
/// A traditional builder-style interface for constructing
/// a BSON array.
///
class array : public sub_array {
   public:
    ///
    /// Default constructor
    ///
    array() : sub_array(&_core), _core(true) {}

    ///
    /// Destructor
    ///
    ~array() = default;

    ///
    /// Move constructor
    ///
    array(array&& arr) noexcept : sub_array(&_core), _core(std::move(arr._core)) {}

    ///
    /// Move assignment operator
    ///
    array& operator=(array&& arr) noexcept {
        _core = std::move(arr._core);
        return *this;
    }

    array(const array&) = delete;
    array& operator=(const array&) = delete;

    ///
    /// @return A view of the BSON array.
    ///
    bsoncxx::v_noabi::array::view view() const {
        return _core.view_array();
    }

    ///
    /// Conversion operator that provides a view of the current builder
    /// contents.
    ///
    /// @return A view of the current builder contents.
    ///
    operator bsoncxx::v_noabi::array::view() const {
        return view();
    }

    ///
    /// Transfer ownership of the underlying array to the caller.
    ///
    /// @return An array::value with ownership of the array.
    ///
    /// @warning
    ///  After calling extract() it is illegal to call any methods
    ///  on this class, unless it is subsequenly moved into.
    ///
    bsoncxx::v_noabi::array::value extract() {
        return _core.extract_array();
    }

    ///
    /// Reset the underlying BSON to an empty array.
    ///
    void clear() {
        _core.clear();
    }

   private:
    core _core;
};

///
/// Creates an array from a list of elements.
///
/// @param args
///   A variadiac list of elements. The types of the elements can be anything that
///   builder::basic::sub_array::append accepts.
///
/// @return
///   A bsoncxx::v_noabi::array::value containing the elements.
///
template <typename... Args>
bsoncxx::v_noabi::array::value make_array(Args&&... args) {
    array array;
    array.append(std::forward<Args>(args)...);
    return array.extract();
}

}  // namespace basic
}  // namespace builder
}  // namespace v_noabi
}  // namespace bsoncxx

namespace bsoncxx {
namespace builder {
namespace basic {

using ::bsoncxx::v_noabi::builder::basic::make_array;

}  // namespace basic
}  // namespace builder
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::basic::array.
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace builder {
namespace basic {

/// @ref bsoncxx::v_noabi::builder::basic::make_array
template <typename... Args>
v_noabi::array::value make_array(Args&&... args);

}  // namespace basic
}  // namespace builder
}  // namespace bsoncxx

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
