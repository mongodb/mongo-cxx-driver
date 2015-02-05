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

#include <cstdlib>
#include <cstring>

#include <bsoncxx/document/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace document {

value::value(std::uint8_t* data, std::size_t length, deleter_type dtor)
    : _data(static_cast<void*>(data), dtor), _length(length) {
}

value::value(document::view view)
    : _data(operator new(static_cast<std::size_t>(view.length())), operator delete),
      _length(view.length()) {
    std::copy(view.data(), view.data() + view.length(), static_cast<uint8_t*>(_data.get()));
}

value::value(const value& rhs) : value(rhs.view()) {
}

value& value::operator=(const value& rhs) {
    *this = std::move(value{rhs.view()});

    return *this;
}

}  // namespace document
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
