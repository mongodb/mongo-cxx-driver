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

#include <bsoncxx/private/version.hh>

//

#include <string>
#include <vector>

#include <bsoncxx/stdx/string_view.hpp>

namespace bsoncxx {
namespace test_util {

std::vector<int> split_version(bsoncxx::stdx::string_view input) {
    static constexpr auto npos = bsoncxx::stdx::string_view::npos;
    static constexpr auto digits = "0123456789";

    std::vector<int> ret;

    std::size_t pos;

    while ((pos = input.find_first_not_of(digits)) != npos) {
        auto const str = input.substr(0u, pos);

        // No more digits.
        if (str.empty()) {
            return ret;
        }

        ret.push_back(std::stoi(static_cast<std::string>(str)));

        input.remove_prefix(str.size());
        input.remove_prefix(input.empty() ? 0u : 1u); // Delimiter.
    }

    // Last group of digits.
    if ((pos = input.find_last_of(digits)) != npos) {
        auto const str = input.substr(0u, pos < input.size() ? pos + 1u : pos);
        ret.push_back(std::stoi(static_cast<std::string>(str)));
    }

    return ret;
}

} // namespace test_util
} // namespace bsoncxx
