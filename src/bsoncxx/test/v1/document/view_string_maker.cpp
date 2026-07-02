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

#include <bsoncxx/test/v1/document/view.hh>

//

#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/types/view.hh> // IWYU pragma: keep: Catch::StringMaker<bsoncxx::v1::types::view>

#include <string>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>

std::string Catch::StringMaker<bsoncxx::v1::document::view>::convert(bsoncxx::v1::document::view const& value) try {
    if (!value) {
        return "invalid";
    }

    auto const end = value.end();
    auto iter = value.begin();

    if (iter == end) {
        return "{}";
    }

    std::string res;
    res += '{';
    res += bsoncxx::test::stringify(*iter);
    for (++iter; iter != end; ++iter) {
        res += ", ";
        res += bsoncxx::test::stringify(*iter);
    }
    res += '}';
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    WARN("exception during stringification: " << ex.what());
    if (ex.code() == bsoncxx::v1::document::view::errc::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}
