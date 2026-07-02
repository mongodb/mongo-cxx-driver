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

#include <utility> // IWYU pragma: export

//

namespace mongocxx {

#if defined(__cpp_lib_exchange_function) && __cpp_lib_exchange_function >= 201304L
using std::exchange;
#else
template <typename T, typename U = T>
T exchange(T& obj, U&& new_val) {
    T old_val = std::move(obj);
    obj = std::forward<U>(new_val);
    return old_val;
}
#endif

} // namespace mongocxx
