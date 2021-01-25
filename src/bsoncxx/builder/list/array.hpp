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
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace list {
using namespace bsoncxx::types;

///
/// A JSON-like builder for creating BSON arrays.
///
class array {
    template <bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;

    template <class T>
    using decay_t = typename std::decay<T>::type;

   public:
    ///
    /// Default constructor. Creates an empty array.
    ///
    array() = default;

    ///
    /// Converting constructor. Initializes each element of the array with the corresponding value
    /// in std::forward<Args>(args).
    ///
    /// @param args values used to initialize each element of the array
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from each argument.
    ///
    /// @see bsoncxx::types::bson_value::value
    ///
    template <typename... Args>
    array(Args&&... args) {
        this->_append(_core, std::forward<Args>(args)...);
    }

    ///
    /// Move constructor. Constructs the array with the contents of other using move semantics.
    ///
    /// @param other another array to use as source to initialize the array with
    ///
    array(array&& other) noexcept = default;

    ///
    /// Replaces the contents with those of other using move semantics.
    ///
    /// @param other another array to use as source to initialize the array with
    ///
    array& operator=(array&& other) noexcept = default;

    ///
    /// Returns an owning bsoncxx::array::value.
    ///
    /// @return An owning array::value representing the entire contents of the array.
    ///
    /// @see bsoncxx::array::value
    ///
    operator bsoncxx::array::value() {
        return this->extract();
    }

    ///
    /// Returns an owning bsoncxx::array::value.
    ///
    /// @return An owning array::value representing the entire contents of the array.
    ///
    /// @see bsoncxx::array::value
    ///
    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    ///
    /// Appends array rhs.
    ///
    /// @param rhs array to append
    ///
    /// @return *this
    ///
    array& operator+=(array&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    ///
    /// Appends array rhs.
    ///
    /// @param rhs array to append
    ///
    /// @return *this
    ///
    array& append(array&& rhs) {
        _core.append(rhs.extract());
        return *this;
    }

    ///
    /// Appends the value rhs.
    ///
    /// @param rhs value to append
    ///
    /// @return *this
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from rhs.
    ///
    /// @see bsoncxx::types::bson_value::value
    ///
    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& operator+=(const T& rhs) {
        this->append(rhs);
        return *this;
    }

    ///
    /// Appends the value rhs.
    ///
    /// @param rhs value to append
    ///
    /// @return *this
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from rhs.
    ///
    /// @see bsoncxx::types::bson_value::value
    ///
    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& append(const T& rhs) {
        _core.append(bson_value::value{rhs});
        return *this;
    }

    ///
    /// Concatenates array rhs.
    ///
    /// @param rhs array to concatenate
    ///
    /// @return *this
    ///
    array& concatenate(array&& rhs) {
        _core.concatenate(rhs.extract().view());
        return *this;
    }

   private:
    void _append(core&) {}  // base-case, no-op

    // appends each element in args to core
    template <typename Arg, typename... Args>
    void _append(core& core, Arg&& a, Args&&... args) {
        _append(core.append(bson_value::value{a}), std::forward<Args>(args)...);
    }

    core _core{true};
};

///
/// Returns an array containing elements from lhs followed by the elements from rhs.
///
/// @param lhs array
///
/// @param rhs array
///
/// @return An array containing elements from lhs followed by the elements from rhs
///
array operator+(array&& lhs, array&& rhs) {
    lhs.concatenate(std::move(rhs));
    return std::forward<array>(lhs);
}

}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
