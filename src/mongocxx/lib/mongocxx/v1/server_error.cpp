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

#include <mongocxx/v1/server_error.hpp>

namespace mongocxx {
namespace v1 {

class server_error::impl {};

// Prevent vague linkage of the vtable and type_info object (-Wweak-vtables).
// - https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-vtable
//   > The key function is the first non-pure virtual function that is not inline at the point of class definition.
// - https://lld.llvm.org/missingkeyfunction:
//   > It’s always advisable to ensure there is at least one eligible function that can serve as the key function.
// - https://gcc.gnu.org/onlinedocs/gcc/Vague-Linkage.html
//   > For polymorphic classes (classes with virtual functions), the ‘type_info’ object is written out along with the vtable.
void server_error::key_function() const {}

} // namespace v1
} // namespace mongocxx
