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

#include <mongocxx/v1/exception.hpp>

//

#include <string>
#include <system_error>

#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/type_traits.hh>

namespace mongocxx {
namespace v1 {

std::error_category const& source_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::source_errc";
        }

        std::string message(int v) const noexcept override {
            using code = v1::source_errc;

            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::mongocxx:
                    return "mongocxx";
                case code::mongoc:
                    return "mongoc";
                case code::mongocrypt:
                    return "mongocrypt";
                case code::server:
                    return "server";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::error_category const& type_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::type_errc";
        }

        std::string message(int v) const noexcept override {
            using code = v1::type_errc;

            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_argument:
                    return "invalid argument";
                case code::runtime_error:
                    return "runtime error";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

exception::~exception() = default;

} // namespace v1
} // namespace mongocxx
