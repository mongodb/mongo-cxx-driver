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

#include <bsoncxx/v1/oid.hpp> // IWYU pragma: export

//

#include <array>
#include <cstdint>

namespace bsoncxx {
namespace v1 {

class oid::internal {
   public:
    // Required by bsoncxx::v_noabi::oid::oid().
    static std::array<std::uint8_t, k_oid_length>& bytes(oid& o) {
        return o._bytes;
    }

    // Required by bsoncxx::v_noabi::oid::oid().
    static oid make_oid_for_overwrite() {
        return {oid::for_overwrite_tag{}};
    }
};

} // namespace v1
} // namespace bsoncxx
