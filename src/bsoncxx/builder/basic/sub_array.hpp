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

#include <bsoncxx/builder/core.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace basic {

namespace impl {

template <typename T>
void value_append(core* core, T&& t);

}  // namespace impl

class BSONCXX_API sub_array {
   public:
    sub_array(core* core) : _core(core) {
    }

    template <typename Arg, typename... Args>
    void append(Arg&& a, Args&&... args) {
        append(std::forward<Arg>(a));
        append(std::forward<Args>(args)...);
    }

    template <typename T>
    void append(T&& t) {
        impl::value_append(_core, std::forward<T>(t));
    }

   private:
    core* _core;
};

}  // namespace basic
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
