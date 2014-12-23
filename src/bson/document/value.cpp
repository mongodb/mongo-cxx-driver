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

#include "bson/document/value.hpp"

namespace bson {
namespace document {

value::value(const std::uint8_t* b, std::size_t l, void (*dtor)(void*))
    : _buf((void*)b, dtor), _len(l) {}

value::value(const document::view& view)
    : _buf(malloc((std::size_t)view.get_len()), free), _len(view.get_len()) {
    std::memcpy(_buf.get(), view.get_buf(), view.get_len());
}

document::view value::view() const { return document::view{(uint8_t*)_buf.get(), _len}; }

value::operator document::view() const { return view(); }

}  // namespace document
}  // namespace bson
