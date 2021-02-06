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

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace list {

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
    /// Default constructor. Creates an empty array builder.
    ///
    array() = default;

    ///
    /// Converting constructor. Initializes each element of the array builder with the corresponding
    /// value in std::forward<Args>(args).
    ///
    /// @param args values used to initialize each element of the array builder
    ///
    /// @note a bsoncxx::types::bson_value::value is direct-initialized from each argument.
    ///
    /// @see bsoncxx::types::bson_value::value
    ///
    template <typename... Args>
    array(Args&&... args) {
        this->_append(std::forward<Args>(args)...);
    }

    ///
    /// Move constructor. Constructs the array builder with the contents of other using move
    /// semantics.
    ///
    /// @param other another array builder to use as source to initialize this array builder with
    ///
    array(array&& other) noexcept = default;

    ///
    /// Replaces the contents with those of other using move semantics.
    ///
    /// @param other another array builder to use as source to initialize this array builder with
    ///
    array& operator=(array&& other) noexcept = default;

    ///
    /// Returns an owning bsoncxx::array::value.
    ///
    /// @return An owning array::value representing the entire contents of the array builder.
    ///
    /// @see bsoncxx::array::value
    ///
    /// @warning
    ///   After calling this method it is illegal to call any methods on this class, unless
    ///   it is subsequently moved into.
    ///
    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    ///
    /// Appends array builder rhs.
    ///
    /// @param rhs array builder to append
    ///
    /// @return *this
    ///
    array& operator+=(array&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    ///
    /// Appends array builder rhs.
    ///
    /// @param rhs array builder to append
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
        _core.append(types::bson_value::value{rhs});
        return *this;
    }

    ///
    /// Concatenates array builder rhs.
    ///
    /// @param rhs array builder to concatenate
    ///
    /// @return *this
    ///
    array& concatenate(array&& rhs) {
        _core.concatenate(rhs.extract().view());
        return *this;
    }

   private:
    void _append() {}  // base-case, no-op

    // appends each element in args to core
    template <typename Arg, typename... Args>
    void _append(Arg&& a, Args&&... args) {
        _core.append(types::bson_value::value{std::forward<Arg>(a)});
        _append(std::forward<Args>(args)...);
    }

    core _core{true};
};

///
/// Returns an array builder containing elements from lhs followed by the elements from rhs.
///
/// @param lhs array builder
///
/// @param rhs array builder
///
/// @return An array builder containing elements from lhs followed by the elements from rhs
///
array operator+(array&& lhs, array&& rhs) {
    lhs.concatenate(std::move(rhs));
    return std::forward<array>(lhs);
}

}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
