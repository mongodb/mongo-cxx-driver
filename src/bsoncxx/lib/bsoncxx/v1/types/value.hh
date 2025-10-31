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

#include <bsoncxx/v1/types/value.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>

#include <bsoncxx/private/bson.hh>

namespace bsoncxx {
namespace v1 {
namespace types {

class value::internal {
   public:
    static v1::stdx::optional<value>
    make(std::uint8_t const* raw, std::uint32_t length, std::uint32_t offset, std::uint32_t keylen);

    // Required by mongocxx::detail::scoped_bson_value.
    static BSONCXX_ABI_EXPORT_CDECL(bson_value_t&) get_bson_value(value& v);
};

} // namespace types
} // namespace v1
} // namespace bsoncxx
