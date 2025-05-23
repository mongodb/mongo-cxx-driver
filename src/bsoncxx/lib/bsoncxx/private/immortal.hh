// Copyright 2009-present MongoDB, Inc.
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

#include <bsoncxx/v1/detail/macros.hpp>

#include <new>

namespace bsoncxx {

//
// Used to construct an object whose destructor is never invoked (hence, "immortal").
//
// This is primarily to avoid generating exit-time destructors for error category objects:
//
// ```cpp
// std::error_category const& name() noexcept {
//     class type final : public std::error_category { ... };
//     static bsoncxx::immortal<type> const instance;
//     return instance.value();
// }
// ```
//
template <typename T>
class immortal {
   private:
    alignas(T) unsigned char _storage[sizeof(T)];

   public:
    ~immortal() = default;
    immortal(immortal&&) = delete;
    immortal& operator=(immortal&&) = delete;
    immortal(immortal const&) = delete;
    immortal& operator=(immortal const&) = delete;

    template <typename... Args>
    explicit immortal(Args&&... args) {
        new (_storage) T(BSONCXX_PRIVATE_FWD(args)...);
    }

    T const& value() const {
        return *reinterpret_cast<T const*>(_storage);
    }

    T& value() {
        return *reinterpret_cast<T*>(_storage);
    }
};

} // namespace bsoncxx
