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

#include "driver/config/prelude.hpp"

#include <memory>

#include "bson/document/view.hpp"

namespace bson {
namespace document {

class LIBMONGOCXX_EXPORT value {

   public:
    value(const std::uint8_t* b, std::size_t l, void (*dtor)(void*) = free);
    value(const view& view);

    document::view view() const;
    operator document::view() const;

   private:
    std::unique_ptr<void, void (*)(void*)> _buf;
    std::size_t _len;

};

}  // namespace document
}  // namespace bson

#include "driver/config/postlude.hpp"
