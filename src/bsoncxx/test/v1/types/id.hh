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

#pragma once

#include <bsoncxx/v1/types/id.hpp> // IWYU pragma: export

//

#include <bsoncxx/test/stringify.hh>

template <>
struct Catch::StringMaker<bsoncxx::v1::types::id> {
    static std::string convert(bsoncxx::v1::types::id const& value) {
        return "k_" + to_string(value);
    }
};

template <>
struct Catch::StringMaker<bsoncxx::v1::types::binary_subtype> {
    static std::string convert(bsoncxx::v1::types::binary_subtype const& value) {
        return "k_" + to_string(value);
    }
};
