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

#include <bsoncxx/v1/oid.hpp>

//

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {

static_assert(is_regular<oid>::value, "bsoncxx::v1::oid must be regular");
static_assert(is_semitrivial<oid>::value, "bsoncxx::v1::oid must be semitrivial");

} // namespace v1
} // namespace bsoncxx
