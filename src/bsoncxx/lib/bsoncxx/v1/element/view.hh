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

#include <bsoncxx/v1/element/view.hpp>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>

#include <bsoncxx/private/bson.hh>

namespace bsoncxx {
namespace v1 {
namespace element {

class view::internal {
   public:
    static view make(
        std::uint8_t const* raw,
        std::uint32_t length,
        std::uint32_t offset,
        std::uint32_t keylen,
        bool is_valid = true);

    static v1::stdx::optional<bson_iter_t> to_bson_iter(view const& v);

   private:
    friend view;

    static view::impl const& impl(view const& v);

    static view::impl const* impl(view const* v);
    static view::impl* impl(view* v);
};

v1::stdx::optional<bson_iter_t> to_bson_iter(view const& v);

} // namespace element
} // namespace v1
} // namespace bsoncxx
