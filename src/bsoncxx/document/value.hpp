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

#include <bsoncxx/document/view.hpp>

namespace mongo {
namespace bson {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace document {

class LIBBSONCXX_API value {
   using deleter_type = void(*)(void*);

   public:
    value(std::uint8_t* data, std::size_t length, deleter_type dtor);
    explicit value(document::view view);

    value(const value& rhs);
    value& operator=(const value& rhs);

    inline document::view view() const noexcept;
    inline operator document::view() const noexcept;

   private:
    std::unique_ptr<void, deleter_type> _data;
    std::size_t _length;

};

document::view value::view() const noexcept {
    return document::view{static_cast<uint8_t*>(_data.get()), _length};
}

value::operator document::view() const noexcept {
    return view();
}

}  // namespace document
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bson
}  // namespace mongo

#include <bsoncxx/config/postlude.hpp>
