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

#include <bsoncxx/v1/document/value.hpp>

//

#include <cstdint>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {
namespace document {

static_assert(is_regular<value>::value, "bsoncxx::v1::document::value must be regular");
static_assert(is_nothrow_moveable<value>::value, "bsoncxx::v1::document::value must be nothrow moveable");

void value::noop_deleter(std::uint8_t*) { /* noop */ }

} // namespace document
} // namespace v1
} // namespace bsoncxx
