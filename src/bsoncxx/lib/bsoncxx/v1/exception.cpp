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

#include <bsoncxx/v1/exception.hpp>

//

#include <string>

#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v1 {

std::error_category const& source_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::source_errc";
        }

        std::string message(int e) const noexcept override {
            using code = v1::source_errc;

            switch (static_cast<code>(e)) {
                case code::zero:
                    return "zero";
                case code::bsoncxx:
                    return "bsoncxx";
                case code::bson:
                    return "bson";
                default:
                    return "unknown: " + std::to_string(e);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::error_category const& type_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "bsoncxx::v1::type_errc";
        }

        std::string message(int e) const noexcept override {
            using code = v1::type_errc;

            switch (static_cast<code>(e)) {
                case code::zero:
                    return "zero";
                case code::invalid_argument:
                    return "invalid argument";
                case code::runtime_error:
                    return "runtime error";
                default:
                    return "unknown: " + std::to_string(e);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {

exception::~exception() = default;

} // namespace v1
} // namespace bsoncxx
