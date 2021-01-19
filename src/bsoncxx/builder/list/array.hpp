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
        _core = make_array(std::move(_core), std::forward<Args>(args)...);
    }

    core make_array(core&& core) {
        return std::move(core);
    }

    template <typename Arg, typename... Args>
    core make_array(core&& core, Arg&& a, Args&&... args) {
        bson_value::value v{a};
        core.append(v);
        return std::move(make_array(std::move(core), std::forward<Args>(args)...));
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
        return extract();
    }

    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    array& operator+=(const array& rhs) {
        this->append(rhs);
        return *this;
    }

    array& operator+=(array&& rhs) {
        this->append(std::move(rhs));
        return *this;
    }

    array& append(const array& rhs) {
        _core.concatenate(rhs._core.view_array());
        return *this;
    }

    array& append(array&& rhs) {
        _core.concatenate(rhs.extract().view());
        return *this;
    }

   private:
    core _core{true};
};

}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
