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

#include <bsoncxx/array/value.hpp>

#include <cstdlib>
#include <cstring>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace array {

value::value(std::uint8_t* data, std::size_t length, deleter_type dtor)
    : _data(data, dtor), _length(length) {}

value::value(unique_ptr_type ptr, std::size_t length) : _data(std::move(ptr)), _length(length) {}

namespace {

void uint8_t_deleter(std::uint8_t* ptr) {
    delete[] ptr;
}

}  // namespace

value::value(array::view view)
    : _data(new std::uint8_t[static_cast<std::size_t>(view.length())], uint8_t_deleter),
      _length(view.length()) {
    std::copy(view.data(), view.data() + view.length(), _data.get());
}

value::value(const value& rhs) : value(rhs.view()) {}

value& value::operator=(const value& rhs) {
    *this = value{rhs.view()};
    return *this;
}

value::unique_ptr_type value::release() {
    _length = 0;
    return std::move(_data);
}

}  // namespace array
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
