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
    /// Creates an empty array.
    ///
    array() = default;

    ///
    /// Creates a BSON array.
    ///
    /// @param init
    ///     the initializer list used to construct the BSON array
    ///
    /// @see bsoncxx::builder::list
    /// @see bsoncxx::builder::document
    ///
    template <typename... Args>
    array(Args&&... args) {
        _append(_core, std::forward<Args>(args)...);
    }

    array(array&& other) = default;
    array& operator=(array&& other) = default;

    array(const array& other) {
        for (auto&& value : other._core.view_array())
            _core.append(bson_value::value{value.get_value()});
    }

    array& operator=(const array& other) {
        if (this != &other)
            *this = array(other);
        return *this;
    }

    operator bsoncxx::array::value() {
        return this->extract();
    }

    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    array& operator+=(const array& rhs) {
        this->append(rhs);
        return *this;
    }

    array& operator+=(list::array&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    array& append(const array& rhs) {
        auto temp = rhs;
        _core.append(temp.extract());
        return *this;
    }

    array& append(array&& rhs) {
        _core.append(rhs.extract());
        return *this;
    }

    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& operator+=(T&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& operator+=(const T& rhs) {
        this->append(rhs);
        return *this;
    }

    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& append(T&& rhs) {
        _core.append(bson_value::value{rhs});
        return *this;
    }

    template <typename T, enable_if_t<!std::is_same<decay_t<T>, array>::value, int> = 0>
    array& append(const T& rhs) {
        _core.append(bson_value::value{rhs});
        return *this;
    }

   private:
    void _append(core&) {}

    template <typename Arg, typename... Args>
    void _append(core& core, Arg&& a, Args&&... args) {
        core.append(bson_value::value{a});
        _append(core, std::forward<Args>(args)...);
    }

    core _core{true};
};

}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
