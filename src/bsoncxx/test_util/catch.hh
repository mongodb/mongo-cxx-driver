// Copyright 2017 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include "catch.hpp"

#include <bsoncxx/config/private/prelude.hh>

namespace Catch {
using namespace bsoncxx;

// Catch2 must be able to stringify documents, optionals, etc. if they're used in Catch2 macros.
template <>
struct StringMaker<bsoncxx::document::view> {
    static std::string convert(const bsoncxx::document::view& value) {
        return bsoncxx::to_json(value, ExtendedJsonMode::k_relaxed);
    }
};

template <typename T>
struct StringMaker<stdx::optional<T>> {
    static std::string convert(const bsoncxx::stdx::optional<T>& value) {
        if (value) {
            StringMaker<T>::convert(value.value());
        }

        return "{nullopt}";
    }
};

template <>
struct StringMaker<stdx::optional<bsoncxx::document::view>> {
    static std::string convert(const bsoncxx::stdx::optional<bsoncxx::document::view>& value) {
        if (value) {
            return StringMaker::convert(value.value());
        }

        return "{nullopt}";
    }
};
}  // namespace Catch

#include <bsoncxx/config/private/postlude.hh>
