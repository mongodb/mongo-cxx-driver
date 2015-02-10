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

#include <bsoncxx/config/prelude.hpp>

#include <cstdlib>
#include <memory>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace array {

class BSONCXX_API value {

   public:
    using deleter_type = void(*)(std::uint8_t*);
    using unique_ptr_type = std::unique_ptr<uint8_t, deleter_type>;

    value(std::uint8_t* data, std::size_t length, deleter_type dtor);
    value(unique_ptr_type ptr, std::size_t length);
    explicit value(array::view view);

    value(const value&);
    value& operator=(const value&);

    value(value&&) = default;
    value& operator=(value&&) = default;

    inline array::view view() const noexcept;
    inline operator array::view() const noexcept;

    unique_ptr_type release();

   private:
    unique_ptr_type _data;
    std::size_t _length;
};

array::view value::view() const noexcept {
    return array::view{static_cast<uint8_t*>(_data.get()), _length};
}

value::operator array::view() const noexcept {
    return view();
}

}  // namespace array
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
