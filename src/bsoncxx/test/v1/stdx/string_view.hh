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

#include <bsoncxx/v1/stdx/string_view.hpp>

//

#include <bsoncxx/v1/config/config.hpp>

#include <string>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_tostring.hpp>

#if defined(BSONCXX_POLY_USE_IMPLS)

template <>
struct Catch::StringMaker<bsoncxx::v1::stdx::string_view> {
    static std::string convert(bsoncxx::v1::stdx::string_view const& value) {
        return bsoncxx::test::stringify(std::string{value});
    }
};

#endif
