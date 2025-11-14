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

#include <bsoncxx/test/v1/element/view.hh>

//

#include <bsoncxx/test/v1/array/view.hh>
#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/exception.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <string>

#include <bsoncxx/test/stringify.hh>

std::string Catch::StringMaker<bsoncxx::v1::element::view>::convert(bsoncxx::v1::element::view const& value) try {
    if (!value) {
        return "invalid";
    }

    std::string res;
    res += bsoncxx::test::stringify(value.key());
    res += ": ";
    res += bsoncxx::test::stringify(value.type_view());
    return res;
} catch (bsoncxx::v1::exception const& ex) {
    using code = bsoncxx::v1::element::view::errc;

    if (ex.code() == code::invalid_view || ex.code() == code::invalid_data) {
        return "invalid";
    } else {
        throw;
    }
}
